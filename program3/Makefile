# Nate Loken
# CS-474 Operating Systems
# The purpose of this file is create the executables for normal, shared, and thread
# where normal is the normal implementation of mergesort
# shared is the implemetation that uses shared memory
# and thread is the implemtation that uses multiple threads

all: normal shared thread

normal:
	gcc merge.c -o normal

shared:
	gcc shared.c -o shared

thread:
	gcc thread.c -lpthread -o thread

clean:
	rm normal shared thread