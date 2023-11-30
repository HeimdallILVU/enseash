#include <unistd.h>
#include <string.h>
#include <stdio.h>
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

void print_header_exit(int status) {
    char output[MAX_OUTPUT_SIZE];
    sprintf(output, HEADER_EXIT, status);
    print_message(output);
}

void print_header_sign(int status) {
    char output[MAX_OUTPUT_SIZE];
    sprintf(output, HEADER_SIGN, status);
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

int internal_command(char * input) {
    if(strcmp(input, "exit") == 0) {
        print_exit();
        exit(EXIT_SUCCESS);
    } 
    return 0;
}

int exec_fun(char * input) {
    int status;
    __pid_t pid;
    if((pid = fork()) == -1) {
        fork_error();
    }

    if(pid == 0) { // I'm the son

        execlp(input, input, NULL);

    } else { // I'm the father

        wait(&status);

    }

    return status;
} 

int input_interpreter(char * input, int size) {
    int status;
    input[size - 1] = '\0'; // remove the '\n'

    if((status = internal_command(input)) == 0) { // checking and running commands if they are custom to this shell
        status = exec_fun(input);
    }

    return status;
}

void process_inputs() {
    char input[MAX_INPUT_SIZE];
    
    ssize_t byteread;

    int status;

    while(1) { // Infinite loop reading the input of the users.

        if((byteread = read(STDIN_FILENO, input, sizeof(input))) == -1) {
            read_error();
        } else {
            if(byteread == 0) {
                print_exit();
                exit(EXIT_SUCCESS);
            }

            status = input_interpreter(input, byteread);
        }

        if (WIFEXITED(status)) {
            print_header_exit(WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            print_header_sign(WTERMSIG(status));
        } 

        
    }
}


