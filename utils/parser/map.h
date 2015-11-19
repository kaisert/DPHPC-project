#ifndef MAP_H
#define MAP_H
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "token_list.h"

typedef struct Key{
	char * begin, * end;
} Key;

typedef struct KeyValuePair{
	Key key;
	uint32_t value;
} KeyValuePair;

typedef struct Map {
	TokenList token_list;
	KeyValuePair * map;
	int size;
} Map;

extern Map * alloc_map();
extern uint32_t get_value(const Key const*, const Map const *);
extern void destroy_map(Map *);
#endif
