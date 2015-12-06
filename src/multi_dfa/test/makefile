CC = gcc
FLAGS = -Wall -g -std=c99 -fopenmp 

mediator_test:
	make -C ../../parser clean
	make -C ../../parser/
	$(CC) $(FLAGS) -o mediator_test ../mediator.c ../../chunker/chunker.c ../../parser/*.o mediator_test.c

clean:
	$(RM) mediator_test
