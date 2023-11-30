#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

