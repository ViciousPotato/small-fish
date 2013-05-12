CC = g++
OPS = -ansi -Wall -Werror -pedantic -g

shell: shell.o builtin.o tokenize.o parse.o eval.o
	$(CC) $(OPS) shell.o builtin.o tokenize.o parse.o eval.o -lreadline -ltermcap -o ../fish

builtin.o: builtin.c
	$(CC) $(OPS) -c builtin.c

shell.o: shell.c
	$(CC) $(OPS) -c shell.c

tokenize.o: tokenize.c
	$(CC) $(OPS) -c tokenize.c

parse.o: parse.c
	$(CC) $(OPS) -c parse.c

eval.o: eval.c
	$(CC) $(OPS) -c eval.c
