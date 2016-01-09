#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <fcntl.h>

int verbose_flag = 0;
int open_flags = 0;
int no_of_files = 0;
int max_files = 15;
int* filesystem = malloc(max_files * sizeof(int));

int main(int argc, char *argv[])
{
    struct option options[] =
    {
        {"rdonly", required_argument, NULL, 'r'},
        {"wronly", required_argument, NULL, 'w'},
        {"command", required_argument, NULL, 'c'},
        {"verbose", no_argument, &verbose_flag, 1}
    }
    
    int option_index = 0;
    int opt;
    int last_optind = 1;

    while ((opt = getopt_long(argc, argv, "", options, &option_index)) != -1) {
        switch (opt) {
            case 'r':
                open_flags |= O_RDONLY;
                open_file(optarg, open_flags);
                break;
            case 'w':
                open_flags |= O_WRONLY;
                open_file(optarg, open_flags);
                break;
            case 'c':
                //get the stdin file descriptor
                int stdin_logical_fid = argv[last_optind + 1];
                int stdin_real_fid = filesystem[stdin_logical_fid];
                //get the stdout file descriptor
                int stdout_logical_fid = argv[last_optind + 2];
                int stdout_real_fid = filesystem[stdout_logical_fid];
                //get the stderr file descriptor
                int stderr_logical_fid = argv[last_optind + 3];
                int stderr_real_fid = filesystem[stderr_logical_fid];
                //get the command string
                char* command = argv[temp_index + 4];
                //find the number of flags
                int num_args = optind - last_optind;
                num_args = num_args - 5;
                //get the args
                char** args = malloc((num_args + 1) * sizeof(char*));
                args[0] = command;
                if (num_args != 0) {
                    int i = 5;
                    while((last_optind + i) < optind); {
                        args[i - 4] = argc[last_optind + i];
                        i++;
                    }
                }
                //call the command via execvp
                break;
        }
        last_optind = optind;
    }
}

