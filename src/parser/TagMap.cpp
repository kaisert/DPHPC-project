#include "TagMap.h"

TagMap::TagMap(std::string path)
{
	std::ifstream f_stream(path, std::ios::binary);
	std::vector<char> tokens;

	if(!f_stream.eof() && !f_stream.fail()) {
		f_stream.seekg(0, std::ios_base::end);
		std::streampos size = f_stream.tellg();
		tokens.resize(size);

		f_stream.seekg(0, std::ios_base::beg);
		f_stream.read(&tokens[0], size);
	}
	char* begin = tokens.data();
    token_type_t i = 1;
	for(char *it = tokens.data(); it < tokens.data() + tokens.size(); ++it) {
		if(*it == '\n') {
			TagKey new_key(begin, it - 1);
			begin = it + 1;
			map[new_key] = i++;
		}
	}

}
