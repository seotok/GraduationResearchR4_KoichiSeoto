/*
*/
#include <stdio.h>
#include "string.h"
#include "setopt.h"
#include "opt.h"
#include "hpck.h"
#include "optck.h"

#define OP_NAME_LEN (16)
#define OP_DATA_LEN (7)
#define TITLE_LEN (15)
#define KM_fprintf(arg) fprintf arg

static char* zero_del(double f, int max)
{
	int i;
	static char ss[128];

	sprintf(ss, "%f", f);
	for(i=strlen(ss)-1; i>=0; i--){
		if(ss[i] == '0'){
			ss[i] = ' ';
		}
		else if(ss[i] == '.'){
			ss[i+1] = '0';
			break;
		}
		else {
			break;
		}
	}

	ss[max] = '\0';
	return(ss);
}

Opt* opt_open (int ac, char **av, char *title)
{
	Opt	*op;

	op = (Opt*)MALLOC(sizeof(Opt));

	op->ac = ac;
	op->av = av;
	op->n = 0;
	op->title = title;

	optck_entry(op);

	return(op);
}

void opt_close(Opt* op)
{
	int i;

	for(i=0; i<op->n; i++){
		if(op->otb[i]->mc_val)
			FREE(op->otb[i]->mc_val);
		if(op->otb[i]->mi_val)
			FREE(op->otb[i]->mi_val);
		if(op->otb[i]->mf_val)
			FREE(op->otb[i]->mf_val);
		if(op->otb[i]->md_val)
			FREE(op->otb[i]->md_val);
		if(op->otb[i]->name)
			FREE(op->otb[i]->name);
		FREE(op->otb[i]);
	}

	optck_delete(op);

	FREE(op);
}

void opt_help (Opt *op, FILE* fp)
{
	opt_prfp(op, fp);
}

static void opt_mprint(FILE* fp, int n, char* ts, char* key, char* val, char* cmt)
{ 
	if(n==0)
		KM_fprintf((fp, "%s%-15s%-8s   %s\n", ts, key, val, cmt));
	else
		KM_fprintf((fp, "%s%-15s%-8s\n", ts, key, val));
}

void opt_pr (Opt *op)
{
	opt_prfp(op, stdout);
}

