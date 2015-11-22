#include"../mediator.h"
#include"../../parser/tokenstream.h"

int main(void)
{

	Tokenstream * ts = parse_file("/home/tobias/i_ti_eytsch/m1/dphpc/git/xml/test10M",
			"/home/tobias/i_ti_eytsch/m1/dphpc/git/xml/tokens", 1);
	Token * t = NULL;
	for(;;)
	{
		if((t = get_token(ts, t)) != NULL)
		{
			printf("tokentype: %d, token: %.*s\n", t->type, t->end - t->begin, t->begin);
		}
		else
		{
			if(ts->next == NULL)
				break;
			ts = ts->next;
			t = NULL;
		}
	}
	return 0;
}
