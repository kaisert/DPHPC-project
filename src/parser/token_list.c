#include "token_list.h"

void destroy_token_list(TokenList * list)
{
	free(list->tokens);
}

void init_token_list(TokenList *token_list, const char * path)
{
	FILE *fp;
	uint32_t size;

	fp = fopen(path, "r");
	if(!fp)
	{
	//	fputs("File error");
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	token_list->tokens = malloc(sizeof(char) * size);
	if(!token_list->tokens)
	{
	//	fputs ("Memory error");
		exit(2);
	}
	
	if(size != fread(token_list->tokens, sizeof(char), size, fp))
	{
		//fputs("Reading error", stderr);
		exit(3);
	}
	fclose(fp);
	
	token_list->tokens_length = size;
}

void print_token(Token *t)
{
	printf("Token: %d, size: %ld", t->type, t->end - t->begin);
}