void opt_prfp (Opt *op, FILE* fp)
{
	int	i, n;
	char title[TITLE_LEN];
	char space[TITLE_LEN];

	if(!op) return;

	/* 同じオプションタイトル名のオプションは表示しない */
	if(optck_title(op->title)) return;	

	int tl = strlen(op->title) < TITLE_LEN ? strlen(op->title) : TITLE_LEN;
	for(i=0; i<tl; i++) title[i] = op->title[i];
	for(; i<TITLE_LEN-1; i++) title[i] = ' ';
	title[TITLE_LEN-1] = '\0';

	for(i=0; i<TITLE_LEN-1; i++) space[i] = ' ';
	space[TITLE_LEN-1] = '\0';

	for(i=0; i<op->n; i++){
		char* cv;
		char* p = op->otb[i]->name;
		char* cmt = op->otb[i]->cmt;
		char* ts = i==0 ? title : space;
		switch(op->otb[i]->kind){
			case OPTION_FLG:
				if(cmt){ // 説明がNULLのときは表示されない
					if(op->otb[i]->flg_val)
						KM_fprintf((fp, "%s%-15s-有効-     %s\n", ts, p, cmt));
					else
						KM_fprintf((fp, "%s%-15s .         %s\n", ts, p, cmt));
				}
				break;
			case OPTION_CHAR:
				cv = op->otb[i]->c_val;
				if(!cv) cv = "NULL";
				KM_fprintf((fp, "%s%-15s%-8s   %s\n", ts, p, cv, cmt));
				break;
			case OPTION_ICHAR:
				KM_fprintf((fp, "%s%-15s%-8c   %s\n", ts, p, op->otb[i]->i_val, cmt));
				break;
			case OPTION_INT:
				KM_fprintf((fp, "%s%-15s%-8d   %s\n", ts, p, op->otb[i]->i_val, cmt));
				break;
			case OPTION_UINT:
				KM_fprintf((fp, "%s%-15s%-8d   %s\n", ts, p, op->otb[i]->ui_val, cmt));
				break;
			case OPTION_FLOAT:
				cv = zero_del((double)op->otb[i]->f_val, 8);
				KM_fprintf((fp, "%s%-15s%-8s   %s\n", ts, p, cv, cmt));
				break;
			case OPTION_DOUBLE:
				cv = zero_del(op->otb[i]->d_val, 8);
				KM_fprintf((fp, "%s%-15s%-8s   %s\n", ts, p, cv, cmt));
				break;
			case OPTION_MCHAR:
				if(op->otb[i]->nin <= 0){
					KM_fprintf((fp, "%s%-15s%-8s   %s\n", ts, p, "NULL", cmt));
				}
				else {
					for(n=0; n<op->otb[i]->nin; n++){
						char* ts = (i==0 && n==0) ? title : space;
						char* p = n==0 ? op->otb[i]->name : space;
						char* c = op->otb[i]->mc_val[n];
						if(!c) c = "NULL";
						opt_mprint(fp, n, ts, p, c, cmt);
					}
				}
				break;
			case OPTION_MINT:
				if(op->otb[i]->nin <= 0){
					KM_fprintf((fp, "%s%-15s%-8s   %s\n", ts, p, "NULL", cmt));
				}
				else {
					for(n=0; n<op->otb[i]->nin; n++){
						char* ts = (i==0 && n==0) ? title : space;
						char* p = n==0 ? op->otb[i]->name : space;
						int k = op->otb[i]->mi_val[n];
						KM_fprintf((fp, "%s%-15s%-8d   %s\n", ts, p, k, cmt));
					}
				}
				break;
			case OPTION_MFLOAT:
				if(op->otb[i]->nin <= 0){
					KM_fprintf((fp, "%s%-15s%-8s   %s\n", ts, p, "NULL", cmt));
				}
				else {
					for(n=0; n<op->otb[i]->nin; n++){
						char* ts = (i==0 && n==0) ? title : space;
						char* p = n==0 ? op->otb[i]->name : space;
						char* c = zero_del((double)op->otb[i]->mf_val[n], 8);
						opt_mprint(fp, n, ts, p, c, cmt);
					}
				}
				break;
			case OPTION_MDOUBLE:
				if(op->otb[i]->nin <= 0){
					KM_fprintf((fp, "%s%-15s%-8s   %s\n", ts, p, "NULL", cmt));
				}
				else {
					for(n=0; n<op->otb[i]->nin; n++){
						char* ts = (i==0 && n==0) ? title : space;
						char* p = n==0 ? op->otb[i]->name : space;
						char* c = zero_del((double)op->otb[i]->md_val[n], 8);
						opt_mprint(fp, n, ts, p, c, cmt);
					}
				}
				break;
			case OPTION_NOP:
				KM_fprintf((fp, "%s%-15s%-8s\n", ts, p, cmt));
				break;
			default:
				break;
		}
	}
}

void opt_nop (Opt* op, char* name, char* cmt)
{
	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_NOP;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_flg: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);
}


int opt_flg (Opt* op, char* name, char* cmt)
{
	if(!name) return 0;
	int flg = setopt_flg(op->ac, op->av, name);
	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_FLG;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->flg_val = flg;
	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_flg: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(flg);
}


int	opt_ichar (Opt* op, char* name, int default_value, char* cmt)
{
	if(!name) return default_value;
	int val = setopt_ichar(op->ac, op->av, name, default_value);
	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_ICHAR;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->i_val = val;
	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_int: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(val);
}


int	opt_int (Opt* op, char* name, int default_value, char* cmt)
{
	if(!name) return default_value;
	int val = setopt_int(op->ac, op->av, name, default_value);
	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_INT;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->i_val = val;
	op->n ++;
	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_int: Number of option exceeded\n"));
	}
	optck_key(op, op->otb[n]->name, op->otb[n]->kind);
	return(val);
}

uint opt_uint (Opt* op, char* name, uint default_value, char* cmt)
{
	if(!name) return default_value;
	uint val = setopt_uint(op->ac, op->av, name, default_value);
	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_UINT;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->ui_val = val;
	op->n ++;
	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_int: Number of option exceeded\n"));
	}
	optck_key(op, op->otb[n]->name, op->otb[n]->kind);
	return val ;
}

float opt_float (Opt* op, char* name, float default_value, char* cmt)
{
	if(!name) return default_value;
	float val = setopt_float(op->ac, op->av, name, default_value);
	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_FLOAT;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->f_val = val;
	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_float: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(val);
}


float opt_float2 (Opt* op, char* name, float default_off, float default_on, char* cmt)
{
	float   val;
	char	*s;

	if(!name) return default_off;

	int narg = setopt(op->ac, op->av, name, &s);

	if(narg < 0) 
		val = default_off;
	else if(narg == 0)
		val = default_on;
	else 
		val = atof(s);

	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_FLOAT;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->f_val = val;
	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_float: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(val);
}


double opt_double (Opt* op, char* name, double default_value, char* cmt)
{
	if(!name) return default_value;

	double val = setopt_double(op->ac, op->av, name, default_value);
	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_DOUBLE;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->d_val = val;
	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_float: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(val);
}

