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
extern int exit_status;

int max(int a, int b);
int open_file(char *filename, int flags);
int call_command(int argc, char* argv[], int stdin_fd, int stdout_fd, int stderr_fd);

#endif