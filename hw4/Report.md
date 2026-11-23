Madeline Guest - HW4 
- Prompt Screenshots in the zip!

Compile Instructions:
cd hw4
make run

to clean:
make clean

Process:
The function main creates a pipe and forks before threads are created. The Parent closes the read function and creates 10 producer* threads. Child closes the write function and creates 20 consumer* threads. Each producer runs a shuffle and takes the first 500 entries. The function rand_r is used to create a per-thread seed (as per the assignment) so the threads won't seeds. Each number is written to the pipe during pipe_write_lock so that the writes from each thread cannot overwrrite each other. Each semaphore producers_done is counted and posted by each producer when it is finished. The parent will wait on it. Every 50 writes will cause a producer to print a progress line. Once the producers finish, the thread ID is printed, the parent closes the write function so the child will see EOF. Each consumer will read 250 ints, one at a time, and then computes the sum of the 250 numbers. Each one has a binary semaphore (pipe_read_sem) so multiple threads aren't read at the same time. The functions read_full and write_full handle the partial read/writes and the interrupted calls (EINTR). The compile time _Static_assert ensures each 5000 int produced is consumed. The child process will verify that read() returns EOF to ensure nothing is left in the pipe. The consumers will start immediately and block in read() until data is available, just to make sure reading will overlap with writing and avoiding long run-times and such.

Once every consumer thread is joined, the child add the 20 sums and divides by 20. Then open average.txt and uses the dup2() func to redirect stdout in the file, so the sums and averages are written with standard I/O redirection (printf).

Test:
make run

