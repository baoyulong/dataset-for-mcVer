#ifndef c2v_h
#define c2v_h
/***********************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <sys/resource.h>
/***********************************************************************/
#define MAXKL 	16
#define MAXNL	128
#define MAXSL 	1024
#define MAXCL 	4096
#define GLOBAL
/***********************************************************************/
#define infos(s) 	{ fprintf(stdout,"%s\n",s); }
#define infoi(s) 	{ fprintf(stdout,"%i\n",s); }
#define error(ll,i) 	{ fprintf(stdout,"UNKNOWN %i %s\n",i,ll); exit(0); }
/***********************************************************************/
/*
#define error(ll,i) 	{ fprintf(stdout,"UNKNOWN STATEMENT %i\n",i ); exit(0);}
/***********************************************************************/
#define isspace3(c)	(c==' '||c=='	'||c=='\n')
#define isdel(c)	(c=='('||c==')'||c=='{'||c=='}'||c==';'||c==','||c=='=')
#define isdelsp(c)	(isspace3(c)||isdel(c))
/***********************************************************************/
typedef struct _stm {
	char key[MAXKL];
	char str[MAXSL];
	struct _stm *left;
	struct _stm *next;
} stm;
/***********************************************************************/
typedef struct _par {
	char type[MAXKL];
	char name[MAXNL];
	char init[MAXSL];
	struct _par *next;
} par;
/***********************************************************************/
typedef struct _fun {
	char type[MAXKL];
	char name[MAXNL];
	par *p0;
	par *v0;
	par *sp;
	struct _stm *s1;
	struct _fun *next;
} fun;
/***********************************************************************/
#include "wfc2v.h"
#endif
