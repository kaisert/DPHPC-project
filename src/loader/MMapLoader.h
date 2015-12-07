//
// Created by Stefan Dietiker on 07/12/15.
//


#ifndef DPHPC15_MMAPLOADER_H
#define DPHPC15_MMAPLOADER_H
#include "Loader.h"

class MMapLoader: public Loader {

public:
    virtual char *operator()(string fname);
    virtual std::size_t length();
    virtual ~MMapLoader();
private:
    std::size_t _length;
    char* _buf;
    int _fd_xml;
};


#endif //DPHPC15_MMAPLOADER_H
