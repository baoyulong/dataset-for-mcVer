#ifndef ce_h
#define ce_h
/***********************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
/***********************************************************************/
#define MAXVARLEN		64
/***********************************************************************/
#define infos(s) 		{fprintf(stdout,"%s\n",s);}
#define infoi(s) 		{fprintf(stdout,"%i\n",s);}
#define infof(s) 		{fprintf(stdout,"%f\n",s);}
#define error(s) 		{fprintf(stdout,"error: %s\n",s); exit(0);}
/***********************************************************************/
typedef struct _vinfo { 
	char name[MAXVARLEN];
	int low;
	int high;
	int value;
	int max;
} vinfo;
/***********************************************************************/
#include "wfce.h"
/***********************************************************************/
#endif
