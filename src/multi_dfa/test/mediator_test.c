#include"../mediator.h"

int main(void)
{

	Tokenstream * ts = parse_file("./test10M",
			"./tokens", 1);
	Token * t = NULL;
	for(;;)
	{
		if((t = ts_get_token(ts, t)) != NULL)
		{
			printf("tokentype: %d, token: %.*s\n", t->type, (int) (t->end - t->begin), t->begin);
		}
		else
		{
			if((ts->next = NULL))
				break;
			ts = ts->next;
			t = NULL;
		}
	}
}
