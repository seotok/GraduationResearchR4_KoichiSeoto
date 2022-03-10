/*
**   	FFT分析補助領域設定＆補助関数
*/

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "hpck.h"
#include "datatype.h"
#include "fftw.h"

/*--------------------------------------------------------------
| 関数名: FFTW* fftw_Open
| 機能  : 実数FFTWモジュールオープン
| 引き数: int fftl FFT点数
| 戻り値: 正常終了: モジュールへのポインタ
|         異常終了: NULL
+-------------------------------------------------------------*/
FFTW *fftw_Open (int fftl)
{
	FFTW *v = (FFTW*)MALLOC(sizeof(FFTW));
	if(!v){
		fprintf(stderr, "fftw_Open: alloc failed\n");
		return(NULL);
	}
	memset(v, 0, sizeof(FFTW));

	v->fftl = fftl;
	if(!(v->wv = (double*)fftw_malloc((fftl+2)*sizeof(double)))){
		fprintf(stderr, "fftw: wv alloc failed\n");
		return(NULL);
	}
	if(!(v->cx = (fftw_complex*)fftw_malloc(fftl*sizeof(fftw_complex)))){
		fprintf(stderr, "fftw: wv alloc failed\n");
		return(NULL);
	}

	v->fwd = fftw_plan_dft_r2c_1d(fftl, v->wv, v->cx, FFTW_MEASURE);
	v->inv = fftw_plan_dft_c2r_1d(fftl, v->cx, v->wv, FFTW_MEASURE);

	return(v);
}

/*--------------------------------------------------------------
| 関数名: void fftw_close
| 機能  : 実数FFTWモジュールクローズ
| 引き数: FFTW *v  モジュールへのポインタ
| 戻り値: 正常終了: 0
|         異常終了: -1
+-------------------------------------------------------------*/
void	fftw_Close(FFTW *v)
{
	if(!v) return;

	fftw_destroy_plan(v->fwd);
	fftw_destroy_plan(v->inv);
	fftw_free(v->wv);
	fftw_free(v->cx);
	FREE(v);
}

void	fftw_Clear(FFTW* v)
{
	memset(v->wv, 0, sizeof(double)*(v->fftl+2));
	memset(v->cx, 0, sizeof(fftw_complex)*v->fftl);
}

/*--------------------------------------------------------------
| 関数名: void fftw_Exec
| 機能  : 実数FFT分析実行
| 引き数: FFTW*   v  分析領域のポインタ
|         double* w  波形へのポインタ
|         Cmplx*  C  スペクトル配列
|
|         N/2+1個の要素が必要 : C[0],C[1],...,C[N/2] 
|                               常に C[0].i = C[N/2].i = 0
|
| 戻り値: 正常終了: 0
|         異常終了: -1
+-------------------------------------------------------------*/
void	fftw_ExecFwd(FFTW *v, double *wv, Cmplx* cx)
{
	memcpy(v->wv, wv, sizeof(double)*v->fftl);
	fftw_execute(v->fwd);
	memcpy(cx, v->cx, sizeof(Cmplx)*(v->fftl/2+1));
}

void	fftw_ExecInv(FFTW *v, double *wv, Cmplx* cx)
{
	memcpy(v->cx, cx, sizeof(Cmplx)*(v->fftl/2+1));
	fftw_execute(v->inv);
	memcpy(wv, v->wv, sizeof(double)*v->fftl);
}

void	fftw_ExecFwdIB(FFTW *v)
{
	fftw_execute(v->fwd);
}

void	fftw_ExecInvIB(FFTW *v)
{
	fftw_execute(v->inv);
}

double* fftw_wvBuf(FFTW* v)
{
	return v->wv;
}

fftw_complex* fftw_cxBuf(FFTW* v)
{
	return v->cx;
}
