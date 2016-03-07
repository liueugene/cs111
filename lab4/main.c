#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>

#define THREADS 1
#define ITERATIONS 2
#define OPT_YIELD 3

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
        pthread_yield();
    *pointer = sum;
}

int no_of_threads = 1;
int iterations = 1;
int opt_yield = 0;

long long counter = 0;

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
    
    for (i = 0; i < iterations; i++) {
        add(&counter, 1);
    }

    for (i = 0; i < iterations; i++) {
        add(&counter, -1);
    }
    
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char *argv[])
{
    static struct option options[] = {
        {"threads", required_argument, 0, THREADS},
        {"iter", required_argument, 0, ITERATIONS},
        {"yield", required_argument, 0, OPT_YIELD},
        {0, 0, 0, 0}
    };
    
    int nsecs;
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
                iterations = strtol(numstring(argv, optind - 1, 7, numarray), &endptr, 10);
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
            default:
            {
            }
        }
    }

    pthread_t threads[no_of_threads];
    struct timespec begin, end;
    
    clock_gettime(CLOCK_REALTIME, &begin);
    
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
    
    clock_gettime(CLOCK_REALTIME, &end);

    nsecs = ((end.tv_sec - begin.tv_sec) * 1000000000) + (end.tv_nsec - begin.tv_nsec);
    no_of_ops = no_of_threads * iterations * 2;
    
    printf("%d threads * %d iterations x (add + subtract) = %d operations\n", no_of_threads, iterations, no_of_ops);
    if (counter != 0) {
        fprintf(stderr, "ERROR: final count = %lld\n", counter);
    }
    printf("elapsed time: %d ns\n", nsecs);
    printf("per operation: %d ns\n", nsecs / no_of_ops);
    exit(0);
}
