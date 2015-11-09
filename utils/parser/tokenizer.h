#include<iostream>
#include<fstream>
#include<string>
#include<functional>
#include<vector>

struct attribute {
    std::string id;
    std::string value;
};

class Tokenizer {
public:
	//type of xml attributes of a tag
    typedef std::vector<attribute> Attributes;
	//function type being called when a start tag event occurs
    typedef std::function<void(std::string const & name, Attributes const & att)> start_tag_event;
	//function type being called when a text event occurs
    typedef std::function<void(std::string const & text)> text_event;
	//function type being called when a end tag event occurs
    typedef std::function<void(std::string const & name)> end_tag_event;
public:
	//parses a istream containing a (or part of a) xml file
    void parse(std::istream & stream);
	//registers function being called when start tag event occurs
    void start_tag(start_tag_event fn);
	//registers function being called when end tag event occurs
    void end_tag(end_tag_event fn);
	//registers function being called when text tag event occurs
    void text_tag(text_event fn);
private:
    start_tag_event start_tag_fn;
    end_tag_event end_tag_fn;
    text_event text_fn;

    std::string parse_nametag(std::istream & stream);
    Attributes parse_attributes(std::istream & stream);
    std::string parse_text(std::istream & stream);
};

