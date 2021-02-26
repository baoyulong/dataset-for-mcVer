#ifndef basics_h
#define basics_h
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
/***********************************************************************/
#define logtmpinit() 		{ fclose(fopen("logtmp.tmp","w")); }
/***********************************************************************/
#define logtmp(s0,s1) {\
	if (strlen(s1)<0x1000) { \
		g=fopen("logtmp.tmp","a"); fprintf(g,"%s\n%s\n",s0,s1); \
		fclose(g); \
	}}\
/***********************************************************************/
#define logtmpi(s0,i) {\
		g=fopen("logtmp.tmp","a"); fprintf(g,"%s: %s\n",s0,i); \
		fclose(g); \
	}\
/***********************************************************************/
#define logtmptree(s0,t0) {\
		g=fopen("logtmp.tmp","a"); \
        	fprintf(g,"%s\n",s0); treewrite(t0,g); fprintf(g,"\n",s0); \
		fclose(g); \
	}\
/***********************************************************************/
#define error(s) 		{fprintf(stderr,"%s error\n",s); exit(0);}
#define pprts(s) 		{fprintf(stdout,"%s\n",s);}
#define pprti(s) 		{fprintf(stdout,"%i\n",s);}
/***********************************************************************/
#define infos(s) 		{fprintf(stdout,"%s\n",s);}
#define infoi(s) 		{fprintf(stdout,"%i\n",s);}
#define infof(s) 		{fprintf(stdout,"%f\n",s);}
#define infosi(s,i) 		{fprintf(stdout,"%s:%i\n",s,i);}
/***********************************************************************/
#define iinfos(s) 		{}
#define iinfoi(s) 		{}
/***********************************************************************/
#define infot()	\
	printf("TIME %f,%i\n",(float)utiltime(1),(int)difftime(time(&t1),t0))\
/***********************************************************************/
#endif
