/*
**		ヒープ領域チェック（デバッグ）
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "hpck.h"

#ifdef HPDBG
#define  KM_fprintf(arg) fprintf arg

#ifdef PRSIZE
static int mem_total;
#endif
static HP *HeapHead = NULL;

void HpCheck(char* msg)
 /*==========================================================================*/
{
	register HP *hp;
	register unsigned char *p;
	int uflag, oflag;
	if(!msg) msg="";

	for (hp=HeapHead; hp; hp=hp->next) {
		uflag = oflag = 0;
		p = (unsigned char *)((char *)(hp + 1) + hp->size);
		if (hp->owhead != OWHEAD)
			uflag = 1;
		if (*p != OWTAIL)
			oflag = 1;
		if (uflag || oflag) {
			HpError();
			KM_fprintf((stderr,
				"HpCheck(%s, %d) : heap violation[%c%c]  %s\n",
				hp->file, hp->line, uflag ? 'U' : ' ',
				oflag ? 'O' : ' ', msg));
		}
	}
}

char *HpMalloc(size, file, line)
 /*==========================================================================*/
 	unsigned long size;
	char *file;
	int line;
{/*--------------------------------------------------------------------------*/
	HP *hp;
	unsigned char *p;

	HpCheck("HpMalloc");

	hp = (HP *)malloc(sizeof(HP) + size + 1);
	if (!hp) return(NULL);
	memset(hp, 0, sizeof(HP) + size + 1);

	hp->next = HeapHead;
	HeapHead = hp;

	hp->file = file;
	hp->line = line;
	hp->size = size;
	hp->owhead = OWHEAD;

#ifdef PRSIZE
	mem_total += size;
	KM_fprintf((stderr, "HpMalloc: %s: size= %d, total= %d\n", hp->file, hp->size, mem_total));
#endif

	hp++;

	p = (unsigned char *)((char *)hp + size);
	*p = OWTAIL;


	return((char *)hp);
}

char *HpRealloc(p, size, file, line)
 /*==========================================================================*/
	char *p;
	unsigned long size;
	char *file;
	int line;
{/*--------------------------------------------------------------------------*/
	register char *q;
	HP *cp;

	q = HpMalloc(size, file, line);
	if (!q) return(NULL);
	if (!p) return(q);

	for (cp=HeapHead; cp; cp=cp->next) {
		if ((char *)&cp[1] == p)
			break;
	}

	if (!cp) {
		HpError();
		KM_fprintf((stderr,
"HpRealloc(%s, %d): cannot find buf to free\n", file, line));
		return(NULL);
	}

	memcpy(q, p, cp->size > size ? size : cp->size);
	HpFree(p, file, line);

	return(q);
}

void HpFree(p, file, line)
 /*==========================================================================*/
	char *p;
	char *file;
	int line;
{/*--------------------------------------------------------------------------*/
	register HP *pp, *cp;

	pp = NULL;
	for (cp=HeapHead; cp; pp=cp, cp=cp->next) {
		if ((char *)&cp[1] == p) {
			if (cp->flag == FREEHEAP) {
				HpError();
				KM_fprintf((stderr,
"HpFree(%s, %d) : %s, %d already freed\n", file, line, cp->file, cp->line));
			} else {
			/*			BUG ??? 
				free(cp); 
			*/
				cp->flag = FREEHEAP;
				if (pp)
					pp->next = cp->next;
				else
					HeapHead = cp->next;
				free(cp);
			}
			return;
		}
	}
	HpError();
	KM_fprintf((stderr, "HpFree(%s, %d) : cannot free pointer\n", file, line));
}

void HpError()
 /*==========================================================================*/
 /* for dbx break point						      */
{/*--------------------------------------------------------------------------*/
	KM_fprintf((stderr, "hp: what had happend?\n"));
}

char *hp_strdup(s, file, line)
 /*==========================================================================*/
 /*--------------------------------------------------------------------------*/
	char *s;
	char *file;
	int line;
{/*--------------------------------------------------------------------------*/
	char *p = HpMalloc(strlen(s)+1, file, line);

	if (!p) return(NULL);

	strcpy(p, s);

	return(p);
}


#endif /* HPDBG */
