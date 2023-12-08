#define __USE_POSIX199309 1 // for time.h to define timespec and all
#include <time.h>

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>

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

void fd_error() {
    print_error(FD_ERROR);
}

int internal_command(char * input) {
    if(strcmp(input, "exit") == 0) {
        print_exit();
        exit(EXIT_SUCCESS);
    } 
    return 0;
}

void print_args(char * args[]) {
    char * token = args[0];
    int index = 0;
    while (token != NULL)  {
        print_message(token);
        print_message(" ");
        index += 1;
        token = args[index];
    }
    print_message("\n");
}

int exec_fun(char * input, char * args[]) {
    int status;
    __pid_t pid;
    if((pid = fork()) == -1) {
        fork_error();
    }

    if(pid == 0) { // I'm the son

        execvp(input, args);
        exec_error();
        exit(EXIT_FAILURE);

    } else { // I'm the father

        wait(&status);

    }

    return status;
} 

void splitArrays(char* inputStrings[], int numStrings, char*** before, char*** after) {
    // Find the index of the "|" string
    int index = -1;
    for (int i = 0; i < numStrings; i++) {
        if (strcmp(inputStrings[i], "|") == 0) {
            index = i;
            break;
        }
    }

    // If "|" is not found, set index to the end of the array
    if (index == -1) {
        index = numStrings;
    }

    // Allocate memory for the "before" array
    *before = (char**)malloc((index + 1)* sizeof(char*));

    // Copy strings before "|"
    for (int i = 0; i < index; i++) {
        (*before)[i] = strdup(inputStrings[i]);
    }

    (*before)[index] = NULL;

    // Allocate memory for the "after" array
    *after = (char**)malloc((numStrings - index + 1) * sizeof(char*));

    // Copy strings after "|"
    for (int i = index + 1 ; i <= numStrings; i++) {
        (*after)[i - index - 1] = strdup(inputStrings[i]);
    }
    (*after)[numStrings - index + 1] = NULL;
}

char ** input_formater(char * input, int size) {

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

    return args;
}

int get_number_of_args(char * args[]) {
    char * token = args[0];
    int index = 0;
    while (token != NULL) {
        index += 1;
        token = args[index];
    }
    return index - 1;
}

int input_interpreter(char ** args) {

    int status;

    int saved_stdin = -1; // Saving the STD In and Out in case they are modfified later on.
    int saved_stdout = -1;

    saved_stdin = dup(STDIN_FILENO);
    saved_stdout = dup(STDOUT_FILENO);

    int index = 0;
    char * token = args[index];

    int size = get_number_of_args(args);



    while(token != NULL) { // Looking for '|' in the args
        if(strcmp(token, "|") == 0) {
            // Cutting args in two, before and after, and running then as separate inputs but with pipe taking STDOUT and writing it to STDIN

            char ** args1;
            char ** args2;

            //makes args1 and args2 from args

            splitArrays(args, size, &args1, &args2);
            
            int pipe_fd[2];
            int ret;
            if((ret = pipe(pipe_fd)) == -1) {
                print_error(PIPE_ERROR);
            }

            pid_t pid;

            if((pid = fork()) == -1 ) {
                fork_error();
            }

            if(pid == 0) { // I'm the son
                close(pipe_fd[0]);
                int saved_stdout = dup(STDOUT_FILENO);
                dup2(pipe_fd[1], STDOUT_FILENO);
                execvp(args1[0], args1);
                dup2(saved_stdout, STDOUT_FILENO);
                close(pipe_fd[1]);
                exit(EXIT_SUCCESS);

            } else { // I'm the father
                close(pipe_fd[1]);
                wait(&status);
                int saved_stdin = dup(STDIN_FILENO);
                dup2(pipe_fd[0], STDIN_FILENO);
                status = input_interpreter(args2);
                dup2(saved_stdin, STDIN_FILENO);
                close(pipe_fd[0]);
            }

            return status;
        } 

    
        index += 1;
        token = args[index];
    }

    index = 0;
    token = args[index];

    while(token != NULL) { // Looking for '>' or '<' in the args
        if(strcmp(token, ">") == 0) {
            int fd_output = open(args[index + 1], O_CREAT | O_WRONLY, 0666);

            if(fd_output == -1) {
                fd_error();
            }

            dup2(fd_output, STDOUT_FILENO); // Changing STDOUT to the file fd

            args[index] = NULL; // Removing args that are only related to the redirection
            args[index + 1] = NULL;
        }

        if(strcmp(token, "<") == 0) {
            int fd_input = open(args[index + 1], O_RDONLY  | O_CREAT, 0666);

            if(fd_input == -1) {
                fd_error();
            }

            dup2(fd_input, STDIN_FILENO); // Changing STDIN to the file fd

            args[index] = NULL; // Removing args that are only related to the redirection
            args[index + 1] = NULL;
        }

        index += 1;
        token = args[index];
        //strcpy(token, args[index]);
    }


    if((status = internal_command(args[0])) == 0) { // checking and running commands if they are custom to this shell
        status = exec_fun(args[0], args);
    }


    dup2(saved_stdin, STDIN_FILENO); // Changing back the STD In and Out, whether they have been modfied of not
    dup2(saved_stdout, STDOUT_FILENO);

    close(saved_stdin);
    close(saved_stdout);

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

            if(byteread != 1) {
                clock_gettime(_POSIX_MONOTONIC_CLOCK, &start_time); 
                char ** args = input_formater(input, byteread);
                status = input_interpreter(args);
                clock_gettime(_POSIX_MONOTONIC_CLOCK, &end_time);

                elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 + (end_time.tv_nsec - start_time.tv_nsec) / 1e6; // Convert time to ms
            }
        }

        if (WIFEXITED(status)) { // Change the Header following the returned status of the previous command
            print_header_exit(WEXITSTATUS(status), elapsed_time);
        } else if (WIFSIGNALED(status)) {
            print_header_sign(WTERMSIG(status), elapsed_time);
        } 

        
    }
}