char *opt_char (Opt *op, char *name, char *default_value, char* cmt)
{
	if(!name) return default_value;

	char* val = setopt_char(op->ac, op->av, name, default_value);
	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_CHAR;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->c_val = val;
	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_char: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(val);
}

char *opt_char1 (Opt *op, char *name, char *default_value, char* cmt)
{
	if(!name) return default_value;

	char* val = setopt_char1(op->ac, op->av, name, default_value);
	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_CHAR;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->c_val = val;
	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_char: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(val);
}

char *opt_char2 (Opt *op, char *name, char *default_val1, char* default_val2, char* cmt)
{
	if(!name) return default_val1;

	char* val = setopt_char2(op->ac, op->av, name, default_val1, default_val2);
	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_CHAR;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->c_val = val;
	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_char: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(val);
}


char** opt_mchar (Opt *op, char *name, char *dflt, int *nopt, char* cmt)
{
	if(!name) return NULL;

	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_MCHAR;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->c_val = NULL;
	op->otb[n]->mc_val = (char**)MALLOC(sizeof(char*)*op->ac);

	*nopt = setopt(op->ac, op->av, name, op->otb[n]->mc_val);
	if(*nopt <= 0 && dflt){
		op->otb[n]->mc_val[0] = dflt;
		*nopt = 1;
	}
	op->otb[n]->nin = *nopt;

	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_char: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(op->otb[n]->mc_val);
}


int* opt_mint_org (Opt *op, char *name, int* dflt, int* nopt, char* cmt)
{
	int     i;
	char*	s[MAX_OPTION];

	if(!name) return NULL;

	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_MINT;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->i_val = 0;

	*nopt = setopt(op->ac, op->av, name, s);
	if(*nopt <= 0 && dflt){
		*nopt = 1;
		op->otb[n]->mi_val = (int*)MALLOC(sizeof(int)*(*nopt));
		op->otb[n]->mi_val[0] = dflt[0];
	}
	else {
		op->otb[n]->mi_val = (int*)MALLOC(sizeof(int)*(*nopt));
		for(i=0; i<*nopt; i++)
			op->otb[n]->mi_val[i] = atoi(s[i]);
	}
	op->otb[n]->nin = *nopt;

	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_mint: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(op->otb[n]->mi_val);
}


int* opt_mint (Opt *op, char *name, int* dflt, int ndflt, int* nopt, char* cmt)
{
	int     i;
	char*	s[MAX_OPTION];

	if(!name) return NULL;

	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_MINT;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->i_val = 0;

	*nopt = setopt(op->ac, op->av, name, s);
	if(*nopt <= 0 && (!dflt||!ndflt)){
		op->otb[n]->mi_val = NULL;
	}
	else if(*nopt <= 0 && dflt){
		if(ndflt >= 1)
			*nopt = ndflt;
		else
			*nopt = 1;
		op->otb[n]->mi_val = (int*)MALLOC(sizeof(int)*(*nopt));
		for(i=0; i<*nopt; i++)
			op->otb[n]->mi_val[i] = dflt[i];
	}
	else {
		op->otb[n]->mi_val = (int*)MALLOC(sizeof(int)*(*nopt));
		for(i=0; i<*nopt; i++)
			op->otb[n]->mi_val[i] = atoi(s[i]);
	}
	op->otb[n]->nin = *nopt;

	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_mint: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(op->otb[n]->mi_val);
}


int* opt_mint2 (Opt *op, char *name, int dflt, int ndflt, int* nopt, char* cmt)
{
	char*	s[MAX_OPTION];

	if(!name) return NULL;
	int n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_MINT;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->i_val = 0;

	int nset = setopt(op->ac, op->av, name, s);
	if(nset <= 0 && ndflt<=0){
		op->otb[n]->mi_val = NULL;
		*nopt = 0;
	}
	else {
		int i, k = nset < ndflt ? ndflt : nset;
		op->otb[n]->mi_val = (int*)MALLOC(sizeof(int)*k);
		for(i=0; i<nset; i++)
			op->otb[n]->mi_val[i] = atoi(s[i]);
		for(; i<k; i++)
			op->otb[n]->mi_val[i] = dflt;
		*nopt = k;
	}
	op->otb[n]->nin = *nopt;

	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_mint2: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(op->otb[n]->mi_val);
}


