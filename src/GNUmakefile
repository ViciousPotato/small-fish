CC = g++
OPS = -ansi -Wall -Werror -pedantic

shell: shell.o builtin.o tokenize.o
	$(CC) $(OPS) shell.o builtin.o tokenize.o -lreadline -ltermcap -o ../fish

builtin.o: builtin.c
	$(CC) $(OPS) -c builtin.c

shell.o: shell.c
	$(CC) $(OPS) -c shell.c

tokenize.o: tokenize.c
	$(CC) $(OPS) -c tokenize.c