#ifndef moddef_h
#define moddef_h
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
#define errors(s0,s1) { fprintf(stdout,"%s\n",s0); error(s1); }
/***********************************************************************/
#define SP      	' '
#define isws(c) 	(c==9||c==10||c==13||c==' ')
#define isatomchar(c)	(isalpha(c)||isdigit(c)||c=='_')
#define iswordchar(c)	(isatomchar(c)||c=='['||c==']'||c=='.')
#define isvarchar(c)	(isatomchar(c)||c=='.')
#define isdel(c)	(!isatomchar(c))
#define isrel(c)	(c=='>'||c=='='||c=='<')
#define skipws(s)	{while (*s&&isws(*s))s++;}
/***********************************************************************/
#define isachar(s)      (islower(s)||isdigit(s)||s=='_')
#define isdchar(c)	(isalpha(c)||isachar(c)))
/***********************************************************************/
typedef struct _MODdefinition {
        char **left;
        char **right;
	int n;
} MODdefinition;
/***********************************************************************/
#include "mstring.h"
#include "wfmoddef.h"
/***********************************************************************/
#endif
