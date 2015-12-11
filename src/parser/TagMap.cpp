#include "TagMap.h"

TagMap::TagMap(std::string path)
{
	std::ifstream f_stream(path, std::ios::binary);


	if(!f_stream.eof() && !f_stream.fail()) {
		f_stream.seekg(0, std::ios_base::end);
		std::streampos size = f_stream.tellg();
		_token_data.resize(size);

		f_stream.seekg(0, std::ios_base::beg);
		f_stream.read(&_token_data[0], size);

	}
	char* begin = _token_data.data();
    token_type_t i = 1;
	for(char *it = _token_data.data(); it < _token_data.data() + _token_data.size(); ++it) {
		if(*it == '\n') {
			TagKey new_key(begin, it);
			begin = it + 1;
			map[new_key] = static_cast<token_type_t >(i);
            std::cout << map[new_key] << std::endl;
            i++;
		}
	}
}
