#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include "parseline.h"
#include <sys/wait.h>
#include <signal.h>

pid_t pid;
struct sigaction siga;
bool isSig;
char line[CMAX];

/*Things we need to do:
 - Make sure it doesn't exit at ctrl c*/

/*cat*/

/*NOTE: I found these things on the internet,
They are not to be implemented into the code, but
simply used as a refrence for how exec and fork
are to be used*/

/*sample redirect exec to send output to a file*/
/*if (fork() == 0)
{
    // child
    int fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    dup2(fd, 1);   // make stdout go to file
    dup2(fd, 2);   // make stderr go to file - you may choose to not do this
                   // or perhaps send stderr to another file

    close(fd);     // fd no longer needed - the dup'ed handles are sufficient

    exec(...);
}*/

/*sample send output to a pipe to read output into buffer*/
/*int pipefd[2];
pipe(pipefd);

if (fork() == 0)
{
    close(pipefd[0]);    // close reading end in the child

    dup2(pipefd[1], 1);  // send stdout to the pipe
    dup2(pipefd[1], 2);  // send stderr to the pipe

    close(pipefd[1]);    // this descriptor is no longer needed

    exec(...);
}
else
{
    // parent

    char buffer[1024];

    close(pipefd[1]);  // close the write end of the pipe in the parent

    while (read(pipefd[0], buffer, sizeof(buffer)) != 0)
    {
    }
}*/

void parseSomething(char *command, char **paramBuffer)
{
    int i;

    for (i = 0; i < PMAX; i++) {
        paramBuffer[i] = strsep(&command, " ");
        if(paramBuffer[i] == NULL)
            break;
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

void executeC(struct Stage *stages)
{
    char *argv[CMAX];
    int childStat;

    pid = fork();
    parseSomething(stages->argv, argv);
    
    /*is command valid?*/

    /*execute*/

    /*error*/
    if (pid == -1) {
        /*do something else?*/
        fprintf(stderr, "pid went wrong\n");
    }
    /*child process*/
    else if (pid == 0) {
        printf("argv[0]: %s\n", argv[0]);
        execvp(argv[0], argv);
        fflush(stdout);
        exit(EXIT_SUCCESS);
        printf("... didn't exit");

        /*check for error*/
    }

    /*parent process*/
    else {
        waitpid(pid, &childStat, 0);
    }

    /*... I have no idea if this works*/
}

void ctrlHandler(int sig)
{
    char c;
    signal(sig, SIG_IGN);
    printf("FUCKING DAMN, why the fuck did you hit CTRL-C?\n");
    fflush(stdout);
    // isSig = true;
    if(pid == 0)
        exit(0);
    else
        signal(SIGINT, ctrlHandler);
    // sigaction(SIGINT, &siga, NULL);
    // fflush(stdout);
    /*if (pid == 0) {
        exit(0);
    }*/
}


int main (int argc, char *argv[])
{
    struct Stage **stages = NULL;
    int len = 0;
    int i;
    isSig = false;

    zeroLine();

    /*memset(&siga, 0, sizeof(siga));

    siga.sa_handler = &ctrlHandler;*/

    // sigaction (SIGINT, &siga, NULL);
    signal(SIGINT, ctrlHandler);
    while(1) {
        getUserInput();
        getLine(line);
        stages = get_stages();
        len = get_num_stages();
        printf("stages argv: %s\n", (*stages)->argv);
        executeC(*stages);
    }

	return 0;
}
