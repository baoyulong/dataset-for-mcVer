#ifndef dmc_h
#define dmc_h
/*******************************************************************/
#include "tbdmm.h"
#include "tbdop.h"
#include "tbdbw.h"
#include "tree.h"
#include "bp2t.h"
#include "option.h"
/*******************************************************************/
#define DMCex0(w)       TBDbw(w)
#define DMCef0(w)       DMCfg(w,TBDds)
#define DMCeg0(w)       DMCfg(w,TBDcn)
#define DMCeu0(u,w)     DMCur(u,w,TBDds,TBDcn)
#define DMCer0(u,w)     DMCur(u,w,TBDcn,TBDds)
/***********************************************************************/
#define cl(a)           
#define clcl(a,b)       
/*
#define cl(a)           {TBDkp(a); TBDcl(); TBDdr(a);}
#define clcl(a,b)       {TBDkp(a); TBDkp(b); TBDcl(); TBDdr(a); TBDdr(b);}
/************************************************************************/
extern time_t t0,t1;
/************************************************************************/
/*
#define POinit() \
        POenable=(char **)malloc((ggt->trscnt)*sizeof(char *)); \
        POdependent=(char **)malloc((ggt->trscnt)*sizeof(char *)); \
        for (i=0;i<ggt->trscnt;i++) { \
                POenable[i]=(char *)malloc((ggt->trscnt)*sizeof(char)); \
                POdependent[i]=(char *)malloc((ggt->trscnt)*sizeof(char)); \
        } \
        POread(POenable,"po.e0",ggt->trscnt); \
        POread(POdependent,"po.d0",ggt->trscnt); \
        POinv=(char *)calloc((ggt->trscnt),sizeof(char)); \
	POinitinv(POinv,es); \
/************************************************************************/
#include "wfdmc.h"
#endif
