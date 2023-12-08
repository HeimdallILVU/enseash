# enseash
This simple shell program was develloped following ENSEA System Prog Course.

## TP 1 : Creation of a simple shell

### Quesiton 1
    For this first question, the shell display a Welcome message at the start, followed by an how-to-exit message.
    Finaly a prompt is displayed.
    To displayed content, we send byte to the STDOUT_FILENO fd.
    At this stage the code is very basic and does take user input into consideration.

### Question 2
    With this question a while(1) is introduced. At this point, user input are read in one chunk and sent to execlv without much consideration.
    We need to use a fork to use execlv, so that we can continue register user input at the end of the instruction. (Father -> wait; Child -> execlv)
    To print the output, we don't need to do much, execlv just output in STDOUT_FILENO.

### Question 3
    Adding "local" instruction was as simple as matching the user input with a list of all the "local" instruction. If the user input match exactly "exit" then the execlv is not called, but a special function is runned instead.
    For the <ctrl> + d, we assume that if no byte is read then it must be because of a <ctrl> + d.

### Question 4
    Using the status that we get using the wait function inside the parent, we can get every information that we require.
    Using defined MACROS with "%d" inside allow us the use of sprintf to format the string with the code exit or signal code.

### Question 5
    Requiring the time.h lib, we also define a POSIX because working using WSL create some issue...
    To add the time to the prompt, we use the same strategy that previously, we just add a "%ld" for long integer.
    We measure the time taken using the clock_gettime function that takes a clock_id and timespec struct as input.

### Question 6
    We add a input formatter code, that will chop the input into segments using the spaces as delimiter. we add NULL at the end of the list.
    Once that is done, we can simply throw the freshly made args list into a execvp.

### Question 7
    At this point the code get a bit of a refactoring and things are moved a bit arround to make them cleaner.
    To handle the redirection ">" and "<", we need to modify the STDOUT_FILENO or STDIN_FILENO.
    After creating the args list, we navigate it looking for ">" or "<" symbole. If found we simply take the dup2(our_file_descriptor, STDIN or STDOUT _FILENO)

### Question 8
    Once again the whole code takes a bit of a face lift. We split the code into smaller functions because we will be using recursivity.
    We look for the "|" symbole inside the args, we found we split args before and after the symbole and we create a pipe, that links STDOUT of the exec that will runs args1 and STDIN of the exec that will run args2.
    Once the args1 and args2 are created, we call the call the input_interpreter function again but two different times, once with args1 and once with args2. (Small optimisation, as we chack linearly the args list, we know that args1 doesn't contains any more "|", so we can simply run execvp on it)
    
### Question 9
    Later