all: shell

shell: shell.c shell.h pipe.h
	gcc -o shell shell.c