Results:
[Producer  1] progress:  10% (50/500 writes)
[Producer  1] progress:  20% (100/500 writes)
[Producer  1] progress:  30% (150/500 writes)
[Producer  1] progress:  40% (200/500 writes)
[Producer  1] progress:  50% (250/500 writes)
[Producer  1] progress:  60% (300/500 writes)
[Producer  1] progress:  70% (350/500 writes)
[Producer  1] progress:  80% (400/500 writes)
[Producer  1] progress:  90% (450/500 writes)
[Producer  1] progress: 100% (500/500 writes)
[Producer  1] finished (thread ID 136805949818560)
[Producer  6] progress:  10% (50/500 writes)
[Producer  6] progress:  20% (100/500 writes)
[Producer  6] progress:  30% (150/500 writes)
[Producer  6] progress:  40% (200/500 writes)
[Producer  6] progress:  50% (250/500 writes)
[Producer  3] progress:  10% (50/500 writes)
[Producer  3] progress:  20% (100/500 writes)
[Producer  3] progress:  30% (150/500 writes)
[Producer  3] progress:  40% (200/500 writes)
[Producer  3] progress:  50% (250/500 writes)
[Producer  3] progress:  60% (300/500 writes)
[Producer  3] progress:  70% (350/500 writes)
[Producer  3] progress:  80% (400/500 writes)
[Consumer 11] sum = 122798 (thread ID 136805865891520)
[Producer 10] progress:  10% (50/500 writes)
[Producer 10] progress:  20% (100/500 writes)
[Producer 10] progress:  30% (150/500 writes)
[Producer 10] progress:  40% (200/500 writes)
[Producer 10] progress:  50% (250/500 writes)
[Producer 10] progress:  60% (300/500 writes)
[Producer 10] progress:  70% (350/500 writes)
[Producer 10] progress:  80% (400/500 writes)
[Producer 10] progress:  90% (450/500 writes)
[Producer 10] progress: 100% (500/500 writes)
[Producer 10] finished (thread ID 136805874284224)
[Producer  3] progress:  90% (450/500 writes)
[Producer  3] progress: 100% (500/500 writes)
[Producer  3] finished (thread ID 136805933033152)
[Producer  4] progress:  10% (50/500 writes)
[Producer  4] progress:  20% (100/500 writes)
[Producer  2] progress:  10% (50/500 writes)
[Producer  7] progress:  10% (50/500 writes)
[Producer  8] progress:  10% (50/500 writes)
[Producer  4] progress:  30% (150/500 writes)
[Producer  4] progress:  40% (200/500 writes)
[Producer  4] progress:  50% (250/500 writes)
[Producer  4] progress:  60% (300/500 writes)
[Producer  4] progress:  70% (350/500 writes)
[Producer  4] progress:  80% (400/500 writes)
[Producer  4] progress:  90% (450/500 writes)
[Producer  4] progress: 100% (500/500 writes)
[Producer  4] finished (thread ID 136805924640448)
[Producer  2] progress:  20% (100/500 writes)
[Producer  2] progress:  30% (150/500 writes)
[Producer  2] progress:  40% (200/500 writes)
[Producer  2] progress:  50% (250/500 writes)
[Producer  2] progress:  60% (300/500 writes)
[Producer  2] progress:  70% (350/500 writes)
[Producer  2] progress:  80% (400/500 writes)
[Producer  2] progress:  90% (450/500 writes)
[Producer  2] progress: 100% (500/500 writes)
[Producer  2] finished (thread ID 136805941425856)
[Producer  7] progress:  20% (100/500 writes)
[Producer  7] progress:  30% (150/500 writes)
[Producer  7] progress:  40% (200/500 writes)
[Producer  7] progress:  50% (250/500 writes)
[Producer  7] progress:  60% (300/500 writes)
[Producer  7] progress:  70% (350/500 writes)
[Producer  7] progress:  80% (400/500 writes)
[Producer  7] progress:  90% (450/500 writes)
[Consumer  1] sum = 124220 (thread ID 136805949818560)
[Consumer  6] sum = 116838 (thread ID 136805907855040)
[Producer  7] progress: 100% (500/500 writes)
[Producer  9] progress:  10% (50/500 writes)
[Producer  9] progress:  20% (100/500 writes)
[Producer  9] progress:  30% (150/500 writes)
[Producer  9] progress:  40% (200/500 writes)
[Producer  9] progress:  50% (250/500 writes)
[Producer  9] progress:  60% (300/500 writes)
[Producer  9] progress:  70% (350/500 writes)
[Producer  9] progress:  80% (400/500 writes)
[Producer  9] progress:  90% (450/500 writes)
[Producer  9] progress: 100% (500/500 writes)
[Producer  9] finished (thread ID 136805882676928)
[Producer  5] progress:  10% (50/500 writes)
[Producer  5] progress:  20% (100/500 writes)
[Producer  5] progress:  30% (150/500 writes)
[Consumer  8] sum = 129411 (thread ID 136805891069632)
[Producer  5] progress:  40% (200/500 writes)
[Producer  6] progress:  60% (300/500 writes)
[Producer  8] progress:  20% (100/500 writes)
[Producer  6] progress:  70% (350/500 writes)
[Producer  6] progress:  80% (400/500 writes)
[Producer  6] progress:  90% (450/500 writes)
[Producer  6] progress: 100% (500/500 writes)
[Producer  6] finished (thread ID 136805907855040)
[Producer  8] progress:  30% (150/500 writes)
[Consumer 13] sum = 125230 (thread ID 136805849106112)
[Producer  8] progress:  40% (200/500 writes)
[Producer  8] progress:  50% (250/500 writes)
[Producer  8] progress:  60% (300/500 writes)
[Producer  8] progress:  70% (350/500 writes)
[Producer  8] progress:  80% (400/500 writes)
[Producer  8] progress:  90% (450/500 writes)
[Producer  8] progress: 100% (500/500 writes)
[Producer  8] finished (thread ID 136805891069632)
[Producer  7] finished (thread ID 136805899462336)
[Consumer 17] sum = 124700 (thread ID 136805815535296)
[Producer  5] progress:  50% (250/500 writes)
[Consumer 18] sum = 124912 (thread ID 136805807142592)
[Producer  5] progress:  60% (300/500 writes)
[Producer  5] progress:  70% (350/500 writes)
[Producer  5] progress:  80% (400/500 writes)
[Consumer 19] sum = 131655 (thread ID 136805798749888)
[Producer  5] progress:  90% (450/500 writes)
[Consumer  9] sum = 130974 (thread ID 136805882676928)
[Producer  5] progress: 100% (500/500 writes)
[Producer  5] finished (thread ID 136805916247744)
[Consumer  2] sum = 114542 (thread ID 136805941425856)
[Consumer 16] sum = 124688 (thread ID 136805823928000)
[Consumer 12] sum = 129111 (thread ID 136805857498816)
[Parent] all 5000 numbers generated
[Consumer  4] sum = 124215 (thread ID 136805924640448)
[Consumer  3] sum = 118142 (thread ID 136805933033152)
[Consumer  7] sum = 122986 (thread ID 136805899462336)
[Consumer  5] sum = 125486 (thread ID 136805916247744)
[Consumer 20] sum = 124791 (thread ID 136805790357184)
[Consumer 10] sum = 116554 (thread ID 136805874284224)
[Consumer 15] sum = 121230 (thread ID 136805832320704)
[Consumer 14] sum = 127651 (thread ID 136805840713408)
[Parent] child finished. Average average.txt
---- average.txt ----
Consumer sums:
 consumer  1: 124220
 consumer  2: 114542
 consumer  3: 118142
 consumer  4: 124215
 consumer  5: 125486
 consumer  6: 116838
 consumer  7: 122986
 consumer  8: 129411
 consumer  9: 130974
 consumer 10: 116554
 consumer 11: 122798
 consumer 12: 129111
 consumer 13: 125230
 consumer 14: 127651
 consumer 15: 121230
 consumer 16: 124688
 consumer 17: 124700
 consumer 18: 124912
 consumer 19: 131655
 consumer 20: 124791
Total of all sums: 2480134
Average of the 20 sums: 124006.70