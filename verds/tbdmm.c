#include "tbdmm.h"

/************************************************************************/
GLOBAL TBD *TBDomg=NULL;
GLOBAL TBD *TBDtau=NULL;
GLOBAL TBDmm *TBDttb=NULL;
GLOBAL TBDmm *TBDctb=NULL;
/************************************************************************/
void TBDmminitialize(int n)
{
        if (TBDomg!=NULL) return;
        TBDmalloc(TBDttb,ICACHESIZE); TBDmalloc(TBDctb,ICACHESIZE);
        TBDtau=(TBD *)calloc(1,sizeof(TBD)); TBDtau->x=n+1;
        TBDomg=(TBD *)calloc(1,sizeof(TBD)); TBDomg->x=-(n+1);
/*
GLOBAL TBD *TBDll=NULL;
        TBDll=(TBD *)calloc(4*n+3,sizeof(TBD))+2*n+1;
        TBDtau=&(TBDll[(2*n+1)]); TBDomg=&(TBDll[-(2*n+1)]);
	TBDtau->x=2*n+1; TBDomg->x=-(2*n+1);
	for (i=1;i<=2*n;i++) {
        	r=&(TBDll[i]);
        	r->y=0; r->n=NULL; r->x=i; 
		r->a=TBDomg; r->b=TBDtau; r->c=TBDtau; 
        	r=&(TBDll[-i]);
        	r->y=0; r->n=NULL; r->x=i; 
		r->a=TBDtau; r->b=TBDomg; r->c=TBDtau; 
	}
*/
}
/************************************************************************/
void TBDmmfree(TBDmm *mmtb)
{
        int i;

        for (i=0;i<mmtb->tablesz;i++)
        if (mmtb->table[i]!=NULL) mmtb->table[i]=TBDmmfreels(mmtb->table[i]);
}
/************************************************************************/
TBD *TBDmmfreels(TBD *w)
{
        TBD *r;

        if (w==NULL) return NULL;
        if (w->y>0) { w->y-=TBDCSTCNT; w->n=TBDmmfreels(w->n); return w; }
        if (w->y<0) { w->n=TBDmmfreels(w->n); return w; }
        r=TBDmmfreels(w->n); free(w); return r;
}
/************************************************************************/

/************************************************************************/
TBD *TBDmmaddt(TBDmm *TBDttb,TBD *w)
{
        int hv=hvaluetct(TBDttb,w);
        TBD *e0=tableitem(TBDttb,hv);
        TBD *e1=e0;

        while (e1!=NULL&&diffitem1(e1,w)) { e0=e1; e1=e1->n; }
        if (e1!=NULL) { free(w); return e1; }
        if (e1==e0) TBDttb->table[hv]=w; else { e0->n=w; } TBDttb->nodes++;
	if (TBDttb->nodes>TBDttb->cap) TBDmmresize(TBDttb,TBDmmaddt);
	return w;
}
/************************************************************************/
TBD *TBDmmgetc(TBDmm *TBDctb,int x,TBD *a,TBD *b)
{
        int hv=hvaluectb(TBDctb,x,a,b);
        TBD *e0=tableitem(TBDctb,hv);
        TBD *e1=e0;
        TBD *w;

        while (e1!=NULL&&diffitem3(e1,x,a,b)) { e0=e1; e1=e1->n; }
        if (e1!=NULL) { return e1->c; }
        w=TBDmmnewnode(x,a,b,NULL);
        if (e1==e0) TBDctb->table[hv]=w; else { e0->n=w; } TBDctb->nodes++;
	if (TBDctb->nodes>TBDctb->cap) TBDmmresize(TBDctb,TBDmmaddc);
	return w;
}
/************************************************************************/
TBD *TBDmmaddc(TBDmm *nc,TBD *w)
{
        int hv=hvaluectb(nc,w->x,w->a,w->b);
        TBD *e0=tableitem(nc,hv);
        TBD *e1=e0;

        while (e1!=NULL&&diffitem3(e1,w->x,w->a,w->b)) { e0=e1; e1=e1->n; }
        if (e1!=NULL) { error("TBDmmaddc"); }
        if (e1==e0) nc->table[hv]=w; else { e0->n=w; }
        nc->nodes++; return w;
}
/************************************************************************/
TBD *TBDmmnewnode(int x,TBD *a,TBD *b,TBD *c)
{
 	TBD *r=(TBD *)malloc(1*sizeof(TBD));
	if (r==NULL) { TBDrealloc(TBDctb,r); }
        r->y=0; r->n=NULL; r->x=x; r->a=a; r->b=b; r->c=c; 
	return r;
}
/************************************************************************/
void TBDmmresize(TBDmm *cache,TBD *f())
{
        int i;
        TBDmm *tbdc;

	if (cache->tablesz>=MCACHESIZE) { cache->nodes=0; return; }
	TBDmalloc(tbdc,2*cache->tablesz); 
	if (tbdc==NULL) { cache->nodes=0; return; }
        for (i=0;i<cache->tablesz;i++)
        if (cache->table[i]!=NULL) { TBDmmresizels(tbdc,cache->table[i],f); }
        free(cache->table); free(cache);
        if (f==TBDmmaddt) TBDttb=tbdc; else TBDctb=tbdc;
}
void TBDmmresizels(TBDmm *newcache,TBD *w,TBD *f())
{
        if (w==NULL) return;
        TBDmmresizels(newcache,w->n,f); w->n=NULL; f(newcache,w);
}
/************************************************************************/

