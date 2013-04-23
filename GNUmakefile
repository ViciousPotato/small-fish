CC = g++
OPS = -ansi -Wall -Werror -pedantic

shell: shell.o builtin.o
	$(CC) $(OPS) shell.o builtin.o -lreadline -ltermcap -o shell

builtin.o: builtin.c
	$(CC) $(OPS) -c builtin.c

shell.o: shell.c
	$(CC) $(OPS) -c shell.c