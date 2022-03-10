/*
**        複素ＦＦＴによる波形<->複素スペクトル分析
*/
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "datatype.h"
#include "const.h"
#include "hpck.h"
#include "fftw-cx.h"
#include "anacx.h"

/*--------------------------------------------------------------
| 関数名: AnaCx* anaCx_Open(int lfft, int wintype)
| 機能  : FFT分析領域初期化
| 引き数: int lfft    FFT点数
|         int lwin    窓長
|         int wintyp  窓関数の種類 (HANNING/HAMMING/RECT/HALFRECT)
|         int posi    フレーム内の窓の位置 (WIN_CENTER/WIN_LEFT)
|
| 戻り値: 正常終了: 分析領域のポインタ
|         異常終了: NULL
+-------------------------------------------------------------*/
AnaCx* anaCx_Open(int lfft, int lwin, int win_typ, int posi)
{
	if(lfft<lwin){
		fprintf(stderr, "anaCx_Open: FFT長と窓長が不適切です。(lfft=%d, lwin=%d)\n", 
			lfft, lwin);
		return(NULL);
	}

	AnaCx* wa = (AnaCx*)MALLOC(sizeof(AnaCx));
	if(!wa){
		fprintf(stderr,"anaCx: area alloc error\n");
		return(NULL);
	}
	memset(wa, 0, sizeof(AnaCx));
	wa->fftw = fftwCx_Open(lfft);
	wa->lfft = lfft;
	wa->lwin = lwin;
	wa->wintype = win_typ;
	wa->wintb = winfunc_Opend(lfft, lwin, win_typ, posi);
	wa->fbf = (Cmplx*)MALLOC(sizeof(Cmplx)*lfft);
	memset(wa->fbf, 0, sizeof(Cmplx)*lfft);
	return(wa);
}

/*--------------------------------------------------------------
| 関数名: void anaCx_Close(AnaCx* wa)
| 機能  : AnaCx領域クローズ
| 引き数: AnaCx* wa  AnaCxデータへのポインタ
+-------------------------------------------------------------*/
void anaCx_Close(AnaCx* wa)
{
	if(!wa) fprintf(stderr, "anaCx_Close: データポインタがNULLです。\n");
	fftwCx_Close(wa->fftw);
	FREE(wa->fbf);
	FREE(wa->wintb);
	FREE(wa);
}

#define F2S_ROUND(f) ((f)>0.0 ? (short)(0.5+(f)) : (short)((f)-0.5))
/*--------------------------------------------------------------
| 関数名: static void anaCx_Execs
| 機能  : FFT分析実行 (short 波形 --> Cmplx スペクトル)
| 引き数: AnaCx* wa  分析モジュールへのポインタ
|         short*   wv  入力波形へのポインタ
|         Cmplx*   spc 出力複素スペクトル  長さ N/2+1(0〜N/2) の配列が必要
+-------------------------------------------------------------*/
void anaCx_Execs(AnaCx *wa, short *wv, Cmplx* spc)
{
	int i;
	for(i=0; i<wa->lfft; i++)
		wa->fbf[i].r = (double)wv[i]*wa->wintb[i];
	fftwCx_ExecFwd(wa->fftw, spc, wa->fbf); 
}

/*--------------------------------------------------------------
| 関数名: static void anaCx_Execf
| 機能  : FFT分析実行 (float 波形 --> Cmplx スペクトル)
| 引き数: AnaCx* wa  分析モジュールへのポインタ
|         float*   wv  入六波形へのポインタ
|         Cmplx*   spc 出力複素スペクトル  長さ N/2+1(0〜N/2) の配列が必要
+-------------------------------------------------------------*/
void anaCx_Execf(AnaCx *wa, float *wv, Cmplx* spc)
{
	int i;
	for(i=0; i<wa->lfft; i++)
		wa->fbf[i].r = (double)wv[i]*wa->wintb[i];
	fftwCx_ExecFwd(wa->fftw, spc, wa->fbf); 
}

/*--------------------------------------------------------------
| 関数名: static void anaCx_Execd
| 機能  : FFT分析実行 (double 波形 --> Cmplx スペクトル)
| 引き数: AnaCx* wa  分析モジュールへのポインタ
|         double*  wv  入六波形へのポインタ
|         Cmplx*   spc 出力複素スペクトル  長さ N/2+1(0〜N/2) の配列が必要
+-------------------------------------------------------------*/
void anaCx_Execd(AnaCx *wa, double *wv, Cmplx* spc)
{
	int i;
	for(i=0; i<wa->lfft; i++)
		wa->fbf[i].r = wv[i]*wa->wintb[i];
	fftwCx_ExecFwd(wa->fftw, spc, wa->fbf); 
}

/*--------------------------------------------------------------
| 関数名: static void anaCx_Execi
| 機能  : FFT分析実行 (int 波形 --> Cmplx スペクトル)
| 引き数: AnaCx* wa  分析モジュールへのポインタ
|         int*     wv  入六波形へのポインタ
|         Cmplx*   spc 出力複素スペクトル  長さ N/2+1(0〜N/2) の配列が必要
+-------------------------------------------------------------*/
void anaCx_Execi(AnaCx *wa, int *wv, Cmplx* spc)
{
	int i;
	for(i=0; i<wa->lfft; i++)
		wa->fbf[i].r = wv[i]*wa->wintb[i];
	fftwCx_ExecFwd(wa->fftw, spc, wa->fbf); 
}
