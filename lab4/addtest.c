#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>

#define THREADS 1
#define ITERATIONS 2
#define OPT_YIELD 3
#define SYNC 4

int no_of_threads = 1;
int iterations = 1;
int opt_yield = 0;
char opt_sync = 0;
pthread_mutex_t pmutex;
int lock = 0;


long long counter = 0;

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
        pthread_yield();
    *pointer = sum;
}

void add_pthread_mutex(long long *pointer, long long value) {
    pthread_mutex_lock(&pmutex);
    long long sum = *pointer + value;
    if (opt_yield)
        pthread_yield();
    *pointer = sum;
    pthread_mutex_unlock(&pmutex);
}

void add_spinlock(long long *pointer, long long value) {
    while(__sync_lock_test_and_set(&lock, 1)) {}
    long long sum = *pointer + value;
    if (opt_yield)
        pthread_yield();
    *pointer = sum;
    __sync_lock_release(&lock, 0);
}

void atomic_add(long long *pointer, long long value) {
    /*
    while(__sync_val_compare_and_swap(&lock, 0, 1));
    long long sum = *pointer + value;
    if (opt_yield)
        pthread_yield();
    *pointer = sum;
    __sync_val_compare_and_swap(&lock, 1, 0);
    */
    long long sum;
    long long orig;
    
    do {
        orig = *pointer;
        sum = orig + value;
        
        if (opt_yield)
            pthread_yield();
    } while (__sync_val_compare_and_swap(pointer, orig, sum) != orig);
}

char* numstring(char** argv, int index, int index2, char *numarray) {
    int i = 0;
    while (argv[index][index2] != '\0' && i != 32) {
        numarray[i] = argv[index][index2];
        i++;
        index2++;
    }
    numarray[i] = '\0';
    return numarray;
}

void *add_func()
{
    int i;
    void (*a_func)(long long *pointer, long long value) = add;

    if (opt_sync == 'm') {
        a_func = add_pthread_mutex;
    } else if (opt_sync == 's') {
        a_func = add_spinlock;
    } else if (opt_sync == 'c') {
        a_func = atomic_add;
    }
    
    for (i = 0; i < iterations; i++) {
        a_func(&counter, 1);
    }

    for (i = 0; i < iterations; i++) {
        a_func(&counter, -1);
    }
    
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[])
{
    static struct option options[] = {
        {"threads", required_argument, 0, THREADS},
        {"iterations", required_argument, 0, ITERATIONS},
        {"yield", required_argument, 0, OPT_YIELD},
        {"sync", required_argument, 0, SYNC},
        {0, 0, 0, 0}
    };
    
    long long nsecs;
    int no_of_ops;
    int opt;
    int i;
    
    while ((opt = getopt_long(argc, argv, "", options, NULL)) != -1) {
    
        char *endptr;
        char numarray[32];
        
        switch (opt) {
            case THREADS:
                no_of_threads = strtol(numstring(argv, optind - 1, 10, numarray), &endptr, 10);
                if (endptr == argv[optind] || no_of_threads < 1) {
                    fprintf(stderr, "Invalid number of threads\n");
                    exit(1);
                }
                break;
            case ITERATIONS:
                iterations = strtol(numstring(argv, optind - 1, 13, numarray), &endptr, 10);
                if (endptr == argv[optind] || iterations < 1) {
                    fprintf(stderr, "Invalid number of iterations\n");
                    exit(1);
                }
                break;
            case OPT_YIELD:
                opt_yield = strtol(numstring(argv, optind - 1, 8, numarray), &endptr, 10);
                if (endptr == argv[optind] || (opt_yield != 0 && opt_yield != 1)) {
                    fprintf(stderr, "Invalid number for opt_yield\n");
                    exit(1);
                }
                break;
            case SYNC:
                opt_sync = argv[optind - 1][7];
                if (opt_sync != 'm' && opt_sync != 's' && opt_sync != 'c') {
                    fprintf(stderr, "Invalid sync option\n");
                    exit(1);
                }
                if (opt_sync == 'm') {
                    pthread_mutex_init(&pmutex, NULL);
                }
                break;
            default:
            {
            }
        }
    }

    pthread_t threads[no_of_threads];
    struct timespec begin, end;
    
    clock_gettime(CLOCK_MONOTONIC, &begin);
    
    //create and run threads
    for (i = 0; i < no_of_threads; i++) {
        if (pthread_create(&threads[i], NULL, (void* (*)(void *))add_func, NULL)) {
            fprintf(stderr, "Error creating thread %d.\n", i);
            return 1;
        }
    }

    //wait for threads
    for (i = 0; i < no_of_threads; i++) {
        if (pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Error waiting for thread %d.\n", i);
            return 1;
        }
    }
    
    clock_gettime(CLOCK_MONOTONIC, &end);

    nsecs = ((end.tv_sec - begin.tv_sec) * 1000000000) + (end.tv_nsec - begin.tv_nsec);
    no_of_ops = no_of_threads * iterations * 2;
    
    printf("%d threads * %d iterations x (add + subtract) = %d operations\n", no_of_threads, iterations, no_of_ops);
    if (counter != 0) {
        fprintf(stderr, "ERROR: final count = %lld\n", counter);
    }
    printf("elapsed time: %lld ns\n", nsecs);
    printf("per operation: %lld ns\n", nsecs / no_of_ops);
    exit(0);
}
