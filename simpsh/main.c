#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "fileoptions.h"

int argument_amount(int argc, char* argv[], int long_index);
int print_option(int argc, char* argv[], int long_index, FILE *print_to);

int verbose_flag = 0;
int verbose_flag2 = 0;
int open_flags = 0;
int no_of_files = 0;
int max_files = 5;
int* filesystem;

int main(int argc, char *argv[])
{
    filesystem = malloc(max_files * sizeof(int));
    struct option options[] =
    {
        {"rdonly", required_argument, NULL, 'r'},
        {"wronly", required_argument, NULL, 'w'},
        {"command", required_argument, NULL, 'c'},
        {"verbose", no_argument, NULL, 'v'}
    };
    
    int option_index = 0;
    int opt;
    int index;
    char* arg_error = "Incorrect number of arguments.";

    while ((opt = getopt_long(argc, argv, "", options, &option_index)) != -1) {
        printf("\n\n%d\n\n", optind);
        index = optind - 1;
        switch (opt) {
            case 'r':
            case 'w':
            case 'c':
                index--;
                break;
            default:
                break;
        }
        if (verbose_flag) {
            if (verbose_flag2) {
                printf(" ");
            }
            else {
                verbose_flag2 = 1;
            }
            print_option(argc, argv, index, stdout);
        }
        int args = argument_amount(argc, argv, index);
        printf("args: %d", args);
        switch (opt) {
            case 'r':
                if (args > 2) {
                    print_option(argc, argv, index, stderr);
                    fprintf(stderr, ": %s\n", arg_error);
                    break;
                }
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                }
                open_flags |= O_RDONLY;
                if (!open_file(optarg, open_flags)) {
                    print_option(argc, argv, index, stderr);
                    fprintf(stderr, ": ");
                    perror(NULL);
                }
                open_flags = 0;
                break;
            case 'w':
                if (args > 2) {
                    print_option(argc, argv, index, stderr);
                    fprintf(stderr, ": %s\n", arg_error);
                    break;
                }
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                }
                open_flags |= O_WRONLY;
                if(!open_file(optarg, open_flags)) {
                    print_option(argc, argv, index, stderr);
                    fprintf(stderr, ": ");
                    perror(NULL);
                }
                open_flags = 0;
                break;
            case 'v':
                if (args != 1) {
                    print_option(argc, argv, index, stderr);
                    fprintf(stderr, ": %s\n", arg_error);
                }
                verbose_flag = 1;
                break;
            case 'c':
                if (args < 5) {
                    print_option(argc, argv, index, stderr);
                    fprintf(stderr, ": %s\n", arg_error);
                    printf("\n");
                    verbose_flag2 = 0;
                    break;
                }
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                }
                //get the stdin file descriptor
                index++;
                int stdin_logical_fid = strtol(argv[index], NULL, 10);
                int stdin_real_fid = filesystem[stdin_logical_fid];
                //get the stdout file descriptor
                index++;
                int stdout_logical_fid = strtol(argv[index], NULL, 10);
                int stdout_real_fid = filesystem[stdout_logical_fid];
                //get the stderr file descriptor
                index++;
                int stderr_logical_fid = strtol(argv[index], NULL, 10);
                int stderr_real_fid = filesystem[stderr_logical_fid];
                //get the command string
                index++;
                char* command = argv[index];
                //find the number of args
                int num_args = args - 5;
                //get the args
                char** args_list = malloc((num_args + 1) * sizeof(char*));
                args_list[0] = command;
                if (num_args != 0) {
                    int i = 1;
                    while(i <= num_args) {
                        args_list[i] = argv[index + i];
                        i++;
                    }
                }
                //call the command via execvp
                free(args_list);
                open_flags = 0;  //??????????????????????????
                break;
            default:
                break;

        }
    }
    free(filesystem); 
}

int argument_amount(int argc, char* argv[], int long_index)
{
    int count = 1;
    if ((long_index + count) == argc) {
        return count;
    }
    while((argv[long_index + count][0] != '-') || (argv[long_index + count][1] != '-')) {
        if ((long_index + count) == argc) {
            break;
        }
        count++;
    }
    return count;
}

int print_option(int argc, char* argv[], int long_index, FILE *print_to)
{
    fprintf(print_to, "%s", argv[long_index]);
    int count = 1;
    if ((long_index + count) == argc) {
        return count;
    }
    while((argv[long_index + count][0] != '-') || (argv[long_index + count][1] != '-')) {
        fprintf(print_to, " %s", argv[long_index + count]);
        count++;
        if ((long_index + count) == argc) {
            break;
        }
    }
    return count;
}