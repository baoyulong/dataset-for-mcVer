#ifndef typetbd_h
#define typetbd_h
/************************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <treetype.h>
/************************************************************************/
#define CEXLEN 		0x00004000
/***********************************************************************/
typedef struct _TBD 	{ int x,y; struct _TBD *a,*b,*c,*n; } TBD; 
typedef struct _TBDmm 	{ int tablesz,nodes,cap; TBD **table; } TBDmm;
/***********************************************************************/
typedef struct _TRtbd 	{ TBD *p,*t,**tt; TBD *tc; char *uclist; } TRtbd;
typedef struct _TPtype 	{ \
	int n; \
	TBD *i;
	TBD **fw1,**bw1; \
	TRtbd **fw2,**bw2; \
	TBD **f; \
	char **v0,**v1; \
	int *vval; char **vname; \
	TBD *fair; \
} TPtype; \
/***********************************************************************/
typedef struct _BPtype 	{ int n; tree *i,*t,*f,*p; char **v1; } BPtype;
/***********************************************************************/
typedef struct _CEXtype	{ TBD *fw[CEXLEN],*bw[CEXLEN]; int cf,cb; } CEXtype;
/***********************************************************************/
#define LOCAL
#define GLOBAL
/***********************************************************************/
#define error(s)  	{fprintf(stderr,"%s error\n",s); exit(0);}
/***********************************************************************/
#define iinfos(s)       {}
#define iinfoi(s)       {}
#define iinfof(s)       {}
/***********************************************************************/
#define infos(s)        {fprintf(stdout,"%s\n",s);}
#define infoi(s)        {fprintf(stdout,"%i\n",s);}
#define infof(s)        {fprintf(stdout,"%f\n",s);}
/***********************************************************************/
#define infot() \
        printf("TIME %f,%i\n",(float)utiltime(1),(int)difftime(time(&t1),t0))\
/************************************************************************/
extern FILE *logtbd;
/***********************************************************************/
#define logs(s)       	{loginit(); fprintf(logtbd,"%s\n",s);}
#define logi(s)       	{loginit(); fprintf(logtbd,"%i\n",s);}
#define logf(s)       	{loginit(); fprintf(logtbd,"%f\n",s);}
#define loginit()  	{if (logtbd==NULL) logtbd=fopen("logtbd.tmp","w");}
#define logwarning(s)  	{loginit(); fprintf(logtbd,"WARNING:%s\n",s);}
#define logexit(i)  	{logi(i);fclose(logtbd);exit(0);}
/***********************************************************************/
typedef struct _TBDtrs {
        tree *pc;
        TBD *p;
        TBD *t;
        TBD *pt;
        TBD *tc;
        char *uclist;
        TBD *bp;
        TBD *bt;
        TBD *bpt;
        TBD **btt;
} TBDtrs;
/*******************************************************************/
typedef struct _TBDprg {
        char **v0,**v1;
        int *vval;
        char **vname;
        int booln;
        TBD *i;
        TBD **fairlist;
        int trscnt;
        TBDtrs **trs;
	tree *vlist;
	int puretrans;
	CEXtype *cex;
} TBDprg;
/*******************************************************************/
typedef struct _TBDtrspo {
        int trok;
        TBD *p;
} TBDtrspo;
/*******************************************************************/
#endif
