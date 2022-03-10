
#include <stdio.h>
#include <string.h>
#include "hpck.h"
#include "opt.h"
#include "optck.h"

static int	nopt;
static Opt*	optlist[MAX_OPTION];

void optck_entry(Opt* op)
{
	optlist[nopt] = op;
	nopt ++;
	if(nopt >= MAX_OPTION){
		fprintf(stderr, "オプションが多すぎます。\n");
	}
}

int optck_key(Opt* op, char* key, int kind)
{
	int	n, k, times=0;

	for(n=0; n<nopt; n++){			/* 全エントリー中のOPTモジュールの番号 */
		if(!optlist[n]) continue;	/* opt_close()したOPTは除く */
		for(k=0; k<optlist[n]->n; k++){	/* n番目のOPTモジュール中のkey番号 */
			if(key == optlist[n]->otb[k]->name) /* 自分を２回数えない */
				continue;

			if(!strcmp(optlist[n]->otb[k]->name, key) // 同じkey
			&& strcmp(op->title, optlist[n]->title) // 違うモジュール
			&& kind == optlist[n]->otb[k]->kind){	// 同じデータ形式
				times ++;
			}
			if(times > 0){
				fprintf(stderr, "同じオプションキーがあります。(%s : %s %s)\n", 
				key, optlist[n]->title, op->title);
				return 1;
			}
		}
	}

	return 0;
}

int optck_title(char* title)
{
	int	i, times=0;

	for(i=0; i<nopt; i++){
		if(!optlist[i]) continue;	/* opt_close()したOPTは除く */
		if(!optlist[i]->title) continue;	/* */
		if(optlist[i]->title == title) 	/* 自分を２回数えない */
			continue;
		if(!strcmp(optlist[i]->title, title)){
			times ++;
		}
		if(times > 0){
			return 1;
		}
	}
	return 0;
}

void optck_delete(Opt* op)
{
	int	i;

	for(i=0; i<nopt; i++){
		if(optlist[i] == op)
			optlist[i] = NULL;
	}
}
