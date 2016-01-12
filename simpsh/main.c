#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include "fileoptions.h"

int cycle_option(int argc, char* argv[], int long_index, int print, FILE *print_to);
void print_error(int argc, char* argv[], int long_index, char* error);
int max(int a, int b);

int verbose_flag = 0;
int verbose_flag2 = 0;
int open_flags = 0;
int no_of_files = 0;
int max_files = 5;
int exit_status = 0;
int* filesystem;

int main(int argc, char *argv[])
{
    filesystem = malloc(max_files * sizeof(int));
    
    if (filesystem == NULL) {
        perror("malloc");
        exit(1);
    }

    int temp = cycle_option(argc, argv, 0, 0, NULL);
    if (temp != 1) {
        print_error(argc, argv, 0, "Invalid arguments.");
    }
    
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
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                }
                if (args != 2) {
                    print_error(argc, argv, index, arg_error);
                    if (args == 1) {
                        optind--;
                    }
                    break;
                }
                open_flags |= O_RDONLY;
                if (!open_file(optarg, open_flags)) {
                    print_error(argc, argv, index, NULL);
                    exit_status = max(1, exit_status);
                }
                open_flags = 0;
                break;
            case 'w':
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                }
                if (args != 2) {
                    print_error(argc, argv, index, arg_error);
                    if (args == 1) {
                        optind--;
                    }
                    break;
                }
                open_flags |= O_WRONLY;
                if(!open_file(optarg, open_flags)) {
                    print_error(argc, argv, index, NULL);
                    exit_status = max(1, exit_status);
                }
                open_flags = 0;
                break;
            case 'v':
                if (args != 1) {
                    print_error(argc, argv, index, arg_error);
                }
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                    print_error(argc, argv, index, "--verbose has already been called.");
                }
                verbose_flag = 1;
                break;
            case 'c':
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                }
                if (args < 5) {
                    print_error(argc, argv, index, arg_error);
                    if (args == 1) {
                        optind--;
                    }
                    break;
                }
                //get the stdin file descriptor
                index++;
                char *end;
                int stdin_logical_fd = strtol(argv[index], &end, 10);
                if (end == argv[index]) {
                    print_error(argc, argv, index - 1, "Invalid file descriptor input for stdin.");
                    break;
                }
                if ((stdin_logical_fd >= no_of_files) || (stdin_logical_fd < 0)) {
                    print_error(argc, argv, index - 1, "Invalid file descriptor number for stdin.");
                    break;
                }
                int stdin_real_fd = filesystem[stdin_logical_fd];
                //get the stdout file descriptor
                index++;
                int stdout_logical_fd = strtol(argv[index], &end, 10);
                if (end == argv[index]) {
                    print_error(argc, argv, index - 2, "Invalid file descriptor input for stdout.");
                    break;
                }
                if ((stdout_logical_fd >= no_of_files) || (stdout_logical_fd < 0)) {
                    print_error(argc, argv, index - 2, "Invalid file descriptor number for stdout.");
                    break;
                }
                int stdout_real_fd = filesystem[stdout_logical_fd];
                //get the stderr file descriptor
                index++;
                int stderr_logical_fd = strtol(argv[index], &end, 10);
                if (end == argv[index]) {
                    print_error(argc, argv, index - 3, "Invalid file descriptor input for stderr.");
                    break;
                }
                if ((stderr_logical_fd >= no_of_files) || (stderr_logical_fd < 0)) {
                    print_error(argc, argv, index - 3, "Invalid file descriptor number for stderr.");
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
                
                if (args_list == NULL) {
                    perror("malloc");
                    exit(max(exit_status, 1));
                }
                
                args_list[0] = command;
                int i = 1;
                if (num_args != 0) {
                    while(i <= num_args) {
                        args_list[i] = argv[index + i];
                        i++;
                    }
                }
                args_list[i] = NULL;
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
    return exit_status;
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
    fflush(print_to);
    return count;
}

void print_error(int argc, char* argv[], int long_index, char* error)
{
    cycle_option(argc, argv, long_index, 1, stderr);
    if (error == NULL) {
        fprintf(stderr, ": ");
        perror(NULL);
    }
    else {
        fprintf(stderr, ": %s\n", error);
    }
}

int max(int a, int b)
{
    if (a > b) {
        return a;
    }
    return b;
}




