#ifndef tbdop_h
#define tbdop_h
/************************************************************************/
#include "tbdmm.h"
#include "tbdbw.h"
/************************************************************************/
#define issubset(s,t)  (TBDmp(TBDcn(s,TBDng(t))))
#define isomgtau(s,t)  (isomg(s)||istau(t))
/************************************************************************/
#define checkng(s)     if (isomg(s)) return TBDtau; if (istau(s)) return TBDomg;
#define checkcn(s,t)   if (isomgtau(s,t)) return s; if (isomgtau(t,s)) return t;
#define checkds(s,t)   if (isomgtau(s,t)) return t; if (isomgtau(t,s)) return s;
/************************************************************************/
#define TBDnnd(s,t)    TBDng(TBDcn(s,t))
#define TBDrc(w)       TBDmmmarkset(w,TBDCSTMARK)
/************************************************************************/
#define putctb(r0,r)   { r0->c=r; return r; }
/************************************************************************/
#define getctb(r0,cst,s,t) \
	r0=TBDmmgetc(TBDctb,cst,s,t);  \
	if (istoo(r0)||r0->c!=NULL) { return r0; } \
/************************************************************************/
#define putctbds(r0,r)   { r0->c=TBDng(r); return r; }
/************************************************************************/
#define getctbds(r0,cst,s,t) \
	r0=TBDmmgetc(TBDctb,cst,TBDng(s),TBDng(t));  \
	if (istoo(r0)||r0->c!=NULL) { return TBDng(r0); } \
/************************************************************************/
#include "wftbdop.h"
#endif
