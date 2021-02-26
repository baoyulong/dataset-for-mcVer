#ifndef prg_h
#define prg_h
/***********************************************************************/
#include "treetype.h"
/***********************************************************************/
typedef struct _process {
	int n;
        tree **tslist;
        tree **tslistpc;
} process;
/***********************************************************************/
typedef struct _prg {
	tree *vlist;
	tree *prog;
	int booln;
	int prscnt;
	process **prs;
	tree *deadlocktr;
	int puretrans;
	tree *varconstr;
	tree *deadlockfreecond;
} prg;
/*******************************************************************/
#endif
