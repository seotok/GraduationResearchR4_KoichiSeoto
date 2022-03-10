#ifndef _anaCx_h_
#define _anaCx_h_

#include "fftw-cx.h"
#include "winfunc.h"

typedef struct AnaCx {
	FFTWCx*	fftw;
	int		lfft;
	int		lwin;
	Cmplx*	fbf;
	double*	wintb;
	int		wintype;
} AnaCx;

extern AnaCx* anaCx_Open (int lfft, int lwin, int wintype, int posi);
extern void   anaCx_Execs(AnaCx* wa, short*  wv, Cmplx* cx);
extern void   anaCx_Execf(AnaCx* wa, float*  wv, Cmplx* cx);
extern void   anaCx_Execd(AnaCx* wa, double* wv, Cmplx* cx);
extern void   anaCx_Execi(AnaCx* wa, int* wv, Cmplx* cx);
extern void   anaCx_Close(AnaCx* wa);

#endif
