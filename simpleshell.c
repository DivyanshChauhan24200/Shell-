#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

#define MAX_LINE 80  // maximum length of a command that user can enter
#define MAX_ARGS 20 // maximum arguments that user can exexcute in a single go
#define MAX_HISTORY 50 // atmost we can store 50 commands as history of session
#define MAX_command_array 10 // maximum commands which caan excute thorugh pipe in this shell
typedef struct {
    pid_t pid; // process id of children
    char command[MAX_LINE]; // command with thier arguments that user enters on shell
    time_t start_time; // start time of a process
    double duration; // time taken by process to complete execution
} Command_History;

Command_History history_log[MAX_HISTORY]; // global array used to store command which user enter.

int history_count = 0;  // counter for how many commands are present in array of command

void launch(char **args, pid_t *child_pid) {
    pid_t pid = fork();
     // create child and stores process id.
    if (pid < 0) { // check whether child created or not.
        perror("SimpleShell: Fork error; something bad happen");  // if fork giving negative value then throw error.
        exit(0); // parent exists.
    } else if (pid == 0) { // child created successfully.
        if (execvp(args[0], args) == -1) { // checking wether execvp is correctly executed or not.
            perror("Command executbale does not exist"); // if not throw an error.

        }
        exit(0); // child exit
    } else {
        *child_pid=pid;

    }
}
void termination_handler(int signum) {  // handling signal 
    if (signum == SIGINT) {  // if signal is crt+c means to terminate it so we want it to handle it own.
        char buffer[256]; // a tempporaray string for formatting.
        write(STDOUT_FILENO, "\nShell terminating. Command statistics:\n", 40);
        
        for (int i = 0; i < history_count; i++) {  // loops through every command which we have in history log.
             char *time_str = ctime(&history_log[i].start_time);
            time_str[strcspn(time_str, "\n")] = 0;
            int len = snprintf(buffer, sizeof(buffer),              // snprintf "prints" a formatted string into the 'buffer' variable.
                             "  - PID: %d, Start: %s, Duration: %.2f sec, Command: [%s]\n",
                             history_log[i].pid,
                             time_str,
                             //history_log[i].start_time,
                             history_log[i].duration,
                             history_log[i].command);
            write(STDOUT_FILENO, buffer, len);  // write the content of the buffer to the screen.
        }
        exit(0); // terminate the shell 
    } 
}
int parse_pipes(char **args, char **command_array[]) {
    int command_count = 0;  // used to stores how many commands we have to execute using pipes.
    command_array[command_count++] = args; // the first command always starts at the begining of the list.


    for (int i = 0; args[i] != NULL; i++) { // iterates through all arguments to find pipe symbol.
        if (strcmp(args[i], "|") == 0) { 
            args[i] = NULL; // when a pipe is found, replace it with NULL, means marking this as a end of the command

            command_array[command_count++] = &args[i + 1]; // assign the next command after the pipe was found 
        }
    }
    command_array[command_count] = NULL; // end the list of commands with NULL.
    return command_count;  // returns the total number of commands found which user want to execute using pipes.
}

void execute_pipeline(char **command_array[], int num_command_array, pid_t pids[]) {
    int fd[2];   // file descriptor of size 2
    int input_fd = STDIN_FILENO;  //this variable holds the input source for the current command in the loop, it starts as the keyboards press.
    
    for (int i = 0; i < num_command_array; i++) {  // iterates through all commands in array of commands using for loop.
        if (i < num_command_array - 1) { // check if we are not at last command.
            if (pipe(fd) < 0) {
                perror("SimpleShell: Pipe error");
                return;
            }
        }
        if ((pids[i] = fork()) < 0) {
            perror("SimpleShell: Fork error; something bad happen");
            return;
        }

        if (pids[i] == 0) { // Child process logic is INSIDE the loop
            if (input_fd != STDIN_FILENO) { // check wether it is a first process.
                dup2(input_fd, STDIN_FILENO); // if not a first process read from prvious pipe.
                close(input_fd);
            }
            if (i < num_command_array - 1) { // if it is not the last command
                dup2(fd[1], STDOUT_FILENO); // pass the output to a new pipe 
                close(fd[0]);
                close(fd[1]);
            }
            if (execvp(command_array[i][0], command_array[i]) < 0) {  // execute the command
                perror("SimpleShell : Command executable does not exist in bin");
                exit(EXIT_FAILURE);
            }
        }

        // Parent process logic is ALSO INSIDE the loop
        if (input_fd != STDIN_FILENO) { //close the read end of prvious used pipe.
            close(input_fd);
        }
        if (i < num_command_array - 1) {  
            close(fd[1]); // close the write end of new pipe since it is already read from prvious pipe and write it.
            input_fd = fd[0];  // save the read pipe end for the next loop.
        }
    } 
}

