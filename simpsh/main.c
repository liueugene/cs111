#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include "fileoptions.h"

int cycle_option(int argc, char* argv[], int long_index, int print, FILE *print_to);
void print_error(int argc, char* argv[], int long_index, char* error);
int max(int a, int b);
void handler(int n);

int verbose_flag = 0;
int v_newline_flag = 0;
int profile_flag = 0;
int open_flags = 0;
int no_of_files = 0;
int max_files = 5;
int* filesystem;
int exit_status = 0;
int* processes;
int* ispipe;
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
#define _pipe 109
#define _profile 110

#ifndef O_RSYNC
#define O_RSYNC O_SYNC
#endif

int main(int argc, char *argv[])
{
    filesystem = malloc(max_files * sizeof(int));
    ispipe = malloc(max_files * sizeof(int));
    processes = malloc(max_processes * sizeof(int));
    commands = malloc(max_processes * sizeof(int));

    if (filesystem == NULL || ispipe == NULL || processes == NULL || commands == NULL) { 
        perror("malloc");
        exit(1);
    }

    int j = 0;
    for (; j < max_files; j++) {
        ispipe[j] = 0;
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
        {"pipe", no_argument, NULL, _pipe},
        {"profile", no_argument, NULL, _profile},


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
        {"trunc", no_argument, NULL, O_TRUNC},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int opt;
    int i;
    int index;
    char* arg_error = "Incorrect number of arguments.";
    char *end;
    char whitespace;
    int expected_num_args;

    //get usage variables
    struct rusage tmp_rusage;
    getrusage(RUSAGE_CHILDREN, &tmp_rusage);

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
            case _pipe:
            case _verbose:
            case _pause:
            case _abort:
            case _wait:
            case _profile:
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
            case _pipe: {
                int pipefd[2];
                if (open_flags & (~(O_NONBLOCK | O_CLOEXEC))){
                    print_error(argc, argv, index, "Invalid flag for pipe.");
                    open_flags = 0;
                    break;
                }
                if (pipe2(pipefd, open_flags) == -1) {
                    print_error(argc, argv, index, NULL);
                    exit(max(exit_status, 1));
                }
                if ((no_of_files + 1) >= max_files) {
                    max_files = max_files * 2;
                    int* temp_filesystem = realloc(filesystem, max_files * sizeof(int));
                    int* temp_ispipe = realloc(ispipe, max_files * sizeof(int));
                    if (temp_filesystem == NULL || temp_ispipe == NULL) {
                        fprintf(stderr, "%s\n", "Unable to allocate space for the array.");
                        exit(max(exit_status, 1));
                    }
                    filesystem = temp_filesystem;
                    ispipe = temp_ispipe;
                }
                open_flags = 0;
                filesystem[no_of_files] = pipefd[0];
                ispipe[no_of_files] = 1;
                filesystem[no_of_files + 1] = pipefd[1];
                ispipe[no_of_files + 1] = 1;
                no_of_files += 2;
                break;
            }
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
            case _command:
                open_flags = 0;
                //get the stdin file descriptor
                int stdin_logical_fd = strtol(argv[index + 1], &end, 10);
                if (end == argv[index + 1]) {
                    print_error(argc, argv, index, "Invalid file descriptor input for stdin.");
                    break;
                }
                if ((stdin_logical_fd >= no_of_files) || (stdin_logical_fd < 0)) {
                    print_error(argc, argv, index, "Invalid file descriptor number for stdin.");
                    break;
                }
                if (filesystem[stdin_logical_fd] == -1) {
                    print_error(argc, argv, index, "stdin file number has been closed.");
                    break;
                }
                int stdin_real_fd = filesystem[stdin_logical_fd];
                //get the stdout file descriptor
                int stdout_logical_fd = strtol(argv[index + 2], &end, 10);
                if (end == argv[index + 2]) {
                    print_error(argc, argv, index, "Invalid file descriptor input for stdout.");
                    break;
                }
                if ((stdout_logical_fd >= no_of_files) || (stdout_logical_fd < 0)) {
                    print_error(argc, argv, index, "Invalid file descriptor number for stdout.");
                    break;
                }
                if (filesystem[stdout_logical_fd] == -1) {
                    print_error(argc, argv, index, "stdout file number has been closed.");
                    break;
                }
                int stdout_real_fd = filesystem[stdout_logical_fd];
                //get the stderr file descriptor
                int stderr_logical_fd = strtol(argv[index + 3], &end, 10);
                if (end == argv[index + 3]) {
                    print_error(argc, argv, index, "Invalid file descriptor input for stderr.");
                    break;
                }
                if ((stderr_logical_fd >= no_of_files) || (stderr_logical_fd < 0)) {
                    print_error(argc, argv, index, "Invalid file descriptor number for stderr.");
                    break;
                }
                if (filesystem[stderr_logical_fd] == -1) {
                    print_error(argc, argv, index, "stderr file number has been closed.");
                    break;
                }
                int stderr_real_fd = filesystem[stderr_logical_fd];
                //get the command string
                char* command = argv[index + 4];
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
                        args_list[i] = argv[index + 4 + i];
                        i++;
                    }
                }
                args_list[i] = NULL;
                optind = index + 4 + i;
                call_command(num_args + 1, args_list, index + 4, stdin_real_fd, stdout_real_fd, stderr_real_fd);

                if (ispipe[stdin_logical_fd] == 1) {
                    close(stdin_real_fd);
                    filesystem[stdin_logical_fd] = -1;
                }

                if (ispipe[stdout_logical_fd] == 1) {
                    close(stdout_real_fd);
                    filesystem[stdout_logical_fd] = -1;
                }

                if (ispipe[stderr_logical_fd] == 1) {
                    close(stderr_real_fd);
                    filesystem[stderr_logical_fd] = -1;
                }

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
                int retval;
                int n = strtol(argv[index+1], &end, 10);
                if (end == argv[index]) {
                    print_error(argc, argv, index, "Argument is not an integer.");
                    break;
                }
                
                if (opt == _default) {
                    retval = signal(n, SIG_DFL);
                }
                else if (opt == _ignore) {
                    retval = signal(n, SIG_IGN);
                }
                else {
                    retval = signal(n, handler);
                }
                
                if (retval == SIG_ERR)
                    print_error(argc, argv, index, NULL);
                
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
            case _wait:
                open_flags = 0;
                int stat_loc;
                for (i = 0; i < no_of_processes; i++) {
                    waitpid(processes[i], &stat_loc, 0); 
                    //returns -1 if error. maybe do something. . . ?
                    int status = 1;
                    if (WIFEXITED(stat_loc)) {
                        status = WEXITSTATUS(stat_loc);
                    } else if (WIFSIGNALED(stat_loc)) {
                        int sig_no = WTERMSIG(stat_loc);
                        raise(sig_no);
                    }
                    exit_status = max(exit_status, status);
                    fprintf(stdout, "%d ", status);
                    cycle_option(argc, argv, commands[i], 1, stdout);
                    putchar('\n');
                }
                no_of_processes = 0;
                break;
            case _profile:
                profile_flag = 1;
                break;
            default:
                break;
        }

        struct rusage usage;
        if (getrusage(RUSAGE_CHILDREN, &usage) == -1) {
            print_error(argc, argv, index, NULL); 
        }        
        else if (profile_flag) {

                if (!verbose_flag)
                    cycle_option(argc, argv, index, 1, stdout);
                
                printf("\nUser CPU time: %ld.%06ld\n", usage.ru_utime.tv_sec - tmp_rusage.ru_utime.tv_sec, usage.ru_utime.tv_usec - tmp_rusage.ru_utime.tv_usec);
                printf("System CPU time: %ld.%06ld\n", usage.ru_stime.tv_sec - tmp_rusage.ru_stime.tv_sec, usage.ru_stime.tv_usec - tmp_rusage.ru_stime.tv_usec);
                printf("Maximum resident set size: %ld\n", usage.ru_maxrss - tmp_rusage.ru_maxrss);
                printf("Page reclaims: %ld\n", usage.ru_minflt - tmp_rusage.ru_minflt);
                printf("Page faults: %ld\n", usage.ru_majflt - tmp_rusage.ru_majflt);
                printf("Block input operations: %ld\n", usage.ru_inblock - tmp_rusage.ru_inblock);
                printf("Block output operations: %ld\n", usage.ru_oublock - tmp_rusage.ru_oublock);
                printf("Voluntary context switches: %ld\n", usage.ru_nvcsw - tmp_rusage.ru_nvcsw);
                printf("Involuntary context switches: %ld\n", usage.ru_nivcsw - tmp_rusage.ru_nivcsw);
        }
        tmp_rusage = usage;
    }
    free(filesystem);
    free(processes);
    free(commands);
    exit(exit_status);
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
    return (a > b) ? a : b;
}

void handler(int n) {
    fprintf(stderr, "%d%s\n", n, " caught");
    exit(n);
}
