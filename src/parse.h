#ifndef __PARSE_H__
#define __PARSE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum {
  PipeCommand, // |
  OutRedirCommand, // >
  OutAppendCommand, // >>
  InpRedirCommand, // <
  PrimCommand
} CommandType;

struct _ASTNode;

typedef struct _ASTNode ASTNode;

typedef struct _PipeCommandNode {
  ASTNode *leftCommand;
  ASTNode *rightCommand;
} PipeCommandNode;

typedef struct _PrimCommandNode {
  char **cmdTokens;
} PrimCommandNode;

// TODO: join these 3 types of commands.
typedef struct _OutRedirCommandNode {
  ASTNode *command;
  char *outFile;
} OutRedirCommandNode;

typedef struct _OutAppendCommandNode {
  ASTNode *command;
  char *appendFile;
} OutAppendCommandNode;

typedef struct _InpRedirCommandNode {
  ASTNode *command;
  char *inpFile;
} InpRedirCommandNode;

struct _ASTNode {
  CommandType type;
  union {
    PipeCommandNode pipe;
    PrimCommandNode prim;
    OutRedirCommandNode out;
    OutAppendCommandNode app;
    InpRedirCommandNode inp;
  } u;
  int stdfds[3];
  int pipefds[2];
};

typedef ASTNode *ASTTree;

ASTTree ParseCommand(char **cmdTokens, int tokenCnt);
void FreeASTTree(ASTTree *ast);

#endif