#include "parse.h"

// Create empty AST node
ASTNode *CreateASTNode() {
  ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
  if (node == NULL) {
    perror("Not enough memory. Exit.\n");
    exit(-1);
  }
  node->stdfds[0] = STDIN_FILENO;
  node->stdfds[1] = STDOUT_FILENO;
  node->stdfds[2] = STDERR_FILENO;
  node->pipefds[0] = -1;
  node->pipefds[1] = -1;
  return node;
}

void DestroyASTNode(ASTNode *node) {
  // TODO: recursively free
  free(node);
}

ASTTree ParseCommand(char **cmdTokens, int tokenCnt) {
  if (tokenCnt <= 0) return NULL;

  int i;
  for (i = tokenCnt-1; i >= 0; i--) {
    char *token = cmdTokens[i];

    if (strcmp(token, "|") == 0) {
      // Create a pipe node
      cmdTokens[i] = NULL;
      ASTNode *pipeNode = CreateASTNode();
      pipeNode->type = PipeCommand;
      pipeNode->u.pipe.leftCommand = ParseCommand(cmdTokens, i);
      pipeNode->u.pipe.rightCommand = ParseCommand(cmdTokens+i+1, tokenCnt-i-1);
      return pipeNode;
    } else if (strcmp(token, ">") == 0) {
      // If > appears, must be > file in the end. Nothing more.
      // don't support > sth < sth2
      if (cmdTokens[i+1] == NULL) {
        perror("Wrong syntax: > should be followed by a file name.");
        return NULL;
      }
      cmdTokens[i] = NULL;
      ASTNode *outNode = CreateASTNode();
      outNode->type = OutRedirCommand;
      outNode->u.out.command = ParseCommand(cmdTokens, tokenCnt-2);
      outNode->u.out.outFile = cmdTokens[i+1];
      return outNode;
    } else if (strcmp(token, ">>") == 0) {
      if (cmdTokens[i+1] == NULL) {
        perror("Wrong syntax: >> should be followed by a file name.");
        return NULL;
      }
      cmdTokens[i] = NULL;
      ASTNode *appNode = CreateASTNode();
      appNode->type = OutAppendCommand;
      appNode->u.app.command = ParseCommand(cmdTokens, tokenCnt-2);
      appNode->u.app.appendFile = cmdTokens[i+1];
      return appNode;
    } else if (strcmp(token, "<") == 0) {
      if (cmdTokens[i+1] == NULL) {
        perror("Wrong syntax: < should be followed by a file name.");
        return NULL;
      }
      cmdTokens[i] = NULL;
      ASTNode *inpNode = CreateASTNode();
      inpNode->type = InpRedirCommand;
      inpNode->u.inp.command = ParseCommand(cmdTokens, tokenCnt-2);
      inpNode->u.inp.inpFile = cmdTokens[i+1];
      return inpNode;
    }
  }
  // Prim Command
  ASTNode *primNode = CreateASTNode();
  primNode->type = PrimCommand;
  primNode->u.prim.cmdTokens = cmdTokens;
  return primNode;
}
