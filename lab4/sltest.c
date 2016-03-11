#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <time.h>
#include "SortedList.h"

#define THREADS 1
#define ITERATIONS 2
#define OPT_YIELD 3
#define SYNC 4
#define LISTS 5

int no_of_threads = 1;
int iterations = 1;
int opt_yield = 0;
char opt_sync = 0;
pthread_mutex_t* pmutex;
int* lock;

SortedList_t** List;
char ***rand_strings;
int list_no = 1;

long long counter = 0;

int list_num(const char* key) {
    int val = 0;
    for (int i = 0; i < 7; i++) {
        val += (int) key[i];
    }
    return (val % list_no);
}

void* list_func(int* thread_no) {
    int i;
    int index;
    SortedListElement_t* element = malloc(iterations * sizeof(SortedListElement_t));
    for (i = 0; i < iterations; i++) {
        element[i].key = rand_strings[*thread_no][i];
        
        index = list_num(element[i].key);

        if (opt_sync == 'm')
            pthread_mutex_lock(&pmutex[index]);
        else if (opt_sync == 's')
            while (__sync_lock_test_and_set(&lock[index], 1));
            
        SortedList_insert(List[index], &element[i]);
        
        if (opt_sync == 'm')
            pthread_mutex_unlock(&pmutex[index]);
        else if (opt_sync == 's')
            __sync_lock_release(&lock[index], 0);
    }
    for (i = 0; i < list_no; i++) {
        if (opt_sync == 'm')
            pthread_mutex_lock(&pmutex[index]);
        else if (opt_sync == 's')
            while (__sync_lock_test_and_set(&lock[index], 1));
            
        SortedList_length(List[i]);
        
        if (opt_sync == 'm')
            pthread_mutex_unlock(&pmutex[index]);
        else if (opt_sync == 's')
            __sync_lock_release(&lock[index], 0);
    }
    
    for (i = 0; i < iterations; i++) {
        int index = list_num(rand_strings[*thread_no][i]);

        if (opt_sync == 'm')
            pthread_mutex_lock(&pmutex[index]);
        else if (opt_sync == 's')
            while (__sync_lock_test_and_set(&lock[index], 1));
            
        if (SortedList_delete(SortedList_lookup(List[index], rand_strings[*thread_no][i])))
            fprintf(stderr, "Corrupted linked list\n");
        
        if (opt_sync == 'm')
            pthread_mutex_unlock(&pmutex[index]);
        else if (opt_sync == 's')
            __sync_lock_release(&lock[index], 0);
    }

    free(element);

    pthread_exit(NULL);
    return 0;
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

int main(int argc, char *argv[])
{
    static struct option options[] = {
        {"threads", required_argument, 0, THREADS},
        {"iterations", required_argument, 0, ITERATIONS},
        {"yield", required_argument, 0, OPT_YIELD},
        {"sync", required_argument, 0, SYNC},
        {"lists", required_argument, 0, LISTS},
        {0, 0, 0, 0}
    };
    
    long long nsecs;
    long long no_of_ops;
    int opt;
    int i;
    
    while ((opt = getopt_long(argc, argv, "", options, NULL)) != -1) {
    
        char *endptr;
        char numarray[32];
        
        switch (opt) {
            case THREADS:
                no_of_threads = strtol(numstring(argv, optind - 1, 10, numarray), &endptr, 10);
                if (endptr == argv[optind - 1] || no_of_threads < 1) {
                    fprintf(stderr, "Invalid number of threads\n");
                    exit(1);
                }
                break;
            case ITERATIONS:
                iterations = strtol(numstring(argv, optind - 1, 13, numarray), &endptr, 10);
                if (endptr == argv[optind - 1] || iterations < 1) {
                    fprintf(stderr, "Invalid number of iterations\n");
                    exit(1);
                }
                break;
            case OPT_YIELD: {
                char *str = &argv[optind - 1][8];
                
                for (int i = 0; str[i] != '\0'; i++) {
                    switch (str[i]) {
                        case 'i':
                            opt_yield |= INSERT_YIELD;
                            break;
                        case 'd':
                            opt_yield |= DELETE_YIELD;
                            break;
                        case 's':
                            opt_yield |= SEARCH_YIELD;
                            break;
                        default:
                            fprintf(stderr, "Invalid yield option\n");
                            exit(1);
                    }
                }
                break;
            }
            case SYNC:
                opt_sync = argv[optind - 1][7];
                if (opt_sync != 'm' && opt_sync != 's') {
                    fprintf(stderr, "Invalid sync option\n");
                    exit(1);
                }
                break;
            case LISTS:
                list_no = strtol(numstring(argv, optind - 1, 8, numarray), &endptr, 10);
                if (endptr == argv[optind - 1] || list_no < 1) {
                    fprintf(stderr, "Invalid number of sublists\n");
                    exit(1);
                }
                break;
            default:
            {
            }
        }
    }

    List = malloc(list_no * sizeof(SortedList_t*));
    for (int i = 0; i < list_no; i++) {
        List[i] = malloc(sizeof(SortedList_t));
        List[i]->prev = List[i];
        List[i]->next = List[i];
        List[i]->key = NULL;
    }
    if (opt_sync == 'm') {
        pmutex = malloc(list_no * sizeof(pthread_mutex_t));
        for (int i = 0; i < list_no; i++) {
            pthread_mutex_init(&pmutex[i], NULL);
        }
    }
    else if (opt_sync == 's') {
        lock = malloc(list_no * sizeof(int));
        for (int i = 0; i < list_no; i++) {
            lock[i] = 0;
        }
    }

    pthread_t threads[no_of_threads];
    struct timespec begin, end;
    
    int* thread_no = malloc(no_of_threads * sizeof(int));

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
    
    clock_gettime(CLOCK_MONOTONIC, &begin);
    
    //create and run threads
    for (i = 0; i < no_of_threads; i++) {
        thread_no[i] = i;
        if (pthread_create(&threads[i], NULL, (void* (*)(void *))list_func, &thread_no[i])) {
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

    free(thread_no);

    nsecs = ((end.tv_sec - begin.tv_sec) * 1000000000) + (end.tv_nsec - begin.tv_nsec);
    no_of_ops = ((long long)no_of_threads * iterations * iterations)/list_no;
    
    printf("%d threads * %d iterations x (ins + lookup/del) x (%d/2 avg len) = %lld operations\n", no_of_threads, iterations, iterations/list_no, no_of_ops);
    for (int i = 0; i < list_no; i++) {
        counter += SortedList_length(List[i]);
    }
    if (counter != 0) {
        fprintf(stderr, "ERROR: final count = %lld\n", counter);
    }
    printf("elapsed time: %lld ns\n", nsecs);
    printf("per operation: %lld ns\n", nsecs / no_of_ops);    
    
    for (int i = 0; i < no_of_threads; i++) {
        for (int j = 0; j < iterations; j++) {
            free(rand_strings[i][j]);
        }
        free(rand_strings[i]);
    }
    free(rand_strings);
    for (int i = 0; i < list_no; i++)
        free(List[i]);
    free(List);
    free(pmutex);
    free(lock);
    
    exit(0);
}
