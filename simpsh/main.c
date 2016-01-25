#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "fileoptions.h"

int cycle_option(int argc, char* argv[], int long_index, int print, FILE *print_to);
void print_error(int argc, char* argv[], int long_index, char* error);
int max(int a, int b);
void handler(int n);

int verbose_flag = 0;
int v_newline_flag = 0;
int open_flags = 0;
int no_of_files = 0;
int max_files = 5;
int* filesystem;
int exit_status = 0;
int sig_min = 0;
int sig_max = 63;
int* processes;
int* commands;
int no_of_processes = 0;
int max_processes = 5;


#define _command 100
#define _verbose 101
#define _abort 102
#define _default 103
#define _ignore 104
#define _catch 105
#define _pause 106
#define _close 107
#define _wait 108

#ifndef O_RSYNC
#define O_RSYNC O_SYNC
#endif

int main(int argc, char *argv[])
{
    filesystem = malloc(max_files * sizeof(int));
    processes = malloc(max_processes * sizeof(int));
    commands = malloc(max_processes * sizeof(int));

    if (filesystem == NULL || processes == NULL || commands == NULL) { 
        perror("malloc");
        exit(1);
    }

    int temp = cycle_option(argc, argv, 0, 0, NULL);
    if (temp != 1) {
        print_error(argc, argv, 0, "Invalid arguments.");
    }
    
    struct option options[] =
    {
        {"rdwr", required_argument, NULL, O_RDWR},
        {"rdonly", required_argument, NULL, O_RDONLY},
        {"wronly", required_argument, NULL, O_WRONLY},
        {"command", required_argument, NULL, _command},
        {"verbose", no_argument, NULL, _verbose},
        {"abort", no_argument, NULL, _abort},
        {"default", required_argument, NULL, _default},
        {"ignore", required_argument, NULL, _ignore},
        {"catch", required_argument, NULL, _catch},
        {"pause", no_argument, NULL, _pause},
        {"close", required_argument, NULL, _close},
        {"wait", no_argument, NULL, _wait},

        //flags
        {"append", no_argument, NULL, O_APPEND},
        {"cloexec", no_argument, NULL, O_CLOEXEC},
       {"creat", no_argument, NULL, O_CREAT},
        {"directory", no_argument, NULL, O_DIRECTORY},
        {"dsync", no_argument, NULL, O_DSYNC},
        {"excl", no_argument, NULL, O_EXCL},
        {"nofollow", no_argument, NULL, O_NOFOLLOW},
        {"nonblock", no_argument, NULL, O_NONBLOCK},
        {"rsync", no_argument, NULL, O_RSYNC},
        {"sync", no_argument, NULL, O_SYNC},
        {"trunc", no_argument, NULL, O_TRUNC}

    };
    
    int option_index = 0;
    int opt;
    int i;
    int index;
    char* arg_error = "Incorrect number of arguments.";
    char *end;
    char whitespace;
    int expected_num_args;

    while ((opt = getopt_long(argc, argv, "", options, &option_index)) != -1) {
        index = optind - 1;
        expected_num_args = 1;

        switch (opt) {
            case O_RDWR:
            case O_RDONLY:
            case O_WRONLY:
            case _command:
            case _default:
            case _ignore:
            case _catch:
            case _close:
                index--;
                expected_num_args = 2;
            case _verbose:
            case _pause:
            case _abort:
            case _wait:
                whitespace = '\n';
                break;
            default:
                whitespace = ' ';
                break;
        }

        int args = cycle_option(argc, argv, index, verbose_flag, stdout);

        if (verbose_flag) {
            putchar(whitespace);
        }

        if (opt != _command) {
            if (args != expected_num_args) {
                print_error(argc, argv, index, arg_error);
                if (expected_num_args == 2 && args == 1) {
                    optind--;
                }
                continue;
            }
        }
        else {
            if (args < 5) {
                print_error(argc, argv, index, arg_error);
                if (args == 1) {
                    optind--;
                }
                continue;
            }
        }
        
        switch (opt) {
            case O_APPEND:
            case O_CLOEXEC:
            case O_CREAT:
            case O_DIRECTORY:
            case O_DSYNC:
            case O_EXCL:
            case O_NOFOLLOW:
            case O_NONBLOCK:
            case O_RSYNC:
#if O_RSYNC != O_SYNC
            case O_SYNC:
#endif
            case O_TRUNC:
                open_flags |= opt;
                break;
            case O_RDWR:
            case O_RDONLY:
            case O_WRONLY:
                open_flags |= opt;
                if (!open_file(optarg, open_flags)) {
                    print_error(argc, argv, index, NULL);
                    exit_status = max(1, exit_status);
                }
                open_flags = 0;
                break;
            case _command:
                open_flags = 0;
                //get the stdin file descriptor
                index++;
                int stdin_logical_fd = strtol(argv[index], &end, 10);
                if (end == argv[index]) {
                    print_error(argc, argv, index - 1, "Invalid file descriptor input for stdin.");
                    break;
                }
                if ((stdin_logical_fd >= no_of_files) || (stdin_logical_fd < 0)) {
                    print_error(argc, argv, index - 1, "Invalid file descriptor number for stdin.");
                    break;
                }
                if (filesystem[stdin_logical_fd] == -1) {
                    print_error(argc, argv, index - 1, "stdin file number has been closed.");
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
                if (filesystem[stdout_logical_fd] == -1) {
                    print_error(argc, argv, index - 2, "stdout file number has been closed.");
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
                if (filesystem[stderr_logical_fd] == -1) {
                    print_error(argc, argv, index - 3, "stderr file number has been closed.");
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
                i = 1;
                if (num_args != 0) {
                    while(i <= num_args) {
                        args_list[i] = argv[index + i];
                        i++;
                    }
                }
                args_list[i] = NULL;
                optind = index + i;
                call_command(num_args + 1, args_list, index, stdin_real_fd, stdout_real_fd, stderr_real_fd);
                free(args_list);
                break;
            case _verbose:
                if (verbose_flag) {
                    print_error(argc, argv, index, "--verbose has already been called.");
                }
                open_flags = 0;
                verbose_flag = 1;
                break;
            case _default:
            case _ignore:
            case _catch:
                open_flags = 0;
                int n = strtol(argv[index+1], &end, 10);
                if (end == argv[index]) {
                    print_error(argc, argv, index, "Argument is not an integer.");
                }
                else if (n < sig_min || n > sig_max) {
                    print_error(argc, argv, index, "Invalid signal value.");
                }
                else if (opt == _default) {
                    signal(n, SIG_DFL);
                }
                else if (opt == _ignore) {
                    signal(n, SIG_IGN);
                }
                else {
                    signal(n, handler);
                }
                break;
            case _pause:
            case _abort:
                open_flags = 0;
                if (opt == _pause) {
                    pause();
                }
                else {
                    /* signal(11, SIG_DFL); */
                    raise(SIGSEGV);
                }
                break;
            case _close:
                open_flags = 0;
                int close_no = strtol(argv[index+1], &end, 10);
                if (end == argv[index]) {
                    print_error(argc, argv, index, "Argument is not an integer.");
                }
                else if (close_no < 0 || close_no >= no_of_files) {
                    print_error(argc, argv, index, "Invalid file number.");
                }
                else if (close(filesystem[close_no]) == -1) {
                    print_error(argc, argv, index, NULL);
                }
                else {
                    filesystem[close_no] = -1;
                }
                break;
            case _wait:
                open_flags = 0;
                int stat_loc;
                for (i = 0; i < no_of_processes; i++) {
                    waitpid(processes[i], &stat_loc, 0); 
                    //returns -1 if error. maybe do something. . . ?
                    int status;
                    if (WIFEXITED(stat_loc)) {
                        status = WEXITSTATUS(stat_loc);
                    } else if (WIFSIGNALED(stat_loc)) {
                        int sig_no = WTERMSIG(stat_loc);
                        raise(sig_no);
                        status = 1;
                    }
                    exit_status = max(exit_status, status);
                    cycle_option(argc, argv, commands[i], 1, stdout);
                    fprintf(stdout, " exited with status %d.\n", status);
                }
                no_of_processes = 0;
                break;
            default:
                break;
        }
    }
    free(filesystem);
    free(processes);
    free(commands);
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

void handler(int n) {
    fprintf(stderr, "%d%s\n", n, " caught");
    exit(n);
}
