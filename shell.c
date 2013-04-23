#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define PROMPT "small-shell> "
#define CMDSIZE 256

// Split cmd into char arrays.
int splitCmd(char *cmd, char ***cmds) {
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

  *cmds = cmdtokens;
  return i;
}

int execute_command(char **cmd, int cmdsize, int mstdin, int mstdout, int mstderr, int *pipefd) {
  if (cmdsize <= 0) return;
  
  printf("execute_command called: cmd=[");
  int j;
  for (j = 0; j < cmdsize; j++) {
    printf("%s,", cmd[j]);
  }
  printf("]");
  printf("cmdsize=%d, stdin=%d, stdout=%d, stderr=%d\n", cmdsize, mstdin, mstdout, mstderr);

  int i;
  for (i = cmdsize-1; i >= 0; i--) {
    char *t = cmd[i];
    if (strcmp(t, "|") == 0) {
      int fd[2];
      pipe(fd);
      cmd[i] = NULL;
      int pidLeft  = execute_command(cmd, i, mstdin, fd[1], mstderr, fd);
      int pidRight = execute_command(cmd+i+1, cmdsize-i-1, fd[0], mstdout, mstderr, fd);
      close(fd[0]); close(fd[1]);
      // Left or right could be other pipes, doesn't need to wait.
      if (pidLeft > 0) pidLeft = wait(NULL);
      if (pidRight > 0) pidRight = wait(NULL);
      return -1;
    } else if (strcmp(t, ">") == 0) {
      printf("> is unimplemeted\n");
    }
  }
  
  // Or we simply execute the program
  char *command = cmd[0];

  int pid = fork();
  if (pid == 0) {
    // Child process
    if (mstdout != STDOUT_FILENO) {
      dup2(mstdout, STDOUT_FILENO);
      close(mstdout);
      if (pipefd) close(pipefd[0]);
    }
    if (mstdin != STDIN_FILENO) {
      dup2(mstdin, STDIN_FILENO);
      close(mstdin);
      if (pipefd) close(pipefd[1]);
    }
    if (mstderr != STDERR_FILENO) {
      dup2(mstderr, STDERR_FILENO);
      close(mstderr);
    }
    
    execvp(command, cmd);
    perror("Can't execute command");
  }

  return pid;
}

int main(int argc, char *argv) {
  char cmd[CMDSIZE];
  while (1) {
    char *token;
    int childstat;
    int redirfd = 0;
    int foundredir = 0;

    write(STDOUT_FILENO, PROMPT, sizeof(PROMPT));
    // Read command
    int readcnt = read(STDIN_FILENO, cmd, sizeof(cmd));
    // Remove last \n
    cmd[readcnt-1] = '\0';

    char **cmds;
    int tokencnt = splitCmd(cmd, &cmds);
    int pid = execute_command(cmds, tokencnt, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO, NULL);
    if (pid >= 0) pid = wait(NULL);
    // Check for I/O redirections.
    //    for (i = 0; i < childargc; i++) {
    //if (foundredir) {
    //  redirfd = open(childargs[i], O_RDWR|O_CREAT);
    //  break;
    //}

    //if (strcmp(childargs[i], ">") == 0) {
        // We found a redirection
    //  childargs[i] = NULL;
    //  foundredir = 1;
    // }
    //}
    // Parent
  }
}
