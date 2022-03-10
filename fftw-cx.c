/*
**   	FFT分析補助領域設定＆補助関数
*/

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "hpck.h"
#include "datatype.h"
#include "fftw-cx.h"

/*--------------------------------------------------------------
| 関数名: FFTWCx* fftwCx_Open
| 機能  : 複素数FFTWCxモジュールオープン
| 引き数: int fftl FFT点数
| 戻り値: 正常終了: モジュールへのポインタ
|         異常終了: NULL
+-------------------------------------------------------------*/
FFTWCx *fftwCx_Open (int fftl)
{
	FFTWCx	*v = (FFTWCx*)MALLOC(sizeof(FFTWCx));
	if(!v){
		fprintf(stderr, "fftw: alloc failed\n");
		return(NULL);
	}
	memset(v, 0, sizeof(FFTWCx));

	v->fftl = fftl;
	if(!(v->in = (fftw_complex*)fftw_malloc(fftl*sizeof(fftw_complex)))){
		fprintf(stderr, "fftw-cx: wv alloc failed\n");
		return(NULL);
	}
	if(!(v->out = (fftw_complex*)fftw_malloc(fftl*sizeof(fftw_complex)))){
		fprintf(stderr, "fftw-cx: wv alloc failed\n");
		return(NULL);
	}

	v->fwd = fftw_plan_dft_1d(fftl, v->in, v->out, FFTW_FORWARD, FFTW_MEASURE);
	v->inv = fftw_plan_dft_1d(fftl, v->in, v->out, FFTW_BACKWARD, FFTW_MEASURE);

	return(v);
}

/*--------------------------------------------------------------
| 関数名: void fftwCx_close
| 機能  : 複素数FFTWCxモジュールクローズ
| 引き数: FFTWCx *v  モジュールへのポインタ
| 戻り値: 正常終了: 0
|         異常終了: -1
+-------------------------------------------------------------*/
void	fftwCx_Close(FFTWCx *v)
{
	if(!v) return;
	fftw_destroy_plan(v->fwd);
	fftw_destroy_plan(v->inv);
	fftw_free(v->in);
	fftw_free(v->out);
	FREE(v);
}

/*--------------------------------------------------------------
| 関数名: void fftwCx_Exec
| 機能  : 複素数FFT分析実行
| 引き数: FFTWCx* v    分析領域のポインタ
|         Cmplx*  out  スペクトル配列
|         Cmplx*  in   スペクトル配列
|
|         N/2+1個の要素が必要 : C[0],C[1],...,C[N/2] 
|                               常に C[0].i = C[N/2].i = 0
|
| 戻り値: 正常終了: 0
|         異常終了: -1
+-------------------------------------------------------------*/
void	fftwCx_ExecFwd(FFTWCx *v, Cmplx* Out, Cmplx* In)
{
	memcpy(v->in, In, sizeof(Cmplx)*v->fftl);
	fftw_execute(v->fwd);
	memcpy(Out, v->out, sizeof(Cmplx)*v->fftl);
}

void	fftwCx_ExecInv(FFTWCx *v, Cmplx* Out, Cmplx* In)
{
	memcpy(v->in, In, sizeof(Cmplx)*v->fftl);
	fftw_execute(v->inv);
	memcpy(Out, v->out, sizeof(Cmplx)*v->fftl);
}

void	fftwCx_ExecFwdIB(FFTWCx *v)
{
	fftw_execute(v->fwd);
}

void	fftwCx_ExecInvIB(FFTWCx *v)
{
	fftw_execute(v->inv);
}
