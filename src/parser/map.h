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

extern Map * alloc_map(const char *path);
extern uint32_t get_value(const Key * key, const Map * map);
extern void destroy_map(Map * map);
extern void print_map(Map * map);

#endif
