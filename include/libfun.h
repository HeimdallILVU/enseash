#define WELCOME_MESSAGE "Bienvenue dans le Shell ENSEA.\n"
#define TO_EXIT_MESSAGE "Pour quitter, tapez 'exit'.\n"
#define EXIT_MESSAGE "Bye bye...\n"

#define WRITE_ERROR "Erreur lors d'un write.\n"
#define READ_ERROR "Erreur lors d'un read.\n"
#define FORK_ERROR "Erreur lors d'un fork.\n"


#define HEADER "enseash % "

#define MAX_INPUT_SIZE 256

// ----- FUN -----------

void print_message(char * message);
void print_welcome(void);
void print_to_exit(void);
void print_header(void);

void process_inputs(void);