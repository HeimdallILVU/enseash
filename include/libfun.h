#define WELCOME_MESSAGE "Bienvenue dans le Shell ENSEA.\n"
#define TO_EXIT_MESSAGE "Pour quitter, tapez 'exit'.\n"
#define EXIT_MESSAGE "Bye bye...\n"

#define WRITE_ERROR "Erreur lors d'un write.\n"
#define READ_ERROR "Erreur lors d'un read.\n"
#define FORK_ERROR "Erreur lors d'un fork.\n"
#define EXEC_ERROR "Erreur lors d'un exec.\n"
#define FD_ERROR "Erreur lors d'un open.\n"
#define PIPE_ERROR "Erreur lors d'un pipe.\n"

#define SEPARATOR " "

#define HEADER "enseash % "
#define HEADER_EXIT "enseash [exit : %d | %ld ms] %% "
#define HEADER_SIGN "enseash [sign : %d | %ld ms] %% "

#define MAX_INPUT_SIZE 256
#define MAX_OUTPUT_SIZE 256

// ----- FUN -----------

void print_message(char * message);
void print_welcome(void);
void print_to_exit(void);
void print_header(void);

void process_inputs(void);