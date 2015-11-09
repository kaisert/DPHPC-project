#include "tokenizer.h"

using namespace std;

void start_tag(string const & name, Tokenizer::Attributes const & att){
    cout << "start tag name: " << name << "\n";
}

void end_tag(string const & name) {
    cout << "end tag name: " << name << "\n";
}

void text(string const & text) {
    cout << "text: " << text << "\n";
}

int main() {

    std::fstream fs;
    fs.open("/home/tobias/i_ti_eytsch/m1/dphpc/git/implementation/utils/parser/xml/01.xml", ios_base::in);

    Tokenizer lxr;
    lxr.start_tag(start_tag);
    lxr.end_tag(end_tag);
    lxr.text_tag(text);
    lxr.parse(fs);
    fs.close();
    return 0;
}
