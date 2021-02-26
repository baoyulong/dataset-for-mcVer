#ifndef mm_h
#define mm_h
/************************************************************************/
#include "typetbd.h"
#include "cst.h"
/************************************************************************/
#define ckmmerror(s)    {if (s==NULL) error("low memory, exit");}
/***********************************************************************/
#define TBDmalloc(mmtb,n) \
        mmtb=(TBDmm *)malloc(1*sizeof(TBDmm)); ckmmerror(mmtb); \
        mmtb->tablesz=n; mmtb->cap=n/2; mmtb->nodes=0; \
        mmtb->table=(TBD **)calloc(mmtb->tablesz,sizeof(TBD *)); \
	if (mmtb->table==NULL) { free(mmtb); mmtb=NULL; } \
/************************************************************************/
#define TBDrealloc(mmtb,r) \
	error("low memory, exit"); \
        TBDmmfree(mmtb); mmtb->tablesz/=2; mmtb->cap/=2; mmtb->nodes=0; \
        mmtb->table=(TBD **)realloc(mmtb->table,mmtb->tablesz*sizeof(TBD *));\
        r=(TBD *)calloc(1,sizeof(TBD)); ckmmerror(r); \
/************************************************************************/
#define add1(x) ((unsigned int)(x))
#define add2(x,y) (add1(x)+add1(y))
#define add3(x,y,z) (add1(x)+add1(y)+add1(z))
/************************************************************************/
#define pair(a,b) add1((add1(add2(a,b)*add3(a,b,1)))/(add1(2))+add1(a))
#define triple(a,b,c) (pair(pair(a,b),c))
#define quadruple(a,b,c,d) (pair(pair(a,b),pair(c,d)))
/************************************************************************/
#define tableitem(t,v) (t->table[v])
/************************************************************************/
#define hvaluectb(t,x,a,b) (triple(x,a,b)%t->tablesz)
#define hvaluetct(t,w) (quadruple(w->x,w->a,w->b,w->c)%t->tablesz)
#define hvaluetct4(t,x,a,b,c) (quadruple(x,a,b,c)%t->tablesz)
/************************************************************************/
#define diffitem1(s,t) ((s->x!=t->x)||(s->a!=t->a)||(s->b!=t->b)||(s->c!=t->c))
#define diffitem3(s,x1,a1,b1) ((s->x!=x1)||(s->a!=a1)||(s->b!=b1))
#define diffitem4(s,x1,a1,b1,c1) ((s->x!=x1)||(s->a!=a1||s->b!=b1||s->c!=c1))
/************************************************************************/
#define istau(w)        (w==TBDtau)
#define isomg(w)        (w==TBDomg)
#define istoo(w)        (istau(w)||isomg(w))
/************************************************************************/
#include "wftbdmm.h"
#endif
