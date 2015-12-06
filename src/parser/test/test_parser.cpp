#include<stdlib.h>
#include<stdio.h>
#include<vector>
#include"../map.h"
#include"../parser.h"

using namespace std;

int main(void) {
	FILE *fp;
	long size;
	char *buffer;

	fp = fopen("./01.xml", "r");
	if(!fp)
	{
		//fputs("File error", sdterr);
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buffer = (char*)  malloc(sizeof(char) * size);
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
	
//	Parser *prs = alloc_parser(buffer, buffer + size);
	Map *map = alloc_map("./token");
    Parser prs(buffer, buffer + size, map);
    vector<token_type_t> ts;
    vector<char*> os;
    auto tbi = back_inserter(ts);
    auto obi = back_inserter(os);
    prs.parse(tbi, obi);
	Token t;

    vector<token_type_t>::iterator tit = ts.begin();
    vector<char*>::iterator cit = os.begin();
    for(;tit != ts.end(); tit++, cit++)
    {
        printf("tag off: %ld, type: %d\n", *cit, *tit);

    }
	return 0;
}
