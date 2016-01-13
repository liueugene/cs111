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
extern int no_of_ignores;
extern int max_ignores;
extern int *ignore_list;
extern int exit_status;

int max(int a, int b);
int open_file(char *filename, int flags);
void call_command(int argc, char* argv[], int stdin_fd, int stdout_fd, int stderr_fd);
void add_ignore(int n);
int should_ignore(int n);

#endif