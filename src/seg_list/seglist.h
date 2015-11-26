#ifndef H_SEGLIST
#define H_SEGLIST

#include<stddef.h>

// TYPES
// TODO: define struct
typedef struct {
} seglist_t;

// TODO: exact types TBD
typedef void* seglist_seg_t;
typedef void* seglist_read_iter_t;

//
void seglist_create(
        seglist_t* seglist,
        size_t buf_size,
        size_t elem_size);


seglist_seg_t seglist_create_segment( // create new segment
        seglist_t*);


size_t seglist_append_n(    // only operates on segment seg
        seglist_seg_t* seg,
        void* chunk,
        size_t n);


size_t seglist_flush(seglist_seg_t* seg); // flushes a segment


void seglist_create_read_iter(  // creates a read iterator for the entire list
        seglist_read_iter_t* read_iter,
        seglist_t seglist);


size_t seglist_get_next_n(
        seglist_read_iter_t* read_iter,
        void *chunk,
        size_t n);

#endif
