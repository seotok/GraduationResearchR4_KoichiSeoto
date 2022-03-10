#ifndef opt_h
#define opt_h

#include "stdio.h"
#include "setopt.h"

#define	OPTION_FLG		(0)
#define	OPTION_CHAR		(1)
#define	OPTION_INT		(2)
#define	OPTION_FLOAT	(3)
#define	OPTION_MCHAR	(4)
#define	OPTION_MINT		(5)
#define	OPTION_DOUBLE	(6)
#define	OPTION_MFLOAT	(7)
#define	OPTION_MDOUBLE	(8)
#define	OPTION_NOP		(9)
#define	OPTION_ICHAR	(10)
#define	OPTION_UINT		(11)
#define	MAX_OPTION		(4096)

typedef struct Opt {
	int	ac;
	char	**av;
	int	n;
	char	*title; 	/* タイトル名（モジュール名）*/
	struct OptTab {
		char*	name;
		char*	cmt;
		int		kind;
		int		flg_val;
		char*	c_val;
		int		i_val;
		uint	ui_val;
		float	f_val;
		double	d_val;
		int		nin;
		char**	mc_val;
		int*	mi_val;
		float*	mf_val;
		double*	md_val;
	} *otb[MAX_OPTION];
} Opt;

extern Opt*   opt_open    (int ac, char **av, char *title);
extern void   opt_close   (Opt *op);
extern void   opt_pr      (Opt *op);
extern void   opt_prfp    (Opt *op, FILE* fp);
extern void   opt_help    (Opt *op, FILE* fp);
extern int    opt_flg     (Opt *op, char *name, char* cmt);
extern int    opt_ichar   (Opt *op, char *name, int dflt, char* cmt);
extern int    opt_int     (Opt *op, char *name, int dflt, char* cmt);
extern uint   opt_uint    (Opt *op, char *name, uint dflt, char* cmt);
extern float  opt_float   (Opt *op, char *name, float  dflt, char* cmt);
extern float  opt_float2  (Opt *op, char *name, float  dflt_off, float dflt_on, char* cmt);
extern double opt_double  (Opt *op, char *name, double dflt, char* cmt);
extern char*  opt_char    (Opt *op, char *name, char*  dflt, char* cmt);
extern char*  opt_char1   (Opt *op, char *name, char*  dflt, char* cmt);
extern char*  opt_char2   (Opt *op, char *name, char *dflt1, char* dflt2, char* cmt);
extern char** opt_mchar   (Opt *op, char *name, char*  dflt, int* nopt, char* cmt);
extern int*   opt_mint_org(Opt *op, char *name, int*   dflt, int* nopt, char* cmt);
extern int*   opt_mint    (Opt *op, char *name, int*   dflt, int ndflt, int* nopt, char* cmt);
extern int*   opt_mint2   (Opt *op, char *name, int    dflt, int ndflt, int* nopt, char* cmt);
extern float* opt_mfloat  (Opt *op, char *name, float  dflt, int ndflt, int* nopt, char* cmt);
extern double* opt_mdouble (Opt *op, char *name, double dflt, int ndflt, int* nopt, char* cmt);
extern double* opt_mdouble2(Opt *op, char *name, double* dflt, int ndflt, char* cmt);
extern double* opt_mdouble3(Opt *op, char *name, double* dflt, int ndflt, int* nopt, char* cmt);
extern void    opt_nop     (Opt *op, char *name, char* cmt);


#endif
