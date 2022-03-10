#ifndef _fftw_h_
#define _fftw_h_

#include <fftw3.h>
#include "datatype.h"

typedef struct FFTW {
	int	fftl;
	fftw_plan fwd;
	fftw_plan inv;
	double*   wv;
	fftw_complex* cx;
} FFTW;

extern FFTW* fftw_Open (int fftsize);
extern void  fftw_Close(FFTW* v);
extern void  fftw_Clear(FFTW* v);
extern void  fftw_ExecFwd(FFTW* v, double* wv, Cmplx* C);
extern void  fftw_ExecInv(FFTW* v, double* wv, Cmplx* C);
extern fftw_complex* fftw_cxBuf(FFTW* v);
extern double* fftw_wvBuf(FFTW* v);
extern void  fftw_ExecFwdIB(FFTW* v);
extern void  fftw_ExecInvIB(FFTW* v);

#endif