/************************************************************************/
int TBDmmmarkset(TBD *w,int marker)
{
        int hv;
        TBD *e0,*e1;

        if (w==NULL) { TBDmmmarkclear(TBDttb,marker); return 1; }
        if (istoo(w)) { e1=w; } else {
                hv=hvaluetct(TBDttb,w); e0=tableitem(TBDttb,hv); e1=e0;
                while (e1!=NULL&&diffitem1(e1,w)) { e0=e1; e1=e1->n; }
                if (e1==NULL) return 0;
        }
        if (e1->y!=marker) { e1->y=marker; return 0; }
        TBDmmmarkclear(TBDttb,marker); return 1;
}
/************************************************************************/
void TBDmmmarkclear(TBDmm *mmtb,int marker)
{
        int i;
        for (i=0;i<mmtb->tablesz;i++) TBDmmmarkclearls(mmtb->table[i],marker);
}
/************************************************************************/
void TBDmmmarkclearls(TBD *w,int marker)
{
        if (w==NULL) return; 
	if (w->y==marker) w->y=0; TBDmmmarkclearls(w->n,marker);
}
/************************************************************************/
int TBDmmnodecnt(TBD *w)
{
        int x; x=TBDmmnodecntbg(w); TBDmmnodeclear(w,TBDCSTCNT); return x;
}
/************************************************************************/
int TBDmmnodecntbg(TBD *w)
{
        if (istoo(w)||w->y>0) return 0; w->y+=TBDCSTCNT;
        return TBDmmnodecntbg(w->a)+TBDmmnodecntbg(w->b)+TBDmmnodecntbg(w->c)+1;
}
/************************************************************************/
void TBDmmnodeclear(TBD *w,int marker)
{
        if (istoo(w)||!(w->y>0)) return; w->y-=marker;
        TBDmmnodeclear(w->a,marker); 
	TBDmmnodeclear(w->b,marker); TBDmmnodeclear(w->c,marker);
}
/************************************************************************/
void TBDkp(TBD *w)
{
        TBD *e0=TBDomg;

	if (istoo(w)) return;
        while (e0->c!=NULL) e0=e0->n; e0->c=w; 
	e0->n=(TBD *)calloc(1,sizeof(TBD)); 
	if (e0->n==NULL) { TBDrealloc(TBDctb,e0->n); }
}
/************************************************************************/
void TBDdr(TBD *w)
{
        TBD *e1,*e0=TBDomg;

	if (istoo(w)) return;
        while (e0->c!=NULL&&diffitem1(e0->c,w)) e0=e0->n;
        if (e0->c==NULL) { return; } 
	e1=e0->n; e0->c=e1->c; e0->n=e1->n; free(e1);
}
/************************************************************************/
void TBDcl()
{
	int k=0;
	TBD *e0;

	TBDomg->y=TBDtau->y=0;
	TBDmmfree(TBDctb); TBDctb->nodes=0;
        for (e0=TBDomg;e0->c!=NULL;e0=e0->n) k+=TBDmmnodecntbg(e0->c);
	TBDmmfree(TBDttb); TBDttb->nodes=k; 
        for (e0=TBDomg;e0->c!=NULL;e0=e0->n) TBDmmnodeclear(e0->c,TBDCSTCNT);
}
/************************************************************************/
int TBDmmnodecnt1(TBD *w)
{
        int x; x=TBDmmnodecntbg1(w); TBDmmnodeclear1(w); return x;
}
/************************************************************************/
int TBDmmnodecntbg1(TBD *w)
{
        if (istoo(w)||w->y<0) return 0; if (w->y>0) return w->y; 
       w->y=TBDmmnodecntbg1(w->a)+TBDmmnodecntbg1(w->b)+TBDmmnodecntbg1(w->c)+1;
	if (w->y<0) w->y=0x01000000; return w->y;
}
/************************************************************************/
void TBDmmnodeclear1(TBD *w)
{
        if (istoo(w)||w->y<=0) return; w->y=0;
        TBDmmnodeclear1(w->a); 
	TBDmmnodeclear1(w->b); TBDmmnodeclear1(w->c);
}
/************************************************************************/
int TBDclck(TBD *i2)
{
 static int clean=0;
	int y=TBDmmnodecnt(i2); 

	if ((clean+=y)>CLEANSIZE) { clean=0; TBDkp(i2); TBDcl(); TBDdr(i2); } 
	return y;
}
/************************************************************************/

