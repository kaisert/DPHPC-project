#include"map.h"

uint64_t min(uint64_t a, uint64_t b) {
	return (a < b) ? a : b;
}

int compare_key(const Key * a, const Key * b)
{
	uint64_t size_a = a->end - a->begin;
    uint64_t size_b = b->end - b->begin;
	int cmp = strncmp(a->begin, b->begin, min(size_a, size_b));
	if(size_a == size_b)
		return cmp;
	else if(cmp == 0)
	    return (size_a > size_b) ? -1 : 1;
	else
	    return cmp;
}

int compare_key_value_p(const KeyValuePair *a, const KeyValuePair *b)
{
	return compare_key(&(a->key), &(b->key));
}

void sort_map(Map * map) 
{
	qsort(map->map, map->size, sizeof(KeyValuePair), (int (*)(const void*, const void*))compare_key_value_p);
}

uint32_t get_value(const Key const* key, const Map const * map)
{
	int last = map->size - 1;
	int first = 0;
	int middle = map->size / 2;
	
	while(first <= last)
	{
		int cmp = compare_key(& (map->map[middle].key), key);
		if(cmp < 0)
		{
			first = middle + 1;
		}
		else if (cmp == 0)
		{
			return map->map[middle].value;
		}
		else
		{
			last = middle - 1;
		}
		middle = (first + last) / 2;
	}
	return map->size + 1;
}

Map * alloc_map(const char * path)
{
	Map * map = malloc(sizeof(Map));
	map->size = 0;
	map->token_list = * (alloc_token_list(path));
	char * tokens = map->token_list.tokens;
	int length = map->token_list.tokens_length;
	for(char * c = tokens; (c - tokens) < length; ++c)
	{
		map->size += *c == '\n';
	}

	map->map = (KeyValuePair *) malloc(map->size * sizeof(KeyValuePair));
	map->map[0].key.begin = 0;
	int i = 1;
	for(char * c = tokens, * begin = tokens; (c - tokens) < length; ++c)
	{
		if(*c == '\n')
		{
			map->map[i-1].key.begin = begin;
			map->map[i-1].key.end = c;
			map->map[i-1].value = i;
			begin = c + 1;
			++i;
		}
	}

	sort_map(map);
	return map;
}


void destroy_map(Map * map)
{
	free(map->map);
	free(map);
}

void print_map(Map *map)
{	
	printf("map size: %d\n", map->size);
	for(int i = 0; i < map->size; ++i)
	{
		int ksize = map->map[i].key.end - map->map[i].key.begin;
		printf("key: %.*s, value: %d\n", ksize, map->map[i].key.begin, map->map[i].value);
	}
}


