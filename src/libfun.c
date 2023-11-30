#define __USE_POSIX199309 1 // for time.h to define timespec and all
#include <time.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>


#include <sys/wait.h>

#include "libfun.h"

void print_message(char * message) {
    write(STDOUT_FILENO, message, strlen(message));
}

void print_welcome() {
    print_message(WELCOME_MESSAGE);
}

void print_to_exit() {
    print_message(TO_EXIT_MESSAGE);
}

void print_exit() {
    print_message(EXIT_MESSAGE);
}

void print_header() {
    print_message(HEADER);
}

void print_header_exit(int status, long elapsed_time) {
    char output[MAX_OUTPUT_SIZE];
    sprintf(output, HEADER_EXIT, status, elapsed_time);
    print_message(output);
}

void print_header_sign(int status, long elapsed_time) {
    char output[MAX_OUTPUT_SIZE];
    sprintf(output, HEADER_SIGN, status, elapsed_time);
    print_message(output);
}

void print_error(char * message) {
    write(STDERR_FILENO, message, strlen(message));
}

void write_error() {
    print_error(WRITE_ERROR);
}

void read_error() {
    print_error(READ_ERROR);
}

void fork_error() {
    print_error(FORK_ERROR);
}

void exec_error() {
    print_error(EXEC_ERROR);
}

void open_error() {
    print_error(OPEN_ERROR);
}
void fstat_error(){
    print_error(FSTAT_ERROR);
}

int internal_command(char * input) {
    if(strcmp(input, "exit") == 0) {
        print_exit();
        exit(EXIT_SUCCESS);
    } 
    return 0;
}

int exec_fun(char * args[]) {
    int status;
    __pid_t pid;
    if((pid = fork()) == -1) {
        fork_error();
    }

    if(pid == 0) { // I'm the son
 
        execvp(args[0], args);
        exec_error();
        exit(EXIT_FAILURE);

    } else { // I'm the father

        wait(&status);

    }

    return status;
} 

int Write_file(char ** args){
    int index = 0;
    int status = 0;

        while(args[index]!=NULL){
        if (strcmp(args[index],">")  && (index > 0) && (args[index + 1]!=NULL)){
            int OUTPUT = dup(STDOUT_FILENO);
            char* filename = args[index+1];
            status = exec_fun(args);
            dup2(STDOUT_FILENO,OUTPUT);
            struct stat statBuffer;
            if (fstat(OUTPUT, &statBuffer) == -1) {
                close(OUTPUT);
                fstat_error();
                status = -1;
            }
            int buffer_size = statBuffer.st_blksize;
            char write_buffer[buffer_size];
            ssize_t bytesRead, bytesWritten;
            int file_fd = open(filename,  O_WRONLY | O_CREAT | O_EXCL,statBuffer.st_mode);

            if(file_fd  == -1){
                close(file_fd);
                open_error();
                status = -1;

            }

            while ((bytesRead = read(OUTPUT, write_buffer, sizeof(write_buffer))) > 0) {
                bytesWritten = write(file_fd, write_buffer, bytesRead);
                if (bytesWritten != bytesRead) {
                    close(OUTPUT);
                    close(file_fd);
                    write_error();
                    status = -1;  
                }
            }
            if(bytesRead == -1){
                close(OUTPUT);
                close(file_fd);
                read_error();
                status = -1;
            }
        }
        index++;
    }
    
    return status;
}

int input_interpreter(char * input, int size) {
    int status;
    int pipefd[2];

    if(pipe(pipefd) == -1){
        print_error(PIPE_ERROR);
    }
    input[size - 1] = '\0'; // remove the '\n'

    char ** args = malloc(sizeof(char *) * strlen(input));
    char * token;
    int index = 0;
    token = strtok(input, SEPARATOR); // Cut the input string into arguments using the SEPARATOR as Separator. "hostname -i" -> "hostname" "-i"
 
    while(token != NULL) {
        args[index] = token;
        index += 1;
        token = strtok(NULL, SEPARATOR);
    }

    args[index] = NULL; // Adding the NULL value so the execvp know it's the end of the args

    if((status = internal_command(input)) == 0) { // checking and running commands if they are custom to this shell
        status = Write_file(args);
        status = exec_fun(args);        
    }

    return status;
}




void process_inputs() {
    char input[MAX_INPUT_SIZE];
    
    ssize_t byteread;

    int status;

    struct timespec start_time, end_time; // structs to handle time measurement
    long elapsed_time;

    while(1) { // Infinite loop reading the input of the users.

        if((byteread = read(STDIN_FILENO, input, sizeof(input))) == -1) {
            read_error();
        } else {
            if(byteread == 0) {
                print_exit();
                exit(EXIT_SUCCESS);
            }
            clock_gettime(_POSIX_MONOTONIC_CLOCK, &start_time); 
            status = input_interpreter(input, byteread);
            clock_gettime(_POSIX_MONOTONIC_CLOCK, &end_time);

            elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 + (end_time.tv_nsec - start_time.tv_nsec) / 1e6; // Convert time to ms

        }

        if (WIFEXITED(status)) { // Change the Header following the returned status of the previous command
            print_header_exit(WEXITSTATUS(status), elapsed_time);
        } else if (WIFSIGNALED(status)) {
            print_header_sign(WTERMSIG(status), elapsed_time);
        } 
        
    }
}


