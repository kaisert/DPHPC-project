#ifndef ZLIBDEFLATOR_H
#define ZLIBDEFLATOR_H


#include <memory>
#include <vector>
#include "../parser/token_list.h"
#include "zlib-1.2.8/zlib.h"


template<typename iter>
class ZLibDeflator {
public:
    using Token_Type = std::allocator<token_type_t>;
    typedef Token_Type allocator_type;
    typedef typename Token_Type::value_type value_type;
    typedef typename Token_Type::reference reference;
    typedef typename Token_Type::const_reference const_reference;
    typedef typename Token_Type::difference_type difference_type;
    typedef typename Token_Type::size_type size_type;
    typedef std::output_iterator_tag iterator_category;


    ZLibDeflator(iter it, int compress_buffer_size)
    : out_iterator(it),
    buffer_size(compress_buffer_size),
    next_index(0)
    {
        in_buf = new token_type_t[compress_buffer_size];
        out_buf = new token_type_t[compress_buffer_size];

        strm.zalloc = Z_NULL;
        strm.zfree = Z_NULL;
        strm.opaque = Z_NULL;
        //int ret = 
        deflateInit(&strm, /* level */ 5);
        //if (ret != Z_OK)
        //    printf("WARNING: initialization of zlib stream failed.\n");
    }

    ~ZLibDeflator() {
        delete [] in_buf;
        delete [] out_buf;
    }

    

    inline ZLibDeflator& operator++() {return *this; };
    inline ZLibDeflator& operator++(int){return *this;};
    inline ZLibDeflator& operator*() { return *this; };

    ZLibDeflator& operator=(token_type_t t) {
        
        //store the compressed content in the out_iterator
        in_buf[next_index++] = t;
        if (next_index == buffer_size)
            compressRemaining();

        return *this;
    }


    /**
     * flush the remaining buffered data and close the zlib stream
     */
    void flush()
    {
        //save remaining buffered data
        compressRemaining();
        *out_iterator++ = NULL_TOKEN;

        /* clean up and return */
        //(void)deflateEnd(&strm);
    }


    

private:
    iter out_iterator;
    int buffer_size;
    int next_index;
    token_type_t * in_buf;
    token_type_t * out_buf;
    z_stream strm;

    void compressRemaining() {
        for (int i=0;i<next_index;++i)
            printf("ctoken %x\n", in_buf[i]);

        //inflate();
        //*out_iterator++ = t;

#ifdef ENABLE_WIP_CODE
        strm.avail_in = 0; /* #bytes */
        strm.next_in = in_buf;

        strm.avail_out = buffer_size;
        strm.next_out = out_buf;



        /* compress until end of file */
        do {
            strm.avail_in = fread(in, 1, CHUNK, source);
            if (ferror(source)) {
                (void)deflateEnd(&strm);
                return Z_ERRNO;
            }
            flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
            strm.next_in = in;

            /* run deflate() on input until output buffer not full, finish
               compression if all of source has been read in */
            do {
                strm.avail_out = CHUNK;
                strm.next_out = out;
                ret = deflate(&strm, flush);    /* no bad return value */
                assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
                have = CHUNK - strm.avail_out;
                if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                    (void)deflateEnd(&strm);
                    return Z_ERRNO;
                }
            } while (strm.avail_out == 0);
            assert(strm.avail_in == 0);     /* all input will be used */

            /* done when last data in file processed */
        } while (flush != Z_FINISH);
        assert(ret == Z_STREAM_END);        /* stream will be complete */
#endif

        next_index = 0;
    }
};

#endif
