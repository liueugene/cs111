#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>

#define THREADS 1
#define ITERATIONS 2

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    *pointer = sum;
}

int no_of_threads = 1;
int iterations = 1;

long long counter = 0;

void add_func(int* thread_no)
{
    clock_t begin, end;
    double time_spent;

    int i;


    begin = clock();

    int num_of_adds = iterations / no_of_threads;
    if (*thread_no < (iterations % no_of_threads)) {
        num_of_adds++;
    }

    for (i = 0; i < num_of_adds; i++) {
        add(&counter, 1);
    }

    for (i = 0; i < num_of_adds; i++) {
        add(&counter, -1);
    }

    end = clock();

    time_spent = (double) ((end - begin) / CLOCKS_PER_SEC);

    pthread_exit(time_spect);
}

int main(int argc, char *argv[])
{
    static struct option options[] = {
        {"threads", required_argument, 0, THREADS},
        {"iterations", required_argument, 0, ITERATIONS},
        {0, 0, 0, 0}
    };
    
    pthread_t* threads[no_of_threads];
    double runtime;
    double tot_runtime;
    int nsecs;
    int no_of_ops;

    int opt;
    
    while ((opt = getopt_long(argc, argv, "", options, NULL)) != -1) {
    
        char *endptr;
        switch (opt) {
            case THREADS:
                no_of_threads = strtol(argv[optind], &endptr, 10);
                if (endptr == argv[optind]) {
                    fprintf(stderr, "Invalid number of threads\n");
                    exit(1);
                }
                break;
            case ITERATIONS:
                iterations = strtol(argv[optind], &endptr, 10);
                if (endptr == argv[optind]) {
                    fprintf(stderr, "Invalid number of threads\n");
                    exit(1);
                }
                break;
        }
    }

    //create and run threads
    for (i = 0; i < no_of_threads; i++) {
        if (pthread_create(&threads[i], NULL, &add_func, &i)) {
            fprintf(stderr, "Error creating thread %d.\n", i);
            return 1;
        }
    }

    tot_runtime = 0;
    //wait for threads
    for (i = 0; i < no_of_threads; i++) {
        if (pthread_join(threads[i], &runtime)) {
            fprintf(stderr, "Error waiting for thread %d.\n", i);
            return 1;
        }
        tot_runtime += runtime;
    }

    nsecs = tot_runtime * 1000000000;
    no_of_ops = threads * iterations * 2;
    
    printf("%d threads * %d iterations x (add + subtract) = %d operations\n", no_of_threads, iterations, num_of_ops);
    if (counter == 0) {
        printf("final count = 0\n");
    }
    else {
        printf("ERROR: final count = %d\n", counter);
    }
    printf("elapsed time: %d ns\n", nsecs);
    printf("per operation: %d ns\n", nsecs / no_of_ops);
    exit(0);
}
