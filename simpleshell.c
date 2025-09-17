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
