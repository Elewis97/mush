#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include "parseline.h"

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

void executeC()
{
	/*checks if redirect is necessary*/
	/*executes as directed*/
}


int main (int argc, char *argv[])
{
	pid_t pid;
    struct Stage **stages;
    int len = 0;
    // char *const parmList[] = {"ls", NULL};
    //
    // execv("/bin/ls", parmList);
    getLine();
    stages = get_stages();
    len = get_num_stages();

    printf("%d\n", len);
    printf("%s\n", stages[0] -> output);

	return 0;
}
