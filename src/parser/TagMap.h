#ifndef TAG_MAP_H
#define TAG_MAP_H

#include<vector>
#include<string>
#include<unordered_map>
#include<iostream>
#include<fstream>
#include"TagKey.h"
#include "../token_type.h"


class TagMap {
public:
	TagMap(std::string path);

    token_type_t get_value(TagKey &k)
    {
		auto search = map.find(k);
        if(search != map.end())
            return search->second;
        return map.size()+1;
    }

    size_t size() {
        return map.size();
    }

private:
	struct KeyHasher
	{
		std::size_t operator()(const TagKey& k) const
		{
			return k.hash_value;
		}
	};
	std::unordered_map<TagKey, token_type_t, KeyHasher> map;
};

#endif
