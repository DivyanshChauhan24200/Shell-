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
