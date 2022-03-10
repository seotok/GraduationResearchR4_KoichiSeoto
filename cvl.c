#include <stdio.h>
#include <string.h>
#include "hpck.h"
#include "cvl.h"

static void cvl_Execss_(Cvl* cvl, short* xx, double* h, short* yy, int lx);
static void cvl_Execsd_(Cvl* cvl, short* xx, double* h, double* yy, int lx);
static void cvl_Execdd_(Cvl* cvl, double* xx, double* h, double* yy, int lx);
static void cvl_Execfd_(Cvl* cvl, float* xx, double* h, double* yy, int lx);
static void cvl_Execff_(Cvl* cvl, float* xx, double* h, float* yy, int lx);

static void pr_er(char *msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(0);
}

Cvl	*cvl_Open(int lh, int blk)
{
	Cvl *cvl = (Cvl*)MALLOC(sizeof(Cvl));
	cvl->buf = (double*)MALLOC(sizeof(double)*(lh+blk));
	cvl->lh = lh;
	cvl->blk = blk;
	cvl_Clear(cvl);
	return(cvl);
}

void cvl_Close(Cvl *cvl)
{
	FREE(cvl->buf);
	FREE(cvl);
}

void cvl_Clear(Cvl *cvl)
{
	memset(cvl->buf, 0, sizeof(double)*(cvl->lh+cvl->blk));
}

void cvl_Execss(Cvl* cvl, short* xx, double* h, short* yy, int lx)
{
	while(lx > cvl->blk){
		cvl_Execss_(cvl, xx, h, yy, cvl->blk);
		lx -= cvl->blk;
		xx += cvl->blk;
		yy += cvl->blk;
	}
	if(lx > 0) cvl_Execss_(cvl, xx, h, yy, lx);
}

void cvl_Execsd(Cvl* cvl, short* xx, double* h, double* yy, int lx)
{
	while(lx > cvl->blk){
		cvl_Execsd_(cvl, xx, h, yy, cvl->blk);
		lx -= cvl->blk;
		xx += cvl->blk;
		yy += cvl->blk;
	}
	if(lx > 0) cvl_Execsd_(cvl, xx, h, yy, lx);
}

void cvl_Execdd(Cvl* cvl, double* xx, double* h, double* yy, int lx)
{
	while(lx > cvl->blk){
		cvl_Execdd_(cvl, xx, h, yy, cvl->blk);
		lx -= cvl->blk;
		xx += cvl->blk;
		yy += cvl->blk;
	}
	if(lx > 0) cvl_Execdd_(cvl, xx, h, yy, lx);
}

void cvl_Execff(Cvl* cvl, float* xx, double* h, float* yy, int lx)
{
	while(lx > cvl->blk){
		cvl_Execff_(cvl, xx, h, yy, cvl->blk);
		lx -= cvl->blk;
		xx += cvl->blk;
		yy += cvl->blk;
	}
	if(lx > 0) cvl_Execff_(cvl, xx, h, yy, lx);
}

void cvl_Execfd(Cvl* cvl, float* xx, double* h, double* yy, int lx)
{
	while(lx > cvl->blk){
		cvl_Execfd_(cvl, xx, h, yy, cvl->blk);
		lx -= cvl->blk;
		xx += cvl->blk;
		yy += cvl->blk;
	}
	if(lx > 0) cvl_Execfd_(cvl, xx, h, yy, lx);
}

//*************************************************************************

static void cvl_Execss_(Cvl* cvl, short* xx, double* h, short* yy, int lx)
{
	int	j, i;

	if(lx > cvl->blk)
		pr_er("cvl_Execsd_: 入力データサイズがブロック長より大きいです。\n");

	for(i=0; i<lx; i++)
		cvl->buf[cvl->lh+i] = xx[i];

	double* x = &cvl->buf[cvl->lh];
	for(j=0; j<lx ;j++){
		double y = 0.0;
		for(i=0; i<cvl->lh ;i++)
			y += x[j-i] * h[i]; 
		yy[j] = y;
	}
	memmove(cvl->buf, &cvl->buf[lx], sizeof(double)*cvl->lh);
}

static void cvl_Execsd_(Cvl* cvl, short* xx, double* h, double* yy, int lx)
{
	int	j, i;

	if(lx > cvl->blk)
		pr_er("cvl_Execsd_: 入力データサイズがブロック長より大きいです。\n");

	for(i=0; i<lx; i++)
		cvl->buf[cvl->lh+i] = xx[i];

	double* x = &cvl->buf[cvl->lh];
	for(j=0; j<lx ;j++){
		yy[j] = 0.0;
		for(i=0; i<cvl->lh ;i++)
			yy[j] += x[j-i] * h[i]; 
	}
	memmove(cvl->buf, &cvl->buf[lx], sizeof(double)*cvl->lh);
}

static void cvl_Execdd_(Cvl* cvl, double* xx, double* h, double* yy, int lx)
{
	int	j, i;

	if(lx > cvl->blk)
		pr_er("cvl_Execdd_: 入力データサイズがブロック長より大きいです。\n");

	for(i=0; i<lx; i++)
		cvl->buf[cvl->lh+i] = xx[i];

	double* x = &cvl->buf[cvl->lh];
	for(j=0; j<lx ;j++){
		yy[j] = 0.0;
		for(i=0; i<cvl->lh ;i++)
			yy[j] += x[j-i] * h[i]; 
	}
	memmove(cvl->buf, &cvl->buf[lx], sizeof(double)*cvl->lh);
}

static void cvl_Execfd_(Cvl* cvl, float* xx, double* h, double* yy, int lx)
{
	int	j, i;

	if(lx > cvl->blk)
		pr_er("cvl_Execfd_: 入力データサイズがブロック長より大きいです。\n");

	for(i=0; i<lx; i++)
		cvl->buf[cvl->lh+i] = xx[i];

	double* x = &cvl->buf[cvl->lh];
	for(j=0; j<lx ;j++){
		yy[j] = 0.0;
		for(i=0; i<cvl->lh ;i++)
			yy[j] += x[j-i] * h[i]; 
	}
	memmove(cvl->buf, &cvl->buf[lx], sizeof(double)*cvl->lh);
}

static void cvl_Execff_(Cvl* cvl, float* xx, double* h, float* yy, int lx)
{
	int	j, i;

	if(lx > cvl->blk)
		pr_er("cvl_Execff_: 入力データサイズがブロック長より大きいです。\n");

	for(i=0; i<lx; i++)
		cvl->buf[cvl->lh+i] = xx[i];

	double* x = &cvl->buf[cvl->lh];
	for(j=0; j<lx ;j++){
		yy[j] = 0.0;
		for(i=0; i<cvl->lh ;i++)
			yy[j] += x[j-i] * h[i]; 
	}
	memmove(cvl->buf, &cvl->buf[lx], sizeof(double)*cvl->lh);
}
