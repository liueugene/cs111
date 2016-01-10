#include "fileoptions.h"

int open_file(char *filename, int flags)
{
    //adding a file:
    if (no_of_files == max_files) {
            max_files = max_files * 2;
            int* temp_filesystem = realloc(filesystem, max_files * sizeof(int));
            if (temp_filesystem == NULL) {
                    fprintf(stderr, "%s\n", "Unable to allocate space for the array.");
                    exit(1);
            }
            filesystem = temp_filesystem;
    }
    int temp_fd_holder = open(filename, flags);
    if (temp_fd_holder == -1) {
        return 0;
    }
    filesystem[no_of_files] = temp_fd_holder;
 /* char* test = malloc(10 * sizeof(char));
    if (read(filesystem[no_of_files], test, 10) == -1) {
        fprintf(stderr, "%s\n", "Unable to read file.");
    }
    printf("%s\n", test);
    free(test); */
    no_of_files++;
    return 1;
}

int call_command(int argc, char* argv[], int stdin_fd, int stdout_fd, int stderr_fd)
{
    
}