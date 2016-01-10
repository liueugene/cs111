#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "fileoptions.h"

int cycle_option(int argc, char* argv[], int long_index, int print, FILE *print_to);

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
        }
        int args = cycle_option(argc, argv, index, verbose_flag, stdout);
        switch (opt) {
            case 'r':
                if (args > 2) {
                    cycle_option(argc, argv, index, 1, stderr);
                    fprintf(stderr, ": %s\n", arg_error);
                    break;
                }
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                }
                open_flags |= O_RDONLY;
                if (!open_file(optarg, open_flags)) {
                    cycle_option(argc, argv, index, 1, stderr);
                    fprintf(stderr, ": ");
                    perror(NULL);
                }
                open_flags = 0;
                break;
            case 'w':
                if (args > 2) {
                    cycle_option(argc, argv, index, 1, stderr);
                    fprintf(stderr, ": %s\n", arg_error);
                    break;
                }
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                }
                open_flags |= O_WRONLY;
                if(!open_file(optarg, open_flags)) {
                    cycle_option(argc, argv, index, 1, stderr);
                    fprintf(stderr, ": ");
                    perror(NULL);
                }
                open_flags = 0;
                break;
            case 'v':
                if (args != 1) {
                    cycle_option(argc, argv, index, 1, stderr);
                    fprintf(stderr, ": %s\n", arg_error);
                }
                verbose_flag = 1;
                break;
            case 'c':
                if (args < 5) {
                    cycle_option(argc, argv, index, 1, stderr);
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
                int stdin_logical_fd = strtol(argv[index], NULL, 10);
                if (stdin_logical_fd >= no_of_files) {
                    cycle_option(argc, argv, index - 1, 1, stderr);
                    fprintf(stderr, ": %s\n", "Invalid file descriptor number for stdin.");
                    break;
                }
                int stdin_real_fd = filesystem[stdin_logical_fd];
                //get the stdout file descriptor
                index++;
                int stdout_logical_fd = strtol(argv[index], NULL, 10);
                if (stdout_logical_fd >= no_of_files) {
                    cycle_option(argc, argv, index - 2, 1, stderr);
                    fprintf(stderr, ": %s\n", "Invalid file descriptor number for stdout.");
                    break;
                }
                int stdout_real_fd = filesystem[stdout_logical_fd];
                //get the stderr file descriptor
                index++;
                int stderr_logical_fd = strtol(argv[index], NULL, 10);
                if (stderr_logical_fd >= no_of_files) {
                    cycle_option(argc, argv, index - 3, 1, stderr);
                    fprintf(stderr, ": %s\n", "Invalid file descriptor number for stderr.");
                    break;
                }
                int stderr_real_fd = filesystem[stderr_logical_fd];
                //get the command string
                index++;
                char* command = argv[index];
                //find the number of args
                int num_args = args - 5;
                //get the args
                char** args_list = malloc((num_args + 2) * sizeof(char*));
                args_list[0] = command;
                int i = 1;
                if (num_args != 0) {
                    while(i <= num_args) {
                        args_list[i] = argv[index + i];
                        printf("%s\n", args_list[i]);
                        i++;
                    }
                    args_list[i] = NULL;
                }
                optind = index + i;
                call_command(num_args + 1, args_list, stdin_real_fd, stdout_real_fd, stderr_real_fd);
                free(args_list);
                open_flags = 0;  //??????????????????????????
                break;
            default:
                break;

        }
    }
    free(filesystem); 
}

int cycle_option(int argc, char* argv[], int long_index, int print, FILE *print_to)
{
    if (print) {
        fprintf(print_to, "%s", argv[long_index]);
    }
    int count = 1;
    if ((long_index + count) == argc) {
        return count;
    }
    while((argv[long_index + count][0] != '-') || (argv[long_index + count][1] != '-')) {
        if (print) {
            fprintf(print_to, " %s", argv[long_index + count]);
        }
        count++;
        if ((long_index + count) == argc) {
            break;
        }
    }
    return count;
}