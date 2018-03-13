
#ifndef PARSELINE_H
#define PARSELINE_H
#define CMAX 512 /*Command line length max*/
#define PMAX 10  /*Pipeline command max*/


struct Stage {
	char input[CMAX];
	char output[CMAX];
	int argc;
	char argv[CMAX];
};

struct Stage *stage = NULL;
struct Stage *stages[PMAX] = {NULL};

struct Stage **get_stages();
void initStage();
char** splitStr(char path[], const char delimiter);
void freeRemainingTokens(char **tokens, int i);
void displayStage(struct Stage *stage);
int get_redir(char **args_v, char *redir);
int check_double_redirect(char arg[], char **args_v, char *sym);
void get_prev_cmd(char **tokens, int tokIdx, char buffer[]);
void get_next_cmd(char **tokens, int tokIdx, char buffer[]);
int fillCommand(char arg[], char **tokens, int tokIdx, int len);
bool getCommand(char arg[], char** tokens, int tokIdx);
bool getStages(char arg[], int stageNum, char** tokens);
bool getLine();

#endif

