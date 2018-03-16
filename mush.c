#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "parseline.h"
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

struct sigaction siga;
bool isSig;
char line[CMAX];

/*Things we need to do:
 - Make sure it doesn't exit at ctrl c*/

/*cat*/

pid_t parentPID;
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
    FILE *fp = NULL;
    int saved_in = 0;
    char buf = 0;
    bool input_changed = 0;
    int out = 0;
    int fstdout;
    int fstdin;


    // saved_in = dup(STDIN_FILENO);
    // printf("stlen: %d\n", stage_len);

    // /* change stdout to file if output isn't stdout*/
    // if (strcmp((*stages)->output, "stdout")) {
    //     printf("out\n");
    //     fstdout = open((*stages)->output, O_RDWR | O_CREAT,
    //         S_IRUSR | S_IWUSR);

    //     dup2(fstdout, 1); //make stdout go to file

    //     close(fstdout);
    // }

    // /*change stdin to file if input isn't stdin*/
    // if (strcmp((*stages) ->input, "stdin")) {
    //     printf("in\n");
    //     fstdin = open((*stages)->input, O_RDONLY, 
    //         S_IRUSR | S_IWUSR);

    //     dup2(fstdin, 0); //make stdin read from file

    //     close(fstdin);
    // }


    for(i = 0; i < stage_len; i++) {

        if(fp != NULL) {
            while((buf = fgetc(fp)) != EOF) {
                write(STDIN_FILENO, &buf, 1);
                // putchar(buf);
            }
            // buf = EOF;
            // putchar(buf);
            // write(STDIN_FILENO, &buf, 1);
        }
        // TODO print out input to stdin if input is a file
        fp = popen(stages[i] -> argv, "r");
        wait(NULL);

        if(i == stage_len - 1) {
            buf = 0;
            if(strcmp(stages[i] -> output, "stdout") == 0) {
                // fstdout = open((*stages)->output, O_RDWR | O_CREAT,
                //     S_IRUSR | S_IWUSR);

                // dup2(fstdout, 1); //make stdout go to file

                // close(fstdout);
            }
            else {
                fstdout = open((*stages)->output, O_RDONLY, 
                    S_IRUSR | S_IWUSR);

                dup2(fstdout, 0); //make stdin read from file

                close(fstdout);
            }

            // pclose(fp);
            fp = NULL;
        }
    }

}

void zeroLine()
{
    int i;

    for (i = 0; i < CMAX; i++) {
        line[i] = '\0';
    }
}


bool getLine(struct Stage **stages, int *stage_len)
{
    // char line[CMAX];
    char c;
    int idx = 0;
    char **tokens;
    int i;
    int len = 0;

    printf("8-p ");

    /*get command while checking if input
    exceeds command line length max (CMAX)*/
    while((c = getchar()) != '\n') {
        if (c == EOF)
            exit(3);
        line[idx] = c;
        idx++;
        if (idx > CMAX) {
            fprintf(stderr,"command too long\n");
            return true;
        }
    }

    // line[idx] = '\0';

    /*Check if there are any commands*/
    if (strlen(line) <= 1) {
        fprintf(stderr, "No command entered\n");
        return true;
    }

    /*Parse commands by pipeline*/
    tokens = splitStr(line, '|');
    while(*(tokens + len)) {
        len ++;
        (*stage_len)++;
    }
    if(len > PMAX) {
        fprintf(stderr, "pipeline too deep\n");
        return true;
    }

    /*exit if there aren't any commands*/
    if(!tokens) {
        fprintf(stderr, "No arguments outputed\n");
        return true;
    }

    /*loop through stages*/
    i = 0;
    while(*(tokens + i)) {
        if(getStages(*(tokens + i), i, tokens, stages)) {
            // i = 0;
            // while (*(tokens + i)) {
            //     free(*(tokens + i));
            //     i++;
            // }
            // free(tokens);
            return true;
        }
        /*free(*(tokens + i));*/
        i++;
    }

    i = 0;
    while (*(tokens + i)) {
        free(*(tokens + i));
        i++;
    }
    free(tokens);

    return false;
}


void executeC(struct Stage **stage_list, int stage_len) {

    int status = 0;
    char *argv[CMAX];
    char temp[CMAX] = {0};
    // pid_t pid;

    struct Stage *stages = stage_list[0];

    // printf("before\n");
    if(strcmp(stages -> input, "stdin") ||
        strcmp(stages -> output, "stdout")) {
            exec_redir(stage_list, stage_len);
            free(stages);
            exit(0);
            return;
    }
    strcpy(temp, stages -> argv);

    parseSomething(temp, argv);

    // pid = fork();

    if(pid == -1) {
        fprintf(stderr, "Something's wrong with fork()\n");
        return;
    }
    /*child process*/
/*    else if (pid == 0) {
        printf("argv[0]: %s\n", argv[0]);
        execvp(argv[0], argv);
        fflush(stdout);
        exit(EXIT_SUCCESS);
        printf("... didn't exit");
*/
    if(pid == 0) {
        /*check if it is cd*/
        if (strcmp(argv[0], "cd") == 0
            && stages->argc == 2) {
            chdir(argv[1]);
            free(stages);
        }
        else if(execvp(argv[0], argv) < 0) {
            fprintf(stderr, "%s: No such file or directory\n",
                argv[0]);
            free(stages);
            exit(1);
        }
        exit(0);
    }
    else {
        wait(&status);
    }

}

void ctrlHandler(int sig)
{
    signal(sig, SIG_IGN);
    fflush(stdout);
    if(pid == 0) {
        printf("\n");
        exit(0);
    }
    signal(SIGINT, ctrlHandler);

}

int main (int argc, char *argv[])
{
    struct Stage *stage_list[CMAX] = {NULL};
    int stage_len = 0;
    int status = 0;


    parentPID = getpid();

    signal(SIGINT, ctrlHandler);

    zeroLine();

    while(1) {
        parentPID = getpid();
        pid = fork();
        if (pid == 0) {
            if(getLine(stage_list, &stage_len)) {
                exit(0);
            }
            executeC(stage_list, stage_len);
        }
        else{
            wait(&status);
            // printf("status: %d\n", WEXITSTATUS(status));
            if (WEXITSTATUS(status) == 3)
                exit(0);
        }

        // executeC(stage_list, stage_len);
    }

    return 0;
}
