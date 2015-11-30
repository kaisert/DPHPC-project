CC = g++
FLAGS = -std=c++11 -Wall -g 


compression_zlib:
	make -C ../../parser clean
	make -C ../../parser/
	$(CC) $(FLAGS) -o compression_zlib.bin ../../chunker/bufsplit.cpp ../../parser/*.o -I ../../chunker -I ../../parser ../SurpressClosingDeflator.cpp  compression_zlib.cpp


clean:
	$(RM) *.bin
	
