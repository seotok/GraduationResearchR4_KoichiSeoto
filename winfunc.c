/*
**        実数ＦＦＴによる波形<->複素スペクトル分析
*/
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "datatype.h"
#include "const.h"
#include "hpck.h"
#include "winfunc.h"

/*--------------------------------------------------------------
| 関数名: double* winfunc_LineTaper()
| 機能  : 直線テーパ窓初期化
+-------------------------------------------------------------*/
static double* winfunc_LineTaper(double* tab, int n, int ltp)
{
	int	i;
	double T = 1.0/(double)ltp;
	for (i=0; i<ltp; i++) tab[i] = T*i;
	for (i=0; i<n-2*ltp; i++) tab[i+ltp] = 1.0;
	for (i=0; i<ltp; i++) tab[i+n-ltp] = 1.0 - T*i;
	return(tab);
}

/*--------------------------------------------------------------
| 関数名: double* winfunc_HannTaper()
| 機能  : ハニングテーパ窓初期化
+-------------------------------------------------------------*/
static double* winfunc_HannTaper(double* tab, int n, int ltp)
{
	int	i;
	double T = (double)(M_PI / (double)ltp);
	for (i=0; i<ltp; i++) tab[i] = (double)(0.5 - 0.5 * cos(T*i));
	for (i=0; i<n-2*ltp; i++) tab[i+ltp] = 1.0;
	for (i=0; i<ltp; i++) tab[i+n-ltp] = (double)(0.5 - 0.5 * cos(T*(i+ltp)));
	return(tab);
}

/*--------------------------------------------------------------
| 関数名: double* winfunc_Triangle()
| 機能  : 三角窓初期化
+-------------------------------------------------------------*/
static double* winfunc_Triangle(double* tab, int n)
{
	int	i;
	double T = 2.0/(double)n;
	for (i=0; i<n/2; i++) tab[i] = T*i;
	for (; i<n; i++) tab[i] = 1.0 - T*(i-n/2);
	return(tab);
}

/*--------------------------------------------------------------
| 関数名: double* winfunc_Rect()
| 機能  : 矩形窓初期化
+-------------------------------------------------------------*/
static double* winfunc_Rect(double* tab, int n)
{
	int	i;
	for (i=0; i<n; i++) tab[i] = 1.0;
	return(tab);
}

/*--------------------------------------------------------------
| 関数名: double* winfunc_Hann()
| 機能  : ハニング窓初期化
+-------------------------------------------------------------*/
static double* winfunc_Hann(double* tab, int n)
{
	int	i;
	double T = (double)(M_PI*2 / (double)n);
	for (i=0; i<n; i++)
		tab[i] = (double)(0.5 - 0.5 * cos(T*i));
	return(tab);
}

/*--------------------------------------------------------------
| 関数名: double* winfunc_Hamm()
| 機能  : ハミング窓初期化
+-------------------------------------------------------------*/
static double* winfunc_Hamm(double* tab, int n)
{
	double T = (double)(M_PI*2 / (double)n);
	int	i;
	for (i=0; i<n; i++)
		tab[i] = (double)(0.54 - 0.46 * cos(T*i));
	return(tab);
}

/*--------------------------------------------------------------
| 関数名: double* winfunc_Blackman()
| 機能  : ハミング窓初期化
+-------------------------------------------------------------*/
static double* winfunc_Blackman(double* tab, int n)
{
	double T = (double)(M_PI*2 / (double)n);
	int	i;
	for (i=0; i<n; i++)
		tab[i] = (double)(0.42 - 0.5*cos(T*i) + 0.08*cos(2*T*i));
	return(tab);
}

/*--------------------------------------------------------------
| 関数名: double* winfunc_Open(int lfft, int lwin, int type, int posi)
| 機能  : ハミング窓初期化
| 引き数: int lfft  FFT長
| 引き数: int lwin  窓長
| 引き数: int type  窓関数
| 引き数: int posi  フレーム内における窓関数の位置(左端/真ん中)
| 戻り値: 正常終了: 窓関数テーブルへのポインタ
|         異常終了: NULL
+-------------------------------------------------------------*/
double* winfunc_sftOpend(int lfft, int lwin, int type, int sft)
{
	double* win = (double*)MALLOC(sizeof(double)*lfft);
	memset(win, 0.0, sizeof(double)*lfft);

	switch(type){
		case TRIANGLE: 
			return winfunc_Triangle(win+sft, lwin);
		case RECT: 
			return winfunc_Rect(win+sft, lwin);
		case HAMMING:
			return winfunc_Hamm(win+sft, lwin);
		case BLACKMAN:
			return winfunc_Blackman(win+sft, lwin);
		default:
			return winfunc_Hann(win+sft, lwin);
	}
}

double* winfunc_Opend(int lfft, int lwin, int type, int posi)
{
	int sft = posi==WIN_CENTER ? (lfft-lwin)/2 : 0;
	return winfunc_sftOpend(lfft, lwin, type, sft);
}

float* winfunc_Openf(int lfft, int lwin, int type, int posi)
{
	float* winf = (float*)MALLOC(sizeof(float)*lfft);
	double* wind = winfunc_Opend(lfft, lwin, type, posi);
	int i;
	for(i=0; i<lfft; i++) winf[i] = (float)wind[i];	
	FREE(wind);
	return winf;
}

double* winfunc_sftOpendTaper(int lfft, int lwin, int type, int sft, int ltp)
{
	double* win = (double*)MALLOC(sizeof(double)*lfft);
	memset(win, 0.0, sizeof(double)*lfft);

	switch(type){
		/*case LINETAPER: 
			return winfunc_LineTaper(win+sft, lwin, ltp);
		case HANNTAPER: */
		default:
			return winfunc_HannTaper(win+sft, lwin, ltp);
	}
}

double* winfunc_OpendTaper(int lfft, int lwin, int type, int posi, int ltp)
{
	int sft = posi==WIN_CENTER ? (lfft-lwin)/2 : 0;
	return winfunc_sftOpendTaper(lfft, lwin, type, sft, ltp);
}

/*--------------------------------------------------------------
| 関数名: void winfunc_Close(AnaReal* wa)
| 機能  : AnaReal領域クローズ
| 引き数: AnaReal* wa  AnaRealデータへのポインタ
+-------------------------------------------------------------*/
void winfunc_Close(void* win)
{
	FREE(win);
}
