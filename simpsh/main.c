#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

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
    int opt = getopt_long(argc, argv, "", options, &option_index);
    
    while (1) {
        if (opt == -1) //exit after parsing all options
            break;
        
        switch (opt) {
            //random comment
            case 'r':
                open_flags |= O_RDONLY;
                open_file(optarg, open_flags);
                break;
            case 'w':
                open_flags |= O_WRONLY;
                open_file(optarg, open_flags);
                break;
            case 'c':
                break;
        }
        
        if (opt != 'c')
            opt = getopt_long(argc, argv, "", options, &option_index);
        else
            opt = temp_opt;
    }
}

