#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "../src/tokenize.h"

int TestTokenize() {
  printf("Testing tokenize..\n");

  char cmd1[] = {"ls"};
  char **cmdTokens;
  int tokencnt;
  tokencnt = Tokenize(cmd1, &cmdTokens);
  assert(strcmp(cmdTokens[0], "ls") == 0);
  assert(cmdTokens[1] == NULL);
  assert(tokencnt == 1);

  char cmd2[] = {"ls -l | wc -l"};
  tokencnt = Tokenize(cmd2, &cmdTokens);
  assert(strcmp(cmdTokens[0], "ls") == 0);
  assert(strcmp(cmdTokens[1], "-l") == 0);
  assert(strcmp(cmdTokens[2], "|") == 0);
  assert(strcmp(cmdTokens[3], "wc") == 0);
  assert(strcmp(cmdTokens[4], "-l") == 0);
  assert(cmdTokens[5] == NULL);
  assert(tokencnt == 5);

  printf("Tokenize testings succeeds.\n");
  return 0;
}

int main() {
  TestTokenize();
  return 0;
}