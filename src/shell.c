#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "tokenize.h"
#include "parse.h"
#include "eval.h"

char *Readline() {
  char *cwd = getcwd(NULL, 0);
  char *prompt = (char *)malloc(strlen(cwd)+3); // 'dir> \0' 
  snprintf(prompt, strlen(cwd)+3, "%s> ", cwd);
  char *cmd = readline(prompt);
  free(cwd);
  free(prompt);
  return cmd;
}

void WaitChildren() {
  // Wait for all child to finish.
  pid_t pid = 1;
  int childstatus;
  while (pid > 0) {
    pid = wait(&childstatus);
  }
}

void FreeCmd(char *cmd) {
  if (cmd == NULL) return;
  free(cmd);
}

void FreeCmdTokens(char **cmdTokens) {
  if (cmdTokens == NULL) return;
  if (cmdTokens) {
    char **f = cmdTokens;
    while (*f) {
      free(*f);
      *f = NULL;
      f++;
    }
  }
}

int main(int argc, char *argv[]) {
  char *cmd = NULL;
  char **cmdTokens = NULL;

  while (1) {
    cmd = Readline();
    if (!cmd || !*cmd) continue;
    
    // Add to readline history
    add_history(cmd);
    
    // Parse and execute
    int tokencnt = Tokenize(cmd, &cmdTokens);
    ASTTree astTree = ParseCommand(cmdTokens, tokencnt);
    EvalCommand(astTree);
    WaitChildren();
    
    FreeCmd(cmd);
    FreeCmdTokens(cmdTokens);
  }

  return 0;
}
