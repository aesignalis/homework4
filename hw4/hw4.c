#define _GNU_SOURCE
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUMS_PRODUCERS 10
#define NUMS_PER_PRODUCER 500
#define NUMS_CONSUMERS 20
#define NUMS_PER_CONSUMER 250
#define MAX_VALUE 1000
#define PROGRESS_STEP 50
#define OUTPUT_FILE "average.txt"

_Static_assert(NUMS_PRODUCERS * NUMS_PER_PRODUCER == 
        NUMS_CONSUMERS * NUMS_PER_CONSUMER, "produced and consumed counts must match");
_Static_assert(NUMS_PER_PRODUCER <= MAX_VALUE + 1,
    "cannot draw that many unique values from the range");

//primivtives
static pthread_mutex_t print_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t pipe_write_lock = PTHREAD_MUTEX_INITIALIZER;
static sem_t producers_done;
static sem_t pipe_read_sem;

static int write_full(int fd, const void *buf, size_t n){
    const char *p = buf;
    while(n>0){
        ssize_t w = write(fd, p, n);
        if(w<0){
            if(errno==EINTR){
                continue;
            }
        return -1;
        }
        p+=w;
        n-=(size_t)w;
    }
    return 0;
}

static int read_full(int fd, void *buf, size_t n){
    char *p = buf;
    while(n>0){
        ssize_t r = read(fd, p, n);
        if(r<0){
            if(errno==EINTR){
                continue;
            }
            return -1;
        }
        if(r==0) return 1;
        p+=r;
        n-=(size_t)r;
    }
    return 0;
}

static void sem_wait_retry(sem_t *s){
    while(sem_wait(s)==-1 && errno==EINTR)
    ;
}

//parent
typedef struct{
    int id;
    int write_fd;
} producer_arg_t;

static void *producer(void *arg){
    producer_arg_t *a=arg;
    unsigned int seed=(unsigned int)time(NULL) ^ ((unsigned int)a->id * 2654435761u);

    //shufle
    int shuf[MAX_VALUE+1];
    for(int i=0;i<=MAX_VALUE; i++){
        shuf[i]=i;
    }
    for(int i=0; i<NUMS_PER_PRODUCER;i++){
        int j=i + (int)(rand_r(&seed)%(unsigned)(MAX_VALUE+1-i));
        int temp=shuf[i]; shuf[i]=shuf[j]; shuf[j]=temp;
    }

    for(int i=0;i<NUMS_PER_PRODUCER;i++){
        int value=shuf[i];

    pthread_mutex_lock(&pipe_write_lock);
    int rc = write_full(a->write_fd, &value, sizeof value);
    pthread_mutex_unlock(&pipe_write_lock);
    if(rc!=0){
        perror("producer: write");
        break;
     }

    //bonus progress indicator here
    int written=i+1;
    if(written%PROGRESS_STEP==0){
	pthread_mutex_lock(&print_lock);
	printf("[Producer %2d] progress: %3d%% (%d/%d writes)\n",
		a->id, written*100 / NUMS_PER_PRODUCER,
		written, NUMS_PER_PRODUCER);
	fflush(stdout);
	pthread_mutex_unlock(&print_lock);
    }
}

//finished segment
pthread_mutex_lock(&print_lock);
printf("[Producer %2d] finished (thread ID %lu)\n",
    a->id, (unsigned long)pthread_self());
fflush(stdout);
pthread_mutex_unlock(&print_lock);

sem_post(&producers_done);
return NULL;
}

//children
typedef struct{
    int id;
    int read_fd;
    long sum;
    int all_read;
} consumer_arg_t;

