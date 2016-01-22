#ifndef FILEOPTIONS_H
#define FILEOPTIONS_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

extern int no_of_files;
extern int max_files;
extern int *filesystem;
extern int no_of_ignores;
extern int max_ignores;
extern int *ignore_list;
extern int exit_status;
extern int *processes;
extern int *commands;
extern int no_of_processes;
extern int max_processes;

int max(int a, int b);
int open_file(char *filename, int flags);
int call_command(int argc, char* argv[], int index, int stdin_fd, int stdout_fd, int stderr_fd);
void add_ignore(int n);
int should_ignore(int n);

#endif