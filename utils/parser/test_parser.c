#include<stdlib.h>
#include<stdio.h>
#include"map.h"
#include"parser.h"

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
	
	Parser *prs = alloc_parser(buffer, buffer + size);
	Map *map = alloc_map();
	init_parser(prs, map);

	Token t;

	while(get_next_token(prs, &t) != 0)
	{
		int size = t.end - t.begin;
		printf("tag: %.*s, type: %d\n", size, t.begin, t.type);
	}

	return 0;
}
