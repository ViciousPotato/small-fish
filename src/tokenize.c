#include "tokenize.h"

// Tokenize command line to separate tokens by whitespace.
int Tokenize(char *cmd, char ***tokens) {
  char **cmdtokens = (char **)malloc(sizeof(char *));
  char *token = strtok(cmd, " ");
  int i = 0;

  cmdtokens[i] = (char *)malloc(strlen(token)+1);
  strcpy(*cmdtokens, token);

  i++;
  while (token) {
    token = strtok(NULL, " ");
    if (token) {
      cmdtokens = (char **)realloc(cmdtokens, sizeof(char *) * i + 1);
      cmdtokens[i] = (char *)malloc(strlen(token)+1);
      strcpy(cmdtokens[i], token);
      i++;
    }
  }

  // Make it easier to be passed as argv.
  cmdtokens = (char **)realloc(cmdtokens, sizeof(char **) * i + 1);
  cmdtokens[i] = NULL;

  *tokens = cmdtokens;

  return i;
}