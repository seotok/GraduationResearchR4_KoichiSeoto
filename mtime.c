#include <stdio.h>
#include <time.h>
#include <stdlib.h>

static struct timespec st, ed;
static int ct, nct;
static double ttime;

void mtime_start(int ict)
{
	if(ict <= 0){
		fprintf(stderr, "mtime_start: 平均化回数が０です。\n");
		exit(0);
	}
	nct = ict;
//	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &st);
	clock_gettime(CLOCK_MONOTONIC, &st);
	ct ++;
}

void mtime_stop(FILE* fp, char* msg)
{
//	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ed);
	clock_gettime(CLOCK_MONOTONIC, &ed);
	double tst = st.tv_sec + st.tv_nsec*1.0e-9;
	double ted = ed.tv_sec + ed.tv_nsec*1.0e-9;
	ttime += ted - tst;
	if(ct%nct == nct-1){
		fprintf(fp, "%s %.3f ms\n", msg, ttime*1.0e3/(double)nct);
		ttime = 0.0;
	}
}
