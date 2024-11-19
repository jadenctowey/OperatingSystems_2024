#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE 80                  // max lenght command

int main(void){
    char *args[MAX_LINE/2 +1];      // command line arguments
    int should_run = 1;             // flag determines when to exit program 
    char user_input[MAX_LINE];

    while (should_run){
        printf("osh>");
        fflush(stdout);

        if (fgets(user_input, MAX_LINE, stdin) == NULL) {     //user input here
            perror("fgets failed");
            continue;
        }

        size_t length = strlen(user_input);      //remove the trailing newline
        if (user_input[length - 1] == '\n'){
            user_input[length -1] = '\0';

        }

        if (strlen(user_input) == 0) {      //check if the user input is empty
            continue;
        }

        int i = 0;        //tokenize the user input
        char *token = strtok(user_input, " ");

        while (token != NULL){
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;

        if (strcmp(args[0], "exit") == 0) {       //looks for exit 
            should_run = 0;
            continue;
        }

        int background = 0;          //look if the command should run in the background
        if (strcmp(args[i - 1], "&") == 0){
            background = 1;
            args[i - 1] = NULL;
        }

        pid_t pid = fork();        //fork the child process

        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        }
        else if (pid == 0){
            printf("Executing command: %s\n", args[0]);     //the child process
            if (execvp(args[0], args) == -1){
                perror("execvp failed");
            }
            exit(1);
        }
        else{
            if (!background){           //the parent process
                waitpid(pid, NULL, 0);
            }
        }
        
    }
    return 0;
}