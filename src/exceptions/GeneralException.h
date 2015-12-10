//
// Created by Stefan Dietiker on 07/12/15.

#ifndef DPHPC15_GENERALEXCEPTION_H
#define DPHPC15_GENERALEXCEPTION_H

struct GeneralException: public std::exception {
    std::string _err_msg;
    GeneralException(std::string err_msg): _err_msg(err_msg) {};
    ~GeneralException() throw () {}
    const char* what() const throw() { return _err_msg.c_str(); }
};

#endif //DPHPC15_GENERALEXCEPTION_H
