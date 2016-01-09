#include "fileoptions.h"

extern int no_of_files;
extern int max_files;
extern int *filesystem;

void open_file(char *filename, int flags)
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

    filesystem[no_of_files] = open(filename, flags);//put code here
    no_of_files++;
}