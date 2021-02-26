#ifndef mod_h
#define mod_h
/***********************************************************************/
#include "moddef.h"
#include "modbin.h"
/***********************************************************************/
typedef struct _MODmodule {
	char key[10]; /*"VVM","MODULE","PROC"*/
	char *txt;
	char name[1024];
	char *parlist[1024]; int parcnt;
	char *proclist[1024]; int proccnt;
	char *varlist[1024]; int varcnt;
	char *init;
	char *proc;
	char *trans;
	char *fair;
	char *inv;
	char *spec;
	struct _MODmodule *next;
} MODmodule;
/***********************************************************************/
typedef struct _MODoption {
	int dir;
	int maxt;
	int test;
	char clname[32]; 
	int trnumber;
} MODoption;
/***********************************************************************/
typedef struct _MODmoduleproc {
	char pname[1024]; 
	char mname[1024]; 
	char *parlist[4096]; int parcnt;
} MODmoduleproc;
/***********************************************************************/
#define freemoduleproc(m) \
	{int i; for(i=0;i<m->parcnt;i++)free(m->parlist[i]);free(m);}
/***********************************************************************/
#define stringnew(b,n) \
	{b=(char *)malloc((n+1)*sizeof(char));} \
/***********************************************************************/
#define stringcpy(b,a) if (a!=NULL) { \
	b=(char *)malloc((strlen(a)+1)*sizeof(char)); strcpy(b,a);} \
/***********************************************************************/
#define stringpar(b,a) if (a!=NULL) { \
	b=(char *)malloc((strlen(a)+7)*sizeof(char)); \
	sprintf(b,"(%s)",a);} \
/***********************************************************************/
#define stringneg(b,a) if (a!=NULL) { \
	b=(char *)malloc((strlen(a)+7)*sizeof(char)); \
	sprintf(b,"!(%s)",a);} \
/***********************************************************************/
#define stringcat(b,a1,a2) if (a1!=NULL&&a2!=NULL) { \
	b=(char *)malloc((strlen(a1)+strlen(a2)+6)*sizeof(char)); \
	sprintf(b,"%s%s",a1,a2);} \
/***********************************************************************/
#define stringconj(b,a1,a2) if (a1!=NULL&&a2!=NULL) { \
	b=(char *)malloc((strlen(a1)+strlen(a2)+6)*sizeof(char)); \
	sprintf(b,"(%s)%c(%s)",a1,'&',a2); }\
/***********************************************************************/
#define stringdisj(b,a1,a2) if (a1!=NULL&&a2!=NULL) { \
	b=(char *)malloc((strlen(a1)+strlen(a2)+6)*sizeof(char)); \
	sprintf(b,"(%s)%c(%s)",a1,'|',a2); }\
/***********************************************************************/
#define stringimp(b,a1,a2) if (a1!=NULL&&a2!=NULL) { \
	b=(char *)malloc((strlen(a1)+strlen(a2)+7)*sizeof(char)); \
	sprintf(b,"!(%s)|(%s)",a1,a2);} \
/***********************************************************************/
#define stringdisj(b,a1,a2) if (a1!=NULL&&a2!=NULL) { \
	b=(char *)malloc((strlen(a1)+strlen(a2)+6)*sizeof(char)); \
	sprintf(b,"(%s)%c(%s)",a1,'|',a2); }\
/***********************************************************************/
#define copymodulecontents(m1,m0) { \
	m1=(MODmodule *)calloc(1,sizeof(MODmodule)); \
	strcpy(m1->key,m0->key);  \
	strcpy(m1->name,m0->name);  \
	stringcpy(m1->init,m0->init);  \
	stringcpy(m1->proc,m0->proc);  \
	stringcpy(m1->trans,m0->trans);  \
	stringcpy(m1->fair,m0->fair);  \
	stringcpy(m1->inv,m0->inv);  \
	stringcpy(m1->spec,m0->spec);  \
	}
/***********************************************************************/
#define Tmod 	1
#define Tflat 	2
#define Tclock 	3
#define Tdlf1 	4
#define Tdlf2 	5
/***********************************************************************/
#include "wfmod.h"
#endif
