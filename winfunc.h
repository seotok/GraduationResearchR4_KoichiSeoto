#ifndef _winfunc_h_
#define _winfunc_h_

#define WIN_CENTER (1)
#define WIN_LEFT (0)

extern double* winfunc_sftOpend(int lfft, int lwin, int wintype, int sft);
extern double* winfunc_Opend(int lfft, int lwin, int wintype, int posi);
extern float*  winfunc_Openf(int lfft, int lwin, int wintype, int posi);
extern double* winfunc_sftOpendTaper(int lfft, int lwin, int wintype, int sft, int ltp);
extern double* winfunc_OpendTaper(int lfft, int lwin, int wintype, int posi, int ltp);
extern void    winfunc_Close(void* win);

#endif
