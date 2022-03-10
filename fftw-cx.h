#ifndef _fftwCx_h_
#define _fftwCx_h_

#include <fftw3.h>
#include "datatype.h"

typedef struct FFTWCx {
	int	fftl;
	fftw_plan fwd;
	fftw_plan inv;
	fftw_complex* in;
	fftw_complex* out;
} FFTWCx;

extern FFTWCx* fftwCx_Open (int fftsize);
extern void    fftwCx_Close(FFTWCx* v);
extern void    fftwCx_ExecFwd(FFTWCx* v, Cmplx* Out, Cmplx* In);
extern void    fftwCx_ExecInv(FFTWCx* v, Cmplx* Out, Cmplx* In);
extern fftw_complex* fftwCx_cxBuf(FFTWCx* v);
extern double* fftwCx_wvBuf(FFTWCx* v);
extern void    fftwCx_ExecFwdIB(FFTWCx* v);
extern void    fftwCx_ExecInvIB(FFTWCx* v);

#endif
