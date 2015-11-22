CC = gcc

CFLAGS = -std=c99 -Wall -c -g 

all: parser.o lexer.o map.o token_list.o tokenstream.o

parser.o:
	$(CC) $(CFLAGS) parser.c

lexer.o:
	$(CC) $(CFLAGS) lexer.c

map.o:
	$(CC) $(CFLAGS) map.c

token_list.o:
	$(CC) $(CFLAGS) token_list.c

tokenstream.o:
	$(CC) $(CFLAGS) tokenstream.c
	
clean:
	$(RM) *.o

