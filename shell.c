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

void execute_command(char **cmd, int cmdsize, int stdin, int stdout, int stderr) {
  if (cmdsize <= 0) return;
  
  printf("execute_command called: cmd=[");
  int j;
  for (j = 0; j < cmdsize; j++) {
    printf("%s,", cmd[j]);
  }
  printf("]");
  printf("cmdsize=%d, stdin=%d, stdout=%d, stderr=%d\n", cmdsize, stdin, stdout, stderr);

  int i;
  for (i = 0; i < cmdsize; i++) {
    char *t = cmd[i];
    if (strcmp(t, "|") == 0) {
      int fd[2];
      printf("Piping\n");
      pipe(fd);
      cmd[i] = NULL;
      execute_command(cmd, i, stdin, fd[1], stderr);
      execute_command(cmd+i+1, cmdsize-i-1, fd[0], stdout, stderr);
      return;
    } else if (strcmp(t, ">") == 0) {
      printf("> is unimplemeted\n");
    }
  }
  
  // Or we simply execute the program
  char *command = cmd[0];

  int pid = fork();
  if (pid == 0) {
    // Child process
    if (stdout != STDOUT_FILENO) {
      dup2(stdout, 1);
      close(stdout);
    }
    if (stdin != STDIN_FILENO) {
      dup2(stdin, 0);
      close(stdin);
    }
    if (stderr != STDERR_FILENO) {
      dup2(stderr, 2);
      close(stderr);
    }
    
    /*
    int j = 0;
    char *arg = childargs[j];
    while (*arg) {
      printf("arg %d = %s\n", j, arg);
      j++;
      arg = childargs[j];
    }
    */
    execvp(command, cmd);
    perror("Can't execute command");
  }
  int childstat;
  pid = wait(&childstat);
  printf("Child finished with: pid:%d, stat:%d\n", pid, childstat);
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
    execute_command(cmds, tokencnt, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO);
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
