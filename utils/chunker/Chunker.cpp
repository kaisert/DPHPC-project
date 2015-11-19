#include <omp.h>
#include <cstdio>
#include <cstdint>

using namespace std;

class Chunker {
    public:
        Chunker(const char* filename, bool &error) {

            FILE* fp = fopen(filename, "r");
            if(!fp) {
                perror("Chunker: File opening failed with ");
                error = true;
                return;
            }

            fseek(fp, 0L, SEEK_END);
            filesize_ = ftell(fp);
            fseek(fp, 0L, SEEK_SET);


            file_ = fp;


            //printf("Chunker: file size is %ld\n", filesize_);
            buf_ = new char[filesize_+1];

            if (!buf_) {
                perror("Not enough memory to read the complete file into memory.\n");
                error = true;
                return;
            }

            error = false;
        }

        void load_file_content() {
            uint64_t size = 0;
            uint64_t c;

            while ((c = fread(buf_, sizeof(buf_[0]), filesize_, file_)) != 0) {
                size += c;
            }
            
            if (filesize_ != size) {
                printf("Warning: Actual amount of data read does not match computed filesize_.");
            }
        }

        /**
         * Divides the file into num_chunks chunks and computes the chunk start and end positions.
         *
         * num_chunks   {int}   Number of chunks or -1 for the machine's core number
         * returns      {char**} Array of char* of size num_chunks+1, indicating the boundaries
         *
         * Warning:  Empty chunks (start == end) are possible in rare cases
         */
        char** compute_chunks(int num_chunks) {

            int num_threads = omp_get_num_procs();
            if (num_chunks < 1)
                num_chunks = num_threads;
            

            char ** chunks = new char* [num_chunks+1];
            chunks[0] = buf_;
            chunks[num_chunks] = buf_ + filesize_ + 1; //last chunk ends at EOF+1


            long chunk_size = filesize_/num_chunks;
            if (filesize_ % num_chunks != 0)
            {
                ++chunk_size;
                printf("Chunker: chunk_size has been ceiled to %ld to read the whole file.\n", chunk_size);
            } else
                printf("Chunker: chunk_size is %ld.\n", chunk_size);


            buf_[filesize_] = '<'; //prevent chunk computation from overshooting

            if (num_chunks/num_threads > 200)
            {
                #pragma omp parallel for \
                            schedule(static,num_chunks/num_threads)
                for (int i=1;i < num_chunks; ++i)
                {
                    char * locbuf = buf_ + chunk_size * i;
                    while (*locbuf++ != '<')
                        /*no other work necessary*/;

                    --locbuf; //reset to position of '<'

                    //printf("locbuf of chunk %i set to %ld in thread %d\n", 
                    //        i, (long) (locbuf - buf_), omp_get_thread_num());
                }
            } else {
                for (int i=1;i < num_chunks; ++i)
                {
                    char * locbuf = buf_ + chunk_size * i;

                    while (*locbuf++ != '<') { /*no other work necessary*/; }

                    --locbuf; //reset to position of '<'

                    //printf("locbuf of chunk %i set to %ld.\n", 
                    //        i, (long) (locbuf - buf_));
                }
            }
            return chunks;
        }


        ~Chunker() {

            delete [] buf_;

            if (ferror(file_))
                puts("Chunker I/O error while reading file ");
         
            fclose(file_);
        }

    private:
        FILE * file_;
        char* buf_;
        uint64_t filesize_;
};
