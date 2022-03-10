#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "setopt.h"

#define	MAXSTR	(512)
#define	NARG (4096)

/* オプションflgが引き数になければ　　ー１
　オプションflgが引き数にあるのに続く引き数がなければ　　０
  その他はそのオプションに続く引き数の数を返す。
  -opt av1 av2 av3 --> return(3)
*/   

int setopt(
	int	ac,
	char	**av,
	char	*option,
	char	**ags
){
	register int i,j,n;
	char	s[MAXSTR];

#ifdef OPT_PRI_FIRST_IN 
	/***** 先入力優先 *****/
	for( i = 1; i < ac; i++ ) {
		if( !strcmp(av[i], option) ) break;
	}
	if( i == ac ){
#else
	/***** 後入力優先 *****/
	for( i = ac-1; i >= 0; i-- ) {
		if( !strcmp(av[i], option) ) break;
	}

	if( i <= 0 ){
#endif
		ags[0] = NULL;
		return(-1);
	}	
	else {
		n = 0;
		for( j = i+1; j < ac; j++ ) {
			if( !strncmp(av[j], "-", 1) ) {
				strcpy( s, av[j] );
				if( '0' <= s[1] && s[1] <= '9' ) /* 負の数の入力に対応 */
					ags[n++] = av[j];
				else
					break;

			} 
			else if( !strncmp(av[j], "", 1) ) {
				continue;
			}
			else {
				ags[n++] = av[j];
			}
		}
	}


	if( n == 0 ){
#ifdef DBG
		KM_fprintf((stderr, "no arg for %s\n", option));
#endif
		ags[0] = NULL;
		return(0);
	}
	else {
		if(n >= NARG){
			fprintf(stderr, "引数が多すぎです。\n");
		}
		return(n);
	}
}


int	setopt_flg (int ac, char **av, char *op)
{
	char    *s[NARG];
	int     n;

	n = setopt(ac, av, op, s);
	if(n>=0)
		return(1);
	else
		return(0);
}


int	setopt_ichar(int ac, char **av, char *op, int default_value)
{
	char    *s[NARG];
	int     n;

	n = setopt(ac, av, op, s);
	if(n>0)
		return(s[0][0]);
	else
		return(default_value);
}


int	setopt_int(int ac, char **av, char *op, int default_value)
{
	char    *s[NARG];
	int     n;
	n = setopt(ac, av, op, s);
	if(n>0)
		return(atoi(s[0]));
	else
		return(default_value);
}

uint setopt_uint(int ac, char **av, char *op, uint default_value)
{
	char    *s[NARG];
	int     n;
	n = setopt(ac, av, op, s);
	if(n>0)
		return (uint)atoi(s[0]);
	else
		return default_value;
}

float	setopt_float(int ac, char **av, char *op, float default_value)
{
	char    *s[NARG];
	int     n;

	n = setopt(ac, av, op, s);
	if(n>0)
		return (float)(atof(s[0]));
	else
		return(default_value);
}


double	setopt_double(int ac, char **av, char *op, double default_value)
{
	char    *s[NARG];
	int     n;

	n = setopt(ac, av, op, s);
	if(n>0)
		return (double)(atof(s[0]));
	else
		return(default_value);
}

char	*setopt_char(int ac, char **av, char *op, char *default_value)
{
	char    *s[NARG];
	int     n = setopt(ac, av, op, s);
	if(n>0)
		return(s[0]);
	else if(n==0)
		return("");
	else
		return(default_value);
}

char	*setopt_char1(int ac, char **av, char *op, char *default_value)
{
	char    *s[NARG];
	int     n = setopt(ac, av, op, s);
	if(n>0)
		return(s[0]);
	else
		return(default_value);
}

char	*setopt_char2(int ac, char **av, char *op, char *val1, char* val2)
{ // 引数キーだけのときは val2 が返る。
	char    *s[NARG];
	int     n;

	n = setopt(ac, av, op, s);
	if(n>0)
		return(s[0]);
	else if(n==0)
		return(val2);
	else
		return(val1);
}
