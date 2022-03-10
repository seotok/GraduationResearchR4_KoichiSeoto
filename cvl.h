#ifndef cvl_h
#define cvl_h

typedef struct Cvl {
	int     lh;
	int     blk;
	double  *buf;
} Cvl;

Cvl* cvl_Open(int lh, int blk);
void	cvl_Close(Cvl *cvl);
void	cvl_Clear(Cvl *cvl);
void	cvl_Execss(Cvl* cvl, short* xx, double* h, short* yy, int lx);
void	cvl_Execsd(Cvl* cvl, short* xx, double* h, double* yy, int lx);
void	cvl_Execdd(Cvl* cvl, double* xx, double* h, double* yy, int lx);
void	cvl_Execfd(Cvl* cvl, float* xx, double* h, double* yy, int lx);
void	cvl_Execff(Cvl* cvl, float* xx, double* h, float* yy, int lx);

#endif
