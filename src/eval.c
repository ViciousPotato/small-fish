#include "eval.h"
#include <assert.h>

// Execute Prim Command
int ExecuteCommand(ASTNode *cmd) {
  assert(cmd->type == PrimCommand);

  char **cmdTokens = cmd->u.prim.cmdTokens;
  char *command = cmdTokens[0];
  int fstdin  = cmd->stdfds[0];
  int fstdin  = cmd->stdfds[1];
  int fstdin  = cmd->stdfds[2];
  int *pipefd = cmd->pipefds;

  int pid = fork();
  if (pid == 0) {
    // Child process
    if (fstdout != STDOUT_FILENO) {
      dup2(fstdout, STDOUT_FILENO);
    }
    if (fstdin != STDIN_FILENO) {
      dup2(fstdin, STDIN_FILENO);
    }
    if (fstderr != STDERR_FILENO) {
      dup2(fstderr, STDERR_FILENO);
      close(fstderr);
    }
    
    if (pipefd[0] > -1) {
      close(pipefd[0]); close(pipefd[1]);
    }

    execvp(command, cmdTokens);
    perror("Can't execute command");
  }

  return pid;
}

// TODO: I think the name CopyStds maybe more appropriate.
void CopyFds(ASTNode *dst, ASTNode *src) {
  if (dst == NULL || src == NULL) return;
  dst->stdfds[0] = src->stdfdsp[0];
  dst->stdfds[1] = src->stdfdsp[1];
  dst->stdfds[2] = src->stdfdsp[2];
}

int EvalCommand(ASTree *ast) {
  if (ast == NULL) return;

  // Node type
  switch(ast->type) {
    case PrimCommand:
      ExecuteCommand(ast);
      break;
    case PipeCommand:
      int fd[2];
      pipe(fd);
      // Put the right file desciptors.
      CopyFds(ast->u.leftCommand, ast);
      CopyFds(ast->u.rightCommand, ast);
      // left write to pipe, right cmd read from pipe
      ast->u.leftCommand->stdfds[1] = fd[1];
      ast->u.rightCommand->stdfds[0] = fd[0];
      ast->u.leftCommand->pipefds[0] = fd[0];
      ast->u.rightCommand->pipefds[1] = fd[1];

      EvalCommand(ast->u.leftCommand);
      EvalCommand(ast->u.rightCommand);

      close(fd[0]); close(fd[1]);
      break;
    case OutRedirCommand:
      int out = open(ast->u.out->outFile, O_RDWR | O_CREAT, 0644);
      CopyFds(ast->u.out->command, ast);
      ast->u.out->command.stdfds[1] = out;
      ExecuteCommand(ast->u.out->command);
      close(out);
      break;
    default:
      break;
  }

  return 0;
}
