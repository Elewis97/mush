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


void exec_redir(struct Stage **stages, int stage_len) {

    int i = 0;

    for(i = 0; i < stage_len; i++) {

        
    }

}


void executeC(struct Stage **stage_list, int stage_len) {

    int status = 0;
    char *argv[CMAX];
    char temp[CMAX] = {0};

    struct Stage *stages = stage_list[0];

    if(strcmp(stages -> input, "stdin") ||
        strcmp(stages -> output, "stdout")) {
            exec_redir(stage_list, stage_len);
            return;
    }

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


int main (int argc, char *argv[])
{
    struct Stage *stage_list[CMAX] = {NULL};
    int stage_len = 0;

    while(1) {
        if(getLine(stage_list, &stage_len)) {
            continue;
        }
        executeC(stage_list, stage_len);
    }

	return 0;
}
