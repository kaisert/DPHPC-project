
void* compression_init();

size_t compression_deflate(
        void*,      /// < point to init struct
        void* src,  /// < source
        size_t n_src, /// < chunk size
        void* dst,  /// < dest
        size_t dst_size
        );


size_t compression_inflate(
        void*,      /// < point to init struct
        void* src,  /// < source
        size_t n_src, /// < chunk size
        void* dst,  /// < dest
        size_t dst_size
        );

void* compression_free(void*);
