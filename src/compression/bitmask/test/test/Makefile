CC = g++
FLAGS = -std=c++11 -Wall -g 

uncompressed:
	make -C ../../../../parser clean
	make -C ../../../../parser/
	$(CC) $(FLAGS) -o uncompressed.bin ../../../../chunker/bufsplit.cpp ../../../../parser/*.o -I ../../../../chunker -I ../../../../parser uncompressed.cpp
compression1:
	make -C ../../../../parser clean
	make -C ../../../../parser/
	$(CC) $(FLAGS) -o compression1.bin ../../../../chunker/bufsplit.cpp ../../../../parser/*.o -I ../../../../chunker -I ../../../../parser ../../../SurpressClosingDeflator.cpp  compression1.cpp
compression2:
	make -C ../../../../parser clean
	make -C ../../../../parser/
	$(CC) $(FLAGS) -o compression2.bin ../../../../chunker/bufsplit.cpp ../../../../parser/*.o -I ../../../../chunker -I ../../../../parser ../../../SurpressClosingVSizeDeflator.cpp  compression2.cpp
clean:
	$(RM) *.bin
	
