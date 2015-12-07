//
// Created by Stefan Dietiker on 07/12/15.
//

#include<string>

using namespace std;

#ifndef DPHPC15_LOADER_H
#define DPHPC15_LOADER_H

class Loader {
public:
    virtual ~Loader() {};
    virtual char* operator() (string fname) = 0;
    virtual size_t length() = 0;
};

#endif //DPHPC15_LOADER_H
