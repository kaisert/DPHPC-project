#include <omp.h>
#include <stdio.h>

int main(int argc, char** argv)
{
	int nthreads,tid;

    for (int i=0;i<argc;++i)
        printf("%s\n", argv[i]);

	#pragma omp parallel private(nthreads, tid)
	{
		tid = omp_get_thread_num();
		nthreads = omp_get_num_threads();
		printf("Hello world from thread %i of %i\n", tid, nthreads);
	}
}
