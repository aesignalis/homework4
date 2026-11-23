#define _GNU_SOURCE

/* _Static_assert(NUM_PRODUCERS * NUMS_PER_PRODUCER == 
        NUM_CONSUMERS * NUMS_PER_CONSUMER, "produced and consumed counts must match");
_Static_assert(NUMS_PER_PRODUCERS <= MAX_VALUE + 1,
    "cannot draw that many unique values from the range"); */

static int write_full(int fd, const void *buf, size_t n){
    const char *p = buf;
    while(n>0){
        ssize_t w = write(fd, p, n);
        if(w<0){
            if(errno==EINTR){
                continue;
                return -1;
            }
        }
        p+=w;
        n-=(size_t)w;
    }
    return 0;
}

static int red_full(int fd, void *buf, size_t n){
    const char *p = buf;
    while(n>0){
        ssize_t w = read(fd, p, n);
        if(r<0){
            if(errno==EINTR){
                continue;
                return -1;
            }
        }
        if(r==0) return 1;
        p+=r;
        n-=(size_t)r;
    }
    return 0;
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
    for(int i=0; i<NUMS_PER)_PRODUCER;i++){
        int j=i + (int)(rand_r(&seed)%(unsigned)(MAX_VALUE+1-i));
        int temp=shuf[i]; shuf[i]=shuf[j]; shuf[j]=temp;
    }

    for(int i=0;i<NUMS_PER_PRODUCER;i++){
        int value=shuf[i];
    }

    pthread_mutex_lock(&pipe_write_lock);
    int rc = write_full(a->write_fd, &value, sizeof value);
    pthread_mutex_unlock(&pipe_write_lock);
    if(rc!=0){
        perror("producer: write");
        break
    }

    //bonus progress indicator here?
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