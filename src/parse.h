typedef enum {
  PipeCommand, // |
  OutRedirCommand, // >
  OutAppendCommand, // >>
  InpRedirCommand, // <
  PrimCommand
} CommandType;

typedef struct _PipeCommandNode {
  char **leftCmdTokens;
  char **rightCmdTokens;
} PipeCommandNode;

typedef struct _PrimCommandNode {
  char **cmdTokens;
} PrimCommandNode;

typedef struct _OutRedirCommandNode {
  char **cmdTokens;
  char *outFile;
} OutRedirCommandNode;

typedef struct _OutAppendCommandNode {
  char **cmdTokens;
  char *appendFile;
} OutAppendCommandNode;

typedef struct _InpRedirCommandNode {
  char **cmdTokens;
  char *inpFile;
} InpRedirCommandNode;

typedef struct _ASTNode {
  CommandType type;
  char *cmd;
  union {
    PipeCommandNode pipe;
    PrimCommandNode prim;
    OutRedirCommandNode out;
    OutAppendCommandNode app;
    InpRedirCommandNode inp;
  } u;
  int stdfds[3];
  int pipefds[2];
} ASTNode;

typedef ASTNode *ASTTree;

ASTTree ParseCommand(char **cmdTokens);