float* opt_mfloat (Opt *op, char *name, float dflt, int ndflt, int* nopt, char* cmt)
{
	int     n, i, nset, k;
	char*	s[MAX_OPTION];

	if(!name) return NULL;

	n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_MFLOAT;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->i_val = 0;

	nset = setopt(op->ac, op->av, name, s);
	if(nset <= 0 && ndflt<=0){
		op->otb[n]->mf_val = NULL;
		*nopt = 0;
	}
	else {
		k = nset < ndflt ? ndflt : nset;
		op->otb[n]->mf_val = (float*)MALLOC(sizeof(float)*k);
		for(i=0; i<nset; i++)
			op->otb[n]->mf_val[i] = atof(s[i]);
		for(; i<k; i++)
			op->otb[n]->mf_val[i] = dflt;
		*nopt = k;
	}
	op->otb[n]->nin = *nopt;

	op->n ++;

	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_mfloat: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(op->otb[n]->mf_val);
}


double* opt_mdouble (Opt *op, char *name, double dflt, int ndflt, int* nopt, char* cmt)
{
	int     n, i, nset, k;
	char*	s[MAX_OPTION];

	if(!name) return NULL;

	n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_MDOUBLE;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->i_val = 0;

	nset = setopt(op->ac, op->av, name, s);
	if(nset <= 0 && ndflt<=0){
		// 引数の数(-keyだけまたはkeyなし)もデフォルト値の数も０ならNULLが返る
		op->otb[n]->md_val = NULL;
		*nopt = 0;
	}
	else {
		k = nset < ndflt ? ndflt : nset;
		op->otb[n]->md_val = (double*)MALLOC(sizeof(double)*k);
		for(i=0; i<nset; i++)
			op->otb[n]->md_val[i] = atof(s[i]);
		for(; i<k; i++)
			op->otb[n]->md_val[i] = dflt;
		*nopt = k;
	}
	op->otb[n]->nin = *nopt;

	op->n ++;
	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_mfloat: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(op->otb[n]->md_val);
}


/*
**	数値の個数は固定(ndflt)、デフォルト値はすべて指定
*/
double* opt_mdouble2 (Opt *op, char *name, double* dflt, int ndflt, char* cmt)
{
	int     n, i, nset, nopt=0;
	char*	s[MAX_OPTION];

	if(!name) return NULL;

	n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_MDOUBLE;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->i_val = 0;

	nset = setopt(op->ac, op->av, name, s);
//	if(nset <= 0 && ndflt<=0){
	if(nset < 0){
		op->otb[n]->md_val = NULL;
		nopt = 0;
	}
	else {
		nopt = nset < ndflt ? ndflt : nset;
		op->otb[n]->md_val = (double*)MALLOC(sizeof(double)*nopt);
		for(i=0; i<nset; i++)
			op->otb[n]->md_val[i] = atof(s[i]);
		for(; i<ndflt; i++) // 指定不足分はデフォルト値
			op->otb[n]->md_val[i] = dflt[i];
	}
	op->otb[n]->nin = nopt;

	op->n ++;
	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_mfloat: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(op->otb[n]->md_val);
}

// 複数デフォルト値でデータ数は入力数
double* opt_mdouble3 (Opt *op, char *name, double* dflt, int ndflt, int* nopt, char* cmt)
{
	int     n, i, nset;
	char*	s[MAX_OPTION];

	if(!name) return NULL;

	n = op->n;
	op->otb[n] = (struct OptTab*)MALLOC(sizeof(struct OptTab));
	memset(op->otb[n], 0, sizeof(struct OptTab));
	op->otb[n]->kind = OPTION_MDOUBLE;
	op->otb[n]->name = (char*)MALLOC(sizeof(char)*(strlen(name)+1));
	strcpy(op->otb[n]->name, name);
	op->otb[n]->cmt = cmt;
	op->otb[n]->i_val = 0;

	nset = setopt(op->ac, op->av, name, s);
	if(nset <= 0 && ndflt<=0){
		// 引数の数(-keyだけまたはkeyなし)もデフォルト値の数も０ならNULLが返る
		op->otb[n]->md_val = NULL;
		*nopt = 0;
	}
	else {
		int k;
		*nopt = k = 0 < nset ? nset : ndflt;
		op->otb[n]->md_val = (double*)MALLOC(sizeof(double)*k);
		for(i=0; i<nset; i++)
			op->otb[n]->md_val[i] = atof(s[i]);
		for(; i<k; i++)
			op->otb[n]->md_val[i] = dflt[i];
	}
	op->otb[n]->nin = *nopt;

	op->n ++;
	if(op->n >= MAX_OPTION){
		KM_fprintf((stderr, "opt_mfloat: Number of option exceeded\n"));
	}

	optck_key(op, op->otb[n]->name, op->otb[n]->kind);

	return(op->otb[n]->md_val);
}
