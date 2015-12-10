#ifndef VECTOR_WRAPPER
#define VECTOR_WRAPPER
template<typename T>
class MyVector : public std::vector<T> {
private:
    typedef typename std::vector<T>::const_iterator c_iter;
public:
    inline void init(int i){}
    inline void flush(){}
};

#endif