static void *consumer(void *arg){
    consumer_arg_t *a = arg;
    long sum=0;
    a->all_read=1;

    for(int i=0;i<NUMS_PER_CONSUMER; i++){
        int value;

        sem_wait_retry(&pipe_read_sem);
        int rc = read_full(a->read_fd, &value, sizeof value);
        sem_post(&pipe_read_sem);

        if(rc!=0){
            fprintf(stderr, "consumer %d: %s after %d numbers\n", a->id,
            rc==1 ? "unexpected EOF" : "read error", i);
            a->all_read=0;
            break;
        }
        sum+=value;
    }
    a->sum=sum;

    pthread_mutex_lock(&print_lock);
    printf("[Consumer %2d] sum = %ld (thread ID %lu)\n",
    a->id+1, sum, (unsigned long)pthread_self());
    fflush(stdout);
    pthread_mutex_unlock(&print_lock);
    return NULL;
}

static int child_main(int read_fd){
    sem_init(&pipe_read_sem, 0, 1);

    pthread_t tids[NUMS_CONSUMERS];
    consumer_arg_t args[NUMS_CONSUMERS];
    for(int i=0;i<NUMS_CONSUMERS;i++){
        args[i].id=i;
        args[i].read_fd = read_fd;
        args[i].sum=0;
        args[i].all_read=0;
        if(pthread_create(&tids[i], NULL, consumer, &args[i])!=0){
            perror("pthread_create (consumer)");
            return 1;
        }
    }

    int failed=0;
    long total=0;
    for(int i=0;i<NUMS_CONSUMERS; i++){
        pthread_join(tids[i], NULL);
        total+=args[i].sum;
        if(!args[i].all_read) failed=1;
    }

    char extra;
    ssize_t leftover = read(read_fd, &extra, 1);
    if(leftover!=0){
        fprintf(stderr, "child: pipe still had unread data (or error)\n");
        failed=1;
    }
    close(read_fd);

    //the average calculation
    double average=(double)total/NUMS_CONSUMERS;

    fflush(stdout);
    int fd=open(OUTPUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd<0){
        perror("open " OUTPUT_FILE);
        return 1;
    }
    if(dup2(fd, STDOUT_FILENO)<0){
        perror("dup2");
        close(fd);
        return 1;
    }
    close(fd);

    printf("Consumer sums:\n");
    for(int i=0;i<NUMS_CONSUMERS;i++){
        printf(" consumer %2d: %ld\n", i+1, args[i].sum);
    }
    printf("Total of all sums: %ld\n", total);
    printf("Average of the %d sums: %.2f\n", NUMS_CONSUMERS, average);
    fflush(stdout);

    return failed;
}

//parent main
int main(void){
    int fds[2];
    if(pipe(fds)==-1){
        perror("pipe");
        return 1;
    }

    fflush(NULL);
    pid_t pid = fork();
    if(pid<0){
        perror("fork");
        return 1;
    }

    //child
    if(pid==0){
        close(fds[1]);
        int rc = child_main(fds[0]);
        _exit(rc);
    }

    //parent
    close(fds[0]);
    sem_init(&producers_done, 0, 0);
    pthread_t tids[NUMS_PRODUCERS];
    producer_arg_t args[NUMS_PRODUCERS];
    for(int i=0;i<NUMS_PRODUCERS;i++){
        args[i].id=i+1;
        args[i].write_fd=fds[1];
        if(pthread_create(&tids[i], NULL, producer, &args[i]) != 0){
            perror("pthread_create (producer)");
            return 1;
        }
    }

    for(int i=0;i<NUMS_PRODUCERS;i++){
        sem_wait_retry(&producers_done);
    }
    for(int i=0;i<NUMS_PRODUCERS;i++){
        pthread_join(tids[i], NULL);
    }

    close(fds[1]);

    //errors
    printf("[Parent] all %d numbers generated\n",
    NUMS_PRODUCERS * NUMS_PER_PRODUCER);
    fflush(stdout);

    int status=0;
    if(waitpid(pid, &status, 0)==-1){
        perror("waitpid");
        return 1;
    }

    if(WIFEXITED(status)&&WEXITSTATUS(status)==0){
        printf("[Parent] child finished. Average %s\n", OUTPUT_FILE);
        return 0;
    }
    fprintf(stderr, "[Parent] child failed (status %d)\n", status);
    return 1;
}