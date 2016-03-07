#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#define THREADS 1
#define ITERATIONS 2

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    *pointer = sum;
}

int threads = 1;
int iterations = 1;

int main(int argc, char *argv[])
{
    static struct option options[] = {
        {"threads", required_argument, 0, THREADS},
        {"iterations", required_argument, 0, ITERATIONS},
        {0, 0, 0, 0}
    };
    
    
}
