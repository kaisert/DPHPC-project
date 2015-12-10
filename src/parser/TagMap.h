#ifndef TAG_MAP_H
#define TAG_MAP_H
#include <unordered_map>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>

#include "TagKey.h"
#include "../config_local.h"

class TagMap {
public:
	TagMap(std::string path);
	inline config::token_type_t get_value(TagKey &k);
	inline unsigned long size();

private:
	struct KeyHasher
	{
		std::size_t operator()(const TagKey& k) const
		{
			return k.hash_value;
		}
	};
	std::unordered_map<TagKey, config::token_type_t, KeyHasher> map;
};

#endif
