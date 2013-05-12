#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "eval.h"

// Execute Prim Command
int ExecuteCommand(ASTNode *cmd) {
  assert(cmd->type == PrimCommand);

  char **cmdTokens = cmd->u.prim.cmdTokens;
  char *command = cmdTokens[0];
  int fstdin    = cmd->stdfds[0];
  int fstdout   = cmd->stdfds[1];
  int fstderr   = cmd->stdfds[2];
  int *pipefd   = cmd->pipefds;

  // printf("Executing command:%s with stdin=%d, stdout=%d, stderr=%d, fd[0]=%d, fd[2]=%d\n",
  //  command, cmd->stdfds[0], cmd->stdfds[1], cmd->stdfds[2], cmd->pipefds[0], cmd->pipefds[1]);

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
  dst->stdfds[0] = src->stdfds[0];
  dst->stdfds[1] = src->stdfds[1];
  dst->stdfds[2] = src->stdfds[2];
  dst->pipefds[0] = src->pipefds[0];
  dst->pipefds[1] = src->pipefds[1];
}

int EvalCommand(ASTTree ast) {
  if (ast == NULL) return -1;

  // Node type
  switch(ast->type) {
    case PrimCommand: {
      ExecuteCommand(ast);
      break;
    }

    case PipeCommand: {
      int fd[2];
      pipe(fd);
      // Put the right file desciptors.
      CopyFds(ast->u.pipe.leftCommand, ast);
      CopyFds(ast->u.pipe.rightCommand, ast);
      // left write to pipe, right cmd read from pipe
      ast->u.pipe.leftCommand->stdfds[1] = fd[1];
      ast->u.pipe.rightCommand->stdfds[0] = fd[0];

      ast->u.pipe.leftCommand->pipefds[0] = fd[0];
      ast->u.pipe.leftCommand->pipefds[1] = fd[1];
      
      ast->u.pipe.rightCommand->pipefds[0] = fd[0];
      ast->u.pipe.rightCommand->pipefds[1] = fd[1];

      EvalCommand(ast->u.pipe.leftCommand);
      EvalCommand(ast->u.pipe.rightCommand);

      close(fd[0]); close(fd[1]);
      break;
    }

    case OutRedirCommand: {
      int out = open(ast->u.out.outFile, O_RDWR | O_CREAT, 0644);
      CopyFds(ast->u.out.command, ast);
      ast->u.out.command->stdfds[1] = out;
      ExecuteCommand(ast->u.out.command);
      close(out);
      break;
    }

    case InpRedirCommand: {
      int inp = open(ast->u.inp.inpFile, O_RDONLY);
      CopyFds(ast->u.inp.command, ast);
      ast->u.inp.command->stdfds[0] = inp;
      ExecuteCommand(ast->u.inp.command);
      close(inp);
      break;
    }

    case OutAppendCommand: {
      int append = open(ast->u.app.appendFile, O_RDWR | O_CREAT | O_APPEND, 0644);
      CopyFds(ast->u.app.command, ast);
      ast->u.app.command->stdfds[1] = append;
      ExecuteCommand(ast->u.app.command);
      close(append);
      break;
    }

    default:
      break;
  }

  return 0;
}
