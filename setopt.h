#ifndef setopt_h
#define setopt_h
#include "datatype.h"

extern int 	setopt(int ac, char **av, char *option, char **ags);
extern int 	setopt_flg (int ac, char **av, char *op);
extern int 	setopt_ichar(int ac, char **av, char *op, int default_value);
extern int 	setopt_int (int ac, char **av, char *op, int default_value);
extern uint 	setopt_uint (int ac, char **av, char *op, uint default_value);
extern float	setopt_float(int ac, char **av, char *op, float default_value);
extern double	setopt_double(int ac, char **av, char *op, double default_value);
extern char	*setopt_char(int ac, char **av, char *op, char *default_value);
extern char	*setopt_char1(int ac, char **av, char *op, char *default_value);
extern char	*setopt_char2(int ac, char **av, char *op, char *default_1, char *default_2);

#endif
