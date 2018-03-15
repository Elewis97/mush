
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "parseline.h"
#include <string.h>
#include <signal.h>
#include <sys/types.h>


pid_t pid = 1;

void parseSomething(char *command, char **paramBuffer)
{
    int i;

    for (i = 0; i < PMAX; i++) {
        paramBuffer[i] = strsep(&command, " ");
        if(paramBuffer[i] == NULL)
            break;
    }
}


void executeC(struct Stage *stages) {

    int status = 0;
    char *argv[CMAX];
    char temp[CMAX] = {0};


    printf("%s\n", stages -> argv);

    strcpy(temp, stages -> argv);

    parseSomething(temp, argv);

    pid = fork();

    if(pid == -1) {
        fprintf(stderr, "Something's wrong with fork()\n");
        return;
    }

    if(pid == 0) {
        if(execvp(argv[0], argv) < 0) {
            fprintf(stderr, "Something's wrong with exec\n");
            exit(1);
        }
        exit(0);
    }
    else {
        wait(&status);
    }

}

// void clear_stages(struct Stage **stages, int len) {
//
//     int i = 0;
//
//     for(i = 0; i < len; i++) {
//
//         stages[i] = NULL;
//     }
//
// }

void ctrlHandler(int sig)
{
    signal(SIGINT, ctrlHandler);
    if(pid == 0) {
        exit(0);
    }

}



int main (int argc, char *argv[])
{
    struct Stage *stages[CMAX] = {NULL};
    int stage_len = 0;

    signal(SIGINT, ctrlHandler);

    while(1) {
        getLine(stages, &stage_len);
        executeC(stages[0]);
    }

	return 0;
}
