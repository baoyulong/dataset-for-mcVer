#ifndef q0cnf_h
#define q0cnf_h
/***********************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
/***********************************************************************/
#define ZWH
#define LOCAL
#define GLOBAL
/***********************************************************************/
#define error(s) 		{fprintf(stderr,"%s error\n",s); exit(0);}
/***********************************************************************/
#define infos(s) 		{fprintf(stdout,"%s\n",s);}
#define infoi(s) 		{fprintf(stdout,"%i\n",s);}
#define infof(s)                {fprintf(stdout,"%f\n",s);}
/***********************************************************************/
#define MAXN			0x00100000
#define MAXNEG			0xffefffff
#define MAXSTRING		0x04000000
#define MAXVARLEN		0x00000040
#define MAXLINELEN		0x00002000
#define MAXLINELENCD		0x00001000
#define MAXCOMLEN		0x00000100
/************************************************************************/
#include "model.h"
/************************************************************************/
typedef struct _qmdata {
        int *Clause;
        struct _qmdata *q;
        struct _qmdata *next;
} qmdata;
/***********************************************************************/
/***********************************************************************/
typedef struct _qcdata{
	int p;
	int t;
	int cl;
	int *Clause;
} qcdata;
/***********************************************************************/
typedef struct _sdata {
	int *LiteralMatrix;
	int *LL;
	int *LS;
} sdata;
/***********************************************************************/
typedef struct _qwdata{
	char *InactiveMatrix;
	int *InactiveList;
	int *Current;
	int depth;
	int *dLiterals;
	int **cur;
	int **cLength;
} qwdata;
/************************************************************************/
typedef struct _qinfo{
	int m;
	int n;
	int l;
	int *mstring;
	char *e;
	qcdata *clauseinfo;
	sdata *sd;
	qwdata *wd;
} qinfo,qmodel;
/************************************************************************/
#include "wfq0cnf.h"
#endif
