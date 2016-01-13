#ifndef FILEOPTIONS_H
#define FILEOPTIONS_H

#define APPEND 0
#define CLOEXEC 1
#define CREAT 2
#define DIRECTORY 3
#define DSYNC 4
#define EXCL 5
#define NOFOLLOW 6
#define NONBLOCK 7
#define RSYNC 8
#define SYNC 9
#define TRUNC 10
#define RDWR 11
#define RDONLY 12
#define WRONLY 13

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