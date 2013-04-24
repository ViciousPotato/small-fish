#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "builtin.h"

BuiltinCmd builtin_commands[] = {
  {"cd",      builtin_cd,      "Good old cd."},
  {"swim",    builtin_cd,      "If fish doesn't like swimming, why do they swim all day?"},
  {"suicide", builtin_suicide, "I know you are cruel."},
  {"tell",    builtin_tell,    "I won't inform the ignorant."},
  {"roll",    builtin_roll,    "Try your luck."},
  {"time",    builtin_time,    "Wise man doesn't need time."},
  {"help",    builtin_help,    "Beg me."},
  {NULL, NULL}
};

void builtin_cd(char *argv[]) {
  if (argv[0])
    chdir(argv[0]);
}

void builtin_suicide(char *argv[]) {
  printf("Poor little fish killed himself.\n");
  exit(-1);
}

void builtin_tell(char *argv[]) {
  printf("Grow up boy, do it your self.\n");
}

void builtin_roll(char *argv[]) {
  srand(time(NULL));
  int random = (rand() % 100) + 1;
  if (random < 60) {
    printf("You got %d loser.\n", random);
  } else {
    printf("Got %d, looks nice.\n", random); 
  }
}

void builtin_time(char *argv[]) {
  printf("Look at the clock.\n");
}

void builtin_help(char *argv[]) {
  printf("Since you are begging me so sincerely, I will tell you some of my secrets.\n\n");

  int i;
  for (i = 0; builtin_commands[i].name; i++) {
    printf("%s\t%s\n", builtin_commands[i].name, builtin_commands[i].help);
  }
}
