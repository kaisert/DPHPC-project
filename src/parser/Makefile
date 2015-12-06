CC = g++

CFLAGS = -std=c++11 -Wall -c -g 

all: parser.o lexer.o map.o token_list.o tokenstream.o

parser.o:
	$(CC) $(CFLAGS) parser.cpp

lexer.o:
	$(CC) $(CFLAGS) lexer.cpp

map.o:
	$(CC) $(CFLAGS) map.cpp

token_list.o:
	$(CC) $(CFLAGS) token_list.cpp

tokenstream.o:
	$(CC) $(CFLAGS) tokenstream.cpp
	
clean:
	$(RM) *.o

