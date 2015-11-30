#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/mman.h>

#include<parser.h>
#include<token_list.h>
#include<bufsplit.h>

#include<iostream>
#include<vector>
#include<iterator>
#include"../SurpressClosingDeflator.h"
#include"../ZLibDeflator.h"

#define panic(msg) \
    do{perror(msg); exit(EXIT_FAILURE); } while(0)

typedef std::back_insert_iterator< std::vector<token_type_t> > back_inserter_t;
using namespace std;

int main(int argc, char * argv[])
{
    if(argc < 4) panic("not enough parameters to main(). Need: xml, tokens, output\n");
#define ARG_XML argv[1]
#define ARG_KNOWN_TOKENS argv[2]
#define ARG_OUT argv[3]

    int fd_xml;
    struct stat f_xml_stat;
    off_t xml_len;

    fd_xml = open(ARG_XML, O_RDWR);
    if(fd_xml < 0 ) panic("could not open xml file.");
    fstat(fd_xml, &f_xml_stat);
    xml_len = (off_t) f_xml_stat.st_size;

    char * xml_buf = 
        (char*) mmap(NULL,
                xml_len,
                PROT_READ,
                MAP_PRIVATE | MAP_POPULATE,//MAP_FILE|MAP_SHARED,
                fd_xml,
                0);
    //chunk xml stream
    char * chunks[2];
    bufsplit_split_xml_stream(xml_buf, xml_len, 1, chunks);

    //open file to be written to
    FILE *out;
    out = fopen(ARG_OUT, "w");

    Map * map = alloc_map(ARG_KNOWN_TOKENS);

    Parser prs(*chunks, *(chunks + 1), map);
    vector<token_type_t> ts;
    vector<char*> os;
    auto tit = back_inserter(ts);
    auto obi = back_inserter(os);
    ZLibDeflator<back_inserter_t> d(tit, 10);
    prs.parse(d, obi);
    std::cerr << "parsed\n";
    d.flush();
    for(std::vector<token_type_t>::iterator it = ts.begin(); it != ts.end();
            ++it)
    {
        uint16_t current = *it;
        /*msb = (char) ((current) > 8);
        lsb = (char) (current);
        fputc(lsb, out);
        fputc(msb, out);*/
        fwrite(&current, sizeof(uint16_t), 1, out);
    }

    fclose(out);
    destroy_map(map);

    close(fd_xml); munmap(xml_buf, xml_len);
}
