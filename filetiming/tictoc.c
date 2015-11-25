#include "tictoc.h"



struct TicToc TicToc_create(int thread_ID, int max_events) {
    struct TicToc t;
    t.times = (struct timeval*) calloc(max_events+1, sizeof(struct timeval*));
    t.msg = (char **) calloc(max_events, sizeof(char*));
    t.thread_ID = thread_ID;
    t.cur_item = 0;
    t.max_events = max_events;
    return t;
}


void TicToc_start(struct TicToc * t) {
    gettimeofday(t->times, 0);
    t->cur_item = 1;
}


void TicToc_measure(struct TicToc * t, char * msg) {
    t->msg[t->cur_item] = msg;
    gettimeofday(t->times+(t->cur_item++), 0);
}


long int timediff_microseconds(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_usec - start.tv_usec);
}


void TicToc_print_summary(struct TicToc * t) {
    struct timeval base = t->times[0];

    char buf[1000];
    for (int i=1; i < t->cur_item; ++i) {
        long int time = timediff_microseconds(base, t->times[i]);
        if (t->thread_ID >= 0)
            sprintf(buf, "At %ld us (~%ld ms): %s in thread %d.\n",time , time/1000, t->msg[i], t->thread_ID);
        else
            sprintf(buf, "At %ld us (~%ld ms): %s.\n", time, time/1000, t->msg[i]);

        printf("%s", buf);
    }
}


void TicToc_free(struct TicToc * t) {
    free(t->times);
    free(t->msg);
}




