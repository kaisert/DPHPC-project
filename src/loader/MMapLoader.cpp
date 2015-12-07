//
// Created by Stefan Dietiker on 07/12/15.
//

#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include "MMapLoader.h"

#include"../exceptions/GeneralException.h"

#ifdef __APPLE__
#define MAP_POPULATE 0
#endif

char *MMapLoader::operator()(string fname) {
    struct stat f_xml_stat;

    _fd_xml = open(fname.c_str(), O_RDWR);
    if (_fd_xml < 0)
        throw GeneralException("could not open xml file.");

    fstat(_fd_xml, &f_xml_stat);
    _length = static_cast<std::size_t>(f_xml_stat.st_size);

    _buf = static_cast<char*>(
            mmap(   NULL,                  // address
                    _length,               // length
                    PROT_READ,             // prot flags
                    MAP_PRIVATE | MAP_POPULATE, // flags
                    _fd_xml,                // file decriptor
                    0)                     // offset in file
    );

#ifdef __APPLE__
    mlock(_buf, _length);
#endif
    if(_buf == MAP_FAILED)
        throw GeneralException("mmap failed");

    return _buf;
}

MMapLoader::~MMapLoader() {
    munmap(_buf, _length);
    close(_fd_xml);
}

std::size_t MMapLoader::length() {
    return _length;
}
