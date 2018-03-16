#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "parseline.h"
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

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
    int in = 0;
    int saved_in = 0;
    char buf = 0;

    saved_in = dup(STDIN_FILENO);

    for(i = 0; i < stage_len; i++) {

        if(fp != NULL) {

            while((buf = fgetc(fp)) != EOF) {
                write(STDIN_FILENO, &buf, 1);
            }
        }
        // TODO print out input to stdin if input is a file
        fp = popen(stages[i] -> argv, "r");
        //TODO do something with the output if it's not piped
        // if it's not piped, set fp = NULL
    }

}

void zeroLine()
{
    int i;

    for (i = 0; i < CMAX; i++) {
        line[i] = '\0';
    }
}

void getUserInput()
{
    int idx = 0;
    char c;
    printf("8-p ");

    /*get command while checking if input
    exceeds command line length max (CMAX)*/
    while((c = getchar()) != '\n') {
     if (c == EOF)
         exit(0);
     if (isSig) {
        zeroLine(line);
        printf("\n8-p ");
        isSig = false;
     }
     line[idx] = c;
     idx++;
     if (idx > CMAX) {
         fprintf(stderr,"command too long\n");
         return;
     }
    }
    line[idx] = '\0';
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
            exit(0);
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
        stage_len++;
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
            i = 0;
            while (*(tokens + i)) {
                free(*(tokens + i));
                i++;
            }
            free(tokens);
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
    pid_t pid;

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
    /*child process*/
/*    else if (pid == 0) {
        printf("argv[0]: %s\n", argv[0]);
        execvp(argv[0], argv);
        fflush(stdout);
        exit(EXIT_SUCCESS);
        printf("... didn't exit");
*/
    if(pid == 0) {
        if(execvp(argv[0], argv) < 0) {
            fprintf(stderr, "%s: No such file or directory\n",
                argv[0]);
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
    char c;
    pid_t selfPID = getpid();
    signal(sig, SIG_IGN);
    // printf("FUCKING DAMN, why the fuck did you hit CTRL-C?\n");
    fflush(stdout);
    if(parentPID != selfPID)
        exit(0);
    /*zeroLine();
    printf("\n8-p ");*/
    signal(SIGINT, ctrlHandler);
    // isSig = true;
    // if(pid == 0)
    //     exit(0);
    // else {
    //     // c = getchar();
    //     // zeroLine();
    //     // printf("\n8-p ");
    //     signal(SIGQUIT, SIG_IGN);
    //     kill(-parent_pid, SIGQUIT);
    //     signal(SIGINT, ctrlHandler);
    // }

    // sigaction(SIGINT, &siga, NULL);
    // fflush(stdout);
    /*if (pid == 0) {
        exit(0);
    }*/
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
    // struct Stage **stages = NULL;
    // int len = 0;
    // int i;
    // isSig = false;
    /*memset(&siga, 0, sizeof(siga));

    siga.sa_handler = &ctrlHandler;*/

    // sigaction (SIGINT, &siga, NULL);
    // signal(SIGINT, ctrlHandler);
    // while(1) {
        // getUserInput();
        // getLine(line);
        // stages = get_stages();
        // len = get_num_stages();
        // printf("stages argv: %s\n", (*stages)->argv);
        // executeC(*stages);
    struct Stage *stage_list[CMAX] = {NULL};
    int stage_len = 0;

    parentPID = getpid();

    signal(SIGINT, ctrlHandler);

    zeroLine();


    while(1) {
        parentPID = getpid();
        if(getLine(stage_list, &stage_len)) {
            continue;
        }
        executeC(stage_list, stage_len);
    }

	return 0;
}
