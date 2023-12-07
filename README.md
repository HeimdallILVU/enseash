# enseash
Q1 : 
For the first question we simply write a string message to the file descriptor STDOUT_FILENO.
We are creating functions to display messages that will be useful later on.

Q2 :
Defined many error messages and created the fucntions to display them on the console using the function defined in Q1.
Created exec_fun function that takes a string input being the command to execute. It first splits into parent and child process, then the child execute the command while the parent wait for the son to finish.
For now input_interpreter simply remove the \n of the input and pass it on to exec_fun.
Function process_inputs is where the while(1) loop is which allows to return to the prompt after executing a command. It reads the input in STDIN_FILENO and pass it on to input_interpreter.

Q3 : 
Adding internal_command function to detect if input is equal to "exit" using strcmp and then exiting the process. It is called in input_intepreter after the \n is removed.

Q4 :
Here, the goald is to display the status of the previous command executed.
The exec_fun now returns the status of the child process when it exits.
in the while(1) of process_input we pass on this status to WIFEXITED(status) and WIFSIGNALED(status). Then we display the status on the terminal using the print_header functions.

Q5 :

