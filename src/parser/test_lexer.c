#include<stdlib.h>
#include<stdio.h>
#include"lexer.h"

int main(void) {
	FILE *fp;
	long size;
	char *buffer;

	fp = fopen("/home/tobias/i_ti_eytsch/m1/dphpc/git/implementation/utils/parser/01.xml", "r");
	if(!fp)
	{
		//fputs("File error", sdterr);
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buffer = malloc(sizeof(char) * size);
	if(!buffer)
	{
		//fputs ("Memory error", stderr);
		exit(2);
	}

	if(size != fread(buffer, sizeof(char), size, fp))
	{
		//fputs("Reading error", stderr);
		exit(3);
	}

	fclose(fp);
	printf("file: \n%s\n", buffer);

	Lexer lxr;
	lxr.begin = buffer;
	lxr.end = buffer + size;
	Tag t;
	while(get_next_tag(&lxr, &t) == 1){
		int size_tag, size_name;
		size_tag = t.end - t.begin;
		size_name = t.end_of_id - (t.begin + 1);
		printf("Tag: %.*s, Tagname: %.*s\n", size_tag, t.begin, size_name, t.begin + 1); 
	}


	free(buffer);
	
	return 0;
}
