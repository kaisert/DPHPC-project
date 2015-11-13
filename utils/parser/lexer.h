#include<string>
#include<functional>
#include<iterator>

template <class RandIterator>
class Lexer {
public:
	//function type being called when a start tag event occurs
    typedef std::function<void(std::string const & name, long start_pos, long end_pos)> start_tag_event;
	//function type being called when a text event occurs
    typedef std::function<void(long start_pos, long end_pos)> text_event;
	//function type being called when a end tag event occurs
    typedef std::function<void(std::string const & name, long start_pos, long end_pos)> end_tag_event;

private:
    enum Tag{START_TAG, END_TAG};

    class IteratorHelper {
    public:
        IteratorHelper(RandIterator & it){
            it_ = it;
            pos_ = 0;
            current_ = *it;
        }

        void forward(){
            it_++;
            current_ = *it_;
            pos_++;
        }

        IteratorHelper& operator++(){
            forward();
            return * this;
        }

        IteratorHelper& operator++(int){
            forward();
            return * this;
        }

        char operator*(){
            return current_;
        }

        long pos(){return pos_;}
    private:
        RandIterator it_;
        long pos_;
        char current_;
    };


public:
    Lexer(RandIterator & it, const long offset, const long input_length)
            : offset_(offset), input_length_(input_length), ith_(it){ }

    void lex() {
        while(ith_.pos() != input_length_){
            long start_pos;
            long end_pos;
            switch (*ith_) {
                case '<': {
                    std::string name = "";
                    start_pos = ith_.pos() + offset_;
                    Tag tag_type = lex_tag(name);
                    end_pos = ith_.pos() + offset_;
                    if (tag_type == START_TAG) {
                        start_tag_fn(name, start_pos, end_pos);
                    } else {
                        end_tag_fn(name, start_pos, end_pos);
                    }
                    ith_++;
                    break;
                }
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                case '>':
                    break;
                default:
                    start_pos = ith_.pos() + offset_;
                    lex_text();
                    end_pos = ith_.pos() + offset_;
                    text_fn(start_pos, end_pos);
                    break;
            }
        }
    }

    void start_tag(start_tag_event fn) {
        start_tag_fn = fn;
    }

    void end_tag(end_tag_event fn) {
        end_tag_fn = fn;
    }

    void text_tag(text_event fn) {
        text_fn = fn;
    }

private:
    IteratorHelper ith_;
    long const offset_;
    long const input_length_;
    long pos_;

    start_tag_event start_tag_fn;
    end_tag_event end_tag_fn;
    text_event text_fn;

    Tag lex_tag(std::string &name) {
        ith_++;
        Tag type;
        if(*ith_ == '/') {
            type = END_TAG;
            ith_++;
        } else {
            type = START_TAG;
        }
        bool tag_lexed = false;
        name = "";
        while(*ith_ != '>') {
            tag_lexed = *ith_ == ' ';
            if (!tag_lexed) {
                name += *ith_;
            }
            ++ith_;
        }
        return type;
    }

    void lex_text() {
        while(*ith_ != '<'){
            ith_++;
        }
    }
};

