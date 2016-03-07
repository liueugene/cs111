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

char ***rand_strings;


long long counter = 0;

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
                opt_yield = &argv[optind - 1][8];
                if (opt_yield != 'i' && opt_yield != 'd' && opt_yield != 's') {
                    fprintf(stderr, "Invalid sync option\n");
                    exit(1);
                }
                break;
            default:
            {
            }
        }
    }

    rand_strings = malloc(no_of_threads * sizeof(char**));
    for (int i = 0; i < no_of_threads; i++) {
        rand_strings[i] = malloc(iterations * sizeof(char*));
        for (int j = 0; j < iterations; j++) {
            rand_strings[i][j] = malloc(8 * sizeof(char));
            for (int k = 0; k < 7; k++)
                rand_strings[i][j][k] = (rand() % 95) + 32;
            rand_strings[i][j][7] = '\0';
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
    
    
    for (int i = 0; i < no_of_threads; i++) {
        for (int j = 0; j < iterations; j++) {
            free(rand_strings[i][j]);
        }
        free(rand_strings[i]);
    }
    free(rand_strings);
    
    exit(0);

}