/************************************************************************/
void TBDcheck(TBD *w,int min,int max)
{
	int x;
	if (istoo(w)) return;
	x=abs(w->x);
	if (x>max) error("TBDcheck >"); if (x<min) error("TBDcheck <");
	x++; TBDcheck(w->a,x,max); TBDcheck(w->b,x,max); TBDcheck(w->c,x,max);
}
/************************************************************************/
void TBDcheckodd(TBD *w,int odd)
{
        int x=abs(w->x);
	if (istoo(w)) return;
        if (x%2!=odd) error("TBDcheckodd");
        TBDcheckodd(w->a,odd); TBDcheckodd(w->b,odd); TBDcheckodd(w->c,odd);
}
/*******************************************************************/

/************************************************************************/
LOCAL FILE *logtbd=NULL;
/************************************************************************/
void TBDwrite(TBD *w,int level,FILE *g)
{
	p2writelev(w,0,level,g);
}
/************************************************************************/
void TBDwriteclause(TBD *w,int n,FILE *g)
{
	int *clause=(int *)malloc((n+1)*sizeof(int));
	*clause=0; p2writeclause(w,clause,g);
}
/************************************************************************/
int p2intlen(int *aa)
{
        int i=0; while (*aa) { aa++; i++; } return i;
}
/************************************************************************/
void p2intwrite(int *ss,int a,int b,FILE *g)
{
        int i,j;
        for (j=0,i=a;i<=b;i++) { fprintf(g,"%i ",ss[i]); }
        fprintf(g,"0\n");
}
/*******************************************************************/
void p2writelev(TBD *w,int slevel,int elevel,FILE *g)
{
    int l=slevel;
    if (elevel&&slevel>=elevel) return;

    while (slevel--) fprintf(g," ");
    if (w==TBDtau) fprintf(g,"T\n"); else 
    if (w==TBDomg) fprintf(g,"F\n"); else 
    if (w->c==TBDtau&&w->a==TBDtau&&w->b==TBDomg) fprintf(g,"%i\n",-w->x); else 
    if (w->c==TBDtau&&w->a==TBDomg&&w->b==TBDtau) fprintf(g,"%i\n", w->x); else 
    {
	fprintf(g,"%i\n",w->x);
	p2writelev(w->a,l+1,elevel,g); 
	p2writelev(w->b,l+1,elevel,g); 
	p2writelev(w->c,l+1,elevel,g);
    }
}
/************************************************************************/
void p2writeclause(TBD *w,int *clause,FILE *g)
{
	int i=p2intlen(clause);

	if (w==TBDtau) return;
	if (w==TBDomg) { p2intwrite(clause,0,i,g); return; }
	clause[i]=w->x; clause[i+1]=0; p2writeclause(w->a,clause,g);
	clause[i]=-w->x; clause[i+1]=0; p2writeclause(w->b,clause,g);
	clause[i]=0; p2writeclause(w->c,clause,g);
}
/************************************************************************/
