#ifndef __BUILTIN_H_
#define __BUILTIN_H_

typedef void (*BuiltinFunc) (char *argv[]);

typedef struct builtin_cmd {
  const char *name;
  BuiltinFunc func;
} BuiltinCmd;

void builtin_cd(char *argv[]);

#endif