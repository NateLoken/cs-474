# Nate Loken
# CS-474 Operating Systems
# The pupose of this make file is to compile the c file into an executable program named "program2"
# The secondary purpose is to remove the executable by running the clean command for easy cleanup after testing

# In order to create the 'program2' executable, first the execute script must be compiled 
# then the shell program, then using both compiled files the Makefile creates the executable

all: program2

program2: shell.o execute.o header.h
	gcc -o program2 shell.o execute.o

shell.o: shell.c header.h
	gcc -c shell.c

execute.o: execute.c header.h
	gcc -c execute.c

clean:
	rm program2 *.o
