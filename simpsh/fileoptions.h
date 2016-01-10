#ifndef FILEOPTIONS_H
#define FILEOPTIONS_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

extern int no_of_files;
extern int max_files;
extern int *filesystem;

int open_file(char *filename, int flags);

#endif