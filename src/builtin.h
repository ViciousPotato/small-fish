#ifndef __BUILTIN_H_
#define __BUILTIN_H_

typedef void (*BuiltinFunc) (char *argv[]);

typedef struct builtin_cmd {
  const char *name;
  BuiltinFunc func;
  const char *help;
} BuiltinCmd;

void builtin_cd(char *argv[]);
void builtin_suicide(char *argv[]);
void builtin_tell(char *argv[]);
void builtin_roll(char *argv[]);
void builtin_time(char *argv[]);
void builtin_help(char *argv[]);

#endif

