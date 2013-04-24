CC = g++
OPS = -ansi -Wall -Werror -pedantic

test: test.o
	$(CC) $(OPS) test.o ../src/tokenize.o -o test

test.o: test.c
	$(CC) $(OPS) -c test.c