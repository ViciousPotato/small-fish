#include <unistd.h>

#include "builtin.h"

BuiltinCmd builtin_commands[] = {
  {"cd", builtin_cd},
  {NULL, NULL}
};

void builtin_cd(char *argv[]) {
  if (argv[0])
    chdir(argv[0]);
}