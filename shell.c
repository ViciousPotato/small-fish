#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "builtin.h"

extern BuiltinCmd builtin_commands[];

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

int executeCommand(char **cmd, int cmdsize, int mstdin, int mstdout, int mstderr, int pipefd[]) {
  if (cmdsize <= 0) return -1;
  
  // Check if is builtin command.
  int i;
  for (i = 0; builtin_commands[i].name; i++) {
    if (strcmp(builtin_commands[i].name, *cmd) == 0) {
      builtin_commands[i].func(cmd+1);
      return -1;
    }
  }

  for (i = cmdsize-1; i >= 0; i--) {
    char *t = cmd[i];
    if (strcmp(t, "|") == 0) {
      int fd[2];
      pipe(fd);
      cmd[i] = NULL;
      executeCommand(cmd, i, mstdin, fd[1], mstderr, fd);
      executeCommand(cmd+i+1, cmdsize-i-1, fd[0], mstdout, mstderr, fd);
      close(fd[0]); close(fd[1]);
      return -1;
    } else if (strcmp(t, ">") == 0) {
      if (cmd[i+1] == NULL) {
        perror("Wrong syntax: > should be followed by a file name.");
        return -1;
      }
      int out = open(cmd[i+1], O_RDWR | O_CREAT, 0644);
      cmd[i] = NULL;
      executeCommand(cmd, cmdsize-2, mstdin, out, mstderr, NULL);
      close(out);
      return -1;
    } else if (strcmp(t, ">>") == 0) {
      if (cmd[i+1] == NULL) {
        perror("Wrong syntax: >> should be followed by a file name.");
        return -1;
      }
      int append = open(cmd[i+1], O_RDWR | O_CREAT | O_APPEND, 0644);
      if (append == -1) {
        perror("Target file doesn't exist");
        return -1;
      }
      cmd[i] = NULL;
      executeCommand(cmd, cmdsize-2, mstdin, append, mstderr, NULL);
      close(append);
    } else if (strcmp(t, "<") == 0) {
      if (cmd[i+1] == NULL) {
        perror("Wrong syntax: < should be followed by a file name.");
        return -1;
      }
      int inp = open(cmd[i+1], O_RDONLY);
      if (inp == -1) {
        perror("Input file doesn't exit");
        return -1;
      }
      cmd[i] = NULL;
      executeCommand(cmd, cmdsize-2, inp, mstdout, mstderr, NULL);
      close(inp);
    }
  }
  
  // Or we simply execute the program
  char *command = cmd[0];

  int pid = fork();
  if (pid == 0) {
    // Child process
    if (mstdout != STDOUT_FILENO) {
      dup2(mstdout, STDOUT_FILENO);
    }
    if (mstdin != STDIN_FILENO) {
      dup2(mstdin, STDIN_FILENO);
    }
    if (mstderr != STDERR_FILENO) {
      dup2(mstderr, STDERR_FILENO);
      close(mstderr);
    }
    
    if (pipefd) {
      close(pipefd[0]); close(pipefd[1]);
    }

    execvp(command, cmd);
    perror("Can't execute command");
  }

  return pid;
}

int main(int argc, char *argv[]) {
  char *cmd = NULL;
  // Splitted tokens.
  char **cmds = NULL;

  while (1) {
    // First do some cleanups.
    if (cmd) {
      free(cmd);
      cmd = NULL;
    }

    if (cmds) {
      char **f = cmds;
      while (*f) {
        free(*f);
        *f = NULL;
        f++;
      }
      cmds = NULL;
    }

    // Print prompt
    char *cwd = getcwd(NULL, 0);
    char *prompt = (char *)malloc(strlen(cwd)+3); // 'dir> \0' 
    snprintf(prompt, strlen(cwd)+3, "%s> ", cwd);
    cmd = readline(prompt);
    free(cwd);
    free(prompt);

    // Skip empty command
    if (!cmd || !*cmd) continue;
    
    // Add to readline history
    add_history(cmd);

    // Execute command.
    int tokencnt = splitCmd(cmd, &cmds);
    executeCommand(cmds, tokencnt, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO, NULL);

    // Wait for all child to finish.
    pid_t pid = 1;
    int childstatus;
    while (pid > 0) {
      pid = wait(&childstatus);
    }
  }

  return 0;
}
