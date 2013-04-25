#include "eval.h"

int ExecuteCommand(char **cmdTokens, int stdfds[3], int pipfd[2]) {
  char *command = cmdTokens[0];
  int fstdin  = stdfds[0];  
  int fstdout = stdfds[1];  
  int fstderr = stdfds[2];

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

int EvalCommand(ASTree *ast) {
  if (ast == NULL) return;

  // Node type
  switch(ast->type) {
    case PrimCommand:
      ExecuteCommand();
      break;
    case PipeCommand:
      EvalCommand(ast->u.leftCommand);
      EvalCommand(ast->u.rightCommand);
      break;
    default:
      break;
  }

  return 0;
}