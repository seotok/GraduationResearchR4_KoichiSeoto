#ifndef _HP_H_
#define _HP_H_

#include <stdlib.h>

#ifdef HPDBG
typedef struct _HP {
	struct _HP *next;
//	int size;
	unsigned long size;
	char *file;
	int line;
	unsigned char flag;
#define 	FREEHEAP	0xef
	unsigned char owhead;
#define 	OWHEAD		0x77
#define 	OWTAIL		0xff
	char	dummy[6];
} HP;

#define MALLOC(S)		HpMalloc(S, __FILE__, __LINE__)
#define REALLOC(P, S)	HpRealloc(P, S, __FILE__, __LINE__)
#define FREE(P)		HpFree(P, __FILE__, __LINE__)
#define STRDUP(S)		hp_strdup(S, __FILE__, __LINE__)
extern char *HpMalloc();
extern char *HpRealloc();
extern char *hp_strdup();
extern void HpCheck();
extern void HpFree();
extern void HpError();
#else
#define MALLOC	malloc
#define REALLOC	realloc
#define FREE	free
#define STRDUP	strdup
#endif /* HPDBG */

extern unsigned long next_serial();

#endif /* !_HP_H_ */
