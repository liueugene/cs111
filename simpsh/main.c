#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/stat.h>
#include <fcntl.h>

int verbose_flag = 0;
int open_flags = 0;
int no_of_files = 0;
int max_files = 15;
int* filesystem;
int verbose_flag2 = 0;

int main(int argc, char *argv[])
{
    filesystem = malloc(max_files * sizeof(int));
    struct option options[] =
    {
        {"rdonly", required_argument, NULL, 'r'},
        {"wronly", required_argument, NULL, 'w'},
        {"command", required_argument, NULL, 'c'},
        {"verbose", no_argument, &verbose_flag, 1}
    };
    
    int option_index = 0;
    int opt;

    while ((opt = getopt_long(argc, argv, "", options, &option_index)) != -1) {
        printf("%d\n", option_index);
        int args = Argument_Amount(argc, argv, option_index);
        switch (opt) {
            case 'r':
                if (args > 2) {
                    //error
                }
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                }
                open_flags |= O_RDONLY;
                open_file(optarg, open_flags);
                break;
            case 'w':
                if (args > 2) {
                    //error
                }
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                }
                open_flags |= O_WRONLY;
                open_file(optarg, open_flags);
                break;
            case 'c':
                if (args < 5) {
                    //error
                }
                if (verbose_flag) {
                    printf("\n");
                    verbose_flag2 = 0;
                }
                //get the stdin file descriptor
                optind--;
                int stdin_logical_fid = argv[optind];
                int stdin_real_fid = filesystem[stdin_logical_fid];
                //get the stdout file descriptor
                optind++;
                int stdout_logical_fid = argv[optind];
                int stdout_real_fid = filesystem[stdout_logical_fid];
                //get the stderr file descriptor
                optind++;
                int stderr_logical_fid = argv[optind];
                int stderr_real_fid = filesystem[stderr_logical_fid];
                //get the command string
                optind++;
                char* command = argv[optind];
                //find the number of args
                int num_args = args - 5;
                //get the args
                char** args = malloc((num_args + 1) * sizeof(char*));
                args[0] = command;
                if (num_args != 0) {
                    int i = 1;
                    while(i <= num_args) {
                        args[i] = argv[optind];
                        optind++;
                        i++;
                    }
                }
                //call the command via execvp
                free(args);
                break;
            default:
                break;

        }
    }
    free(filesystem); 
}

int Argument_Amount(int argc, char* argv[], int long_index)
{
    int count = 0;
    if (verbose_flag) {
        if (verbose_flag2) {
            printf(" ");
        }
        else {
            verbose_flag2 = 1;
        }
        printf("%s", argv[long_index]);
    }
    count++;
    while((argv[long_index + count][0] != '-') || (argv[long_index + count][1] != '-')) {
        if (verbose_flag) {
            printf(" %s", argv[long_index + count]);
        }
        count++;
        if ((long_index + count) == argc) {
            break;
        }
    }

    return count;
}

