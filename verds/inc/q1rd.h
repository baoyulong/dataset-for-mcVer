#ifndef q1rd_h
#define q1rd_h
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
#include "q0cnf.h"
/************************************************************************/
#include "wfq0cnf.h"
#include "wfq1rd.h"
#endif
