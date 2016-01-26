#include "fileoptions.h"

int open_file(char *filename, int flags)
{
    //adding a file:
    if (no_of_files == max_files) {
        max_files = max_files * 2;
        int* temp_filesystem = realloc(filesystem, max_files * sizeof(int));
        int* temp_ispipe = realloc(ispipe, max_files * sizeof(int));
        if (temp_filesystem == NULL || temp_ispipe == NULL) {
            fprintf(stderr, "%s\n", "Unable to allocate space for the array.");
            exit(max(exit_status, 1));
        }
        filesystem = temp_filesystem;
        ispipe = temp_ispipe;
    }
    int temp_fd_holder = open(filename, flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
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
    ispipe[no_of_files] = 0;
    no_of_files++;
    return 1;
}

void call_command(int argc, char* argv[], int index, int stdin_fd, int stdout_fd, int stderr_fd)
{
    pid_t pid = fork();
    
    //child process
    if (pid == 0) {
        
        if (dup2(stdin_fd, 0) < 0) {
            exit(1);
        }
        if (dup2(stdout_fd, 1) < 0) {
            exit(1);
        }
        if (dup2(stderr_fd, 2) < 0) {
            exit(1);
        }

        for (int i = 0; i < no_of_files; i++) {
            close(filesystem[i]);
        }

        execvp(argv[0], argv);
        perror(NULL);
        exit(1);
        
    } else if (pid == -1) { //error forking
        perror("fork");
        exit_status = max(exit_status, 1);
        
    } else { //parent process
        if (no_of_processes == max_processes) {
            max_processes = max_processes * 2;
            int* temp_processes = realloc(processes, max_processes * sizeof(int));
            int* temp_cmds = realloc(commands, max_processes * sizeof(int));
            if (temp_processes == NULL || temp_cmds == NULL) {
                fprintf(stderr, "%s\n", "Unable to allocate space for the array.");
                exit(max(exit_status, 1));
            }
            processes = temp_processes;
            commands = temp_cmds;
        }
        processes[no_of_processes] = pid;
        commands[no_of_processes] = index;
        no_of_processes++;
    }
}
