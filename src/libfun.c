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

void print_header() {
    print_message(HEADER);
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
    input[size - 1] = '\0'; // remove the '\n'
    exec_fun(input);
    return 0;
}

void process_inputs() {
    char input[MAX_INPUT_SIZE];
    ssize_t byteread;
    while(1) { // Boucle infini de lecture des inputs utilisateurs.

        if((byteread = read(STDIN_FILENO, input, sizeof(input))) == -1) {
            read_error();
        } else {
            input_interpreter(input, byteread);
        }
        print_header();
    }
}


