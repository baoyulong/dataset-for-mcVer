#ifndef s1rd_h
#define s1rd_h
/***********************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
/***********************************************************************/
#define LOCAL
#define GLOBAL
/*******************************************************************/
#define infos(s) 		{fprintf(stdout,"%s\n",s);}
#define infoi(s) 		{fprintf(stdout,"%i\n",s);}
#define infof(s) 		{fprintf(stdout,"%f\n",s);}
#define error(s) 		{fprintf(stderr,"%s error\n",s); exit(0);}
/***********************************************************************/
typedef struct _model { int n; int m; int l; int *mstring; } model;
/***********************************************************************/
typedef struct _mstruct {
        int **Clause;
        int *CL; 
        int *LiteralMatrix;
        int *LS;
        int *LL;
} mstruct;
/***********************************************************************/
typedef struct _mdata {
        char *InactiveMatrix;
        int *InactiveList;
        int *Literals;
        int *Current;
} mdata;
/***********************************************************************/
#include "wfs1rd.h"
#endif
