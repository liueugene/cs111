#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#define THREADS 1
#define ITERATIONS 2

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    *pointer = sum;
}

int no_of_threads = 1;
int iterations = 1;

long long counter = 0;

int main(int argc, char *argv[])
{
    static struct option options[] = {
        {"threads", required_argument, 0, THREADS},
        {"iterations", required_argument, 0, ITERATIONS},
        {0, 0, 0, 0}
    };
    
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
    
    printf("%d threads, %d iterations\n", no_of_threads, iterations);
    exit(0);
}
