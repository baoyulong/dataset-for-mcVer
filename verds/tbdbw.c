#include "tbdbw.h"

/************************************************************************/
#define BW		1
/*
#define FW		2 /*2,3 does not seem to have difference*/
/************************************************************************/
#define BW_one 		1
#define BW_two 		2
#define BW_replace 	3
/************************************************************************/
#define FW_one 		1
#define FW_two 		2
#define FW_sp		3
/************************************************************************/
extern TBDprg *ggt;
extern int DMCprocedurewait;
/************************************************************************/
TBD *pc2tbd(TBDprg *lgt,tree *pname,TBD *s0);
/************************************************************************/
#define checkpcall(a) \
	if (a!=NULL) { \
		if (DMCprocedurewait) break; \
		if (TBDmp(p1)) p1=TBDomg; else p1=pc2tbd(ggt,a,p1); \
	}
/************************************************************************/
#define progress() \
        fprintf(stdout,"Progress"); \
        for (k=i/10,j=0;j<k;j++) fprintf(stdout,"."); fprintf(stdout,"\n"); \
/************************************************************************/
TBD *TBDbw(TBD *r0)
{
        int j,k;
	int n=ggt->booln;
	TBDtrs **t1=ggt->trs; 
	TBD *r=TBDomg;
 static i=0;

/*
 for (k=0;t1[k]!=NULL;k++); infos("n of TRS"); infoi(k);
*/

	i++; reportprogress(1,i); reportprogress(0,i);

	switch(BW) {
	    case BW_one: 
        	 for (k=0;t1[k]!=NULL;k++) 
		 r=TBDds(r,TBDbwone(r0,t1[k]->bpt,n)); 
		 break;
	    case BW_two: 
        	 for (k=0;t1[k]!=NULL;k++) 
	 	 r=TBDds(r,TBDcn(t1[k]->p,TBDbwone(r0,t1[k]->bt,n))); 
		 break;
	    case BW_replace: 
        	 for (k=0;t1[k]!=NULL;k++)
		 r=TBDds(r,TBDcn(t1[k]->p,TBDreplacebwone(r0,t1[k]->btt,n)));
		 break;
	}
	return r;
}
/************************************************************************/
TBD *TBDfw(TBD *p0)
{
        int k;
	int n=ggt->booln;
	TBDtrs **t1=ggt->trs; 
	TBD *p1;
	TBD *r=TBDomg;
	int FW=3;

	if (ggt->puretrans==0) FW=2;

	switch (FW) {
	    case FW_one: 
        	 for (k=0;t1[k]!=NULL;k++) if (t1[k]->p!=TBDomg)
		 r=TBDds(r,TBDbwone(p0,t1[k]->pt,n)); 
		 break;
	    case FW_two: 
        	 for (k=0;t1[k]!=NULL;k++) if (t1[k]->p!=TBDomg) {
		 	p1=TBDcn(p0,t1[k]->p); 
			checkpcall(t1[k]->pc);
		 	r=TBDds(r,TBDrn(TBDrp(p1,t1[k]->t,1,1)));
		 }
		 break;
	    case FW_sp: 
        	 for (k=0;t1[k]!=NULL;k++) if (t1[k]->p!=TBDomg) {
		 	p1=TBDcn(p0,t1[k]->p); checkpcall(t1[k]->pc);
		 	r=TBDds(r,TBDsp(p1,t1[k]->tc,t1[k]->uclist)); 
		 }
		 break;
	}
	return r;
}
/************************************************************************/
TBD *TBDbwSelectOne(TBD *r0,TBD *rr,int *x)
{
        int i,j,k,reuse;
	int n=ggt->booln;
	TBDtrs **t1=ggt->trs; 
	TBD *r,*r1,*rr1;
	tree *pc;

	for (k=0;t1[k]!=NULL;k++) 
	if (t1[k]->p!=TBDomg&&t1[k]->pc==NULL) {
	        switch(BW) {
	    	case BW_one: 
		     r=TBDbwone(r0,t1[k]->bpt,n); 
		     break;
	    	case BW_two: 
	 	     r=TBDcn(t1[k]->p,TBDbwone(r0,t1[k]->bt,n)); 
		     break;
	    	case BW_replace: 
		     r=TBDcn(t1[k]->p,TBDreplacebwone(r0,t1[k]->btt,n));
		     break;
	        }
		r=TBDcn(r,rr); if (!TBDmp(r)) { *x=k; return TBDone(r); }
	}

	reuse=0; pc=NULL;

	for (k=0;t1[k]!=NULL;k++)
	if (t1[k]->p!=TBDomg&&t1[k]->pc!=NULL) {
		for (rr1=rr,i=1;i<2*n;i+=2) {
			for (r1=rr1,j=1;j>=-1;j-=2) {
				rr1=TBDcn(TBDrestrict(r1,i*j),TBDap(i*j));
	 			r=TBDcn(rr1,t1[k]->p); 
	        		if (!TBDmp(r)) {
					if (reuse==1&&t1[k]->pc==pc) {
/*zwh
					   r=pc2tbd(ggt,t1[k]->pc,r); 
*/
					   r=pc2tbd(ggt,NULL,r);
					} else {
					   r=pc2tbd(ggt,t1[k]->pc,r); 
					   pc=t1[k]->pc; reuse=1;
					}
		 			r=TBDrn(TBDrp(r,t1[k]->t,1,1));
					r=TBDcn(r,r0); 
					if (j==1&&!TBDmp(r)) break;
				}
			}
			if (TBDmp(r)) break;
		}
		if (!TBDmp(r)) { *x=k; return TBDone(rr1); }
	}
	error("TBDbwSelectOne");
	return TBDomg;
}
/************************************************************************/
TBD *TBDfwSelectOne(TBD *p0,TBD *rr,int *x)
{
        int k;
	int n=ggt->booln;
	TBDtrs **t1=ggt->trs; 
	TBD *p1;
	TBD *r;
	int FW=3;

	if (ggt->puretrans==0) FW=2;

	for (k=0;t1[k]!=NULL;k++) if (t1[k]->p!=TBDomg) {
	    	switch (FW) {
	    	case FW_one: 
		 	r=TBDbwone(p0,t1[k]->pt,n); 
			break;
	    	case FW_two: 
		 	p1=TBDcn(p0,t1[k]->p); checkpcall(t1[k]->pc);
		 	r=TBDrn(TBDrp(p1,t1[k]->t,1,1));
		 	break;
	    	case FW_sp: 
		 	p1=TBDcn(p0,t1[k]->p); checkpcall(t1[k]->pc);
		 	r=TBDsp(p1,t1[k]->tc,t1[k]->uclist); 
		 	break;
	   	}
		r=TBDcn(r,rr); if (!TBDmp(r)) { *x=k; return TBDone(r); }
	}
	error("TBDfwSelectOne");
	return TBDomg;
}
/************************************************************************/

/************************************************************************/
TBD *TBDbwone(TBD *r0,TBD *r1,int n)
{
	int i;
	TBD *new;

	if (r1==NULL) return TBDomg;
/*
	new=TBDcn(r0,r1); for (i=1;i<2*n;i+=2) new=TBDxb(i,new);
*/
	new=TBDrp(r0,r1,1,1);

        return TBDrn(new);
}
/************************************************************************/
TBD *TBDreplacebwone(TBD *r0,TBD **r1,int n)
{
	int i;
	TBD *new=r0;

	for (i=1;i<=2*n;i+=2) 
	if (r1[i]==TBDtau&&r1[-i]==TBDtau) new=TBDxb(i,new);

	new=TBDreplace(r0,r1); return TBDrn(new); 
}
/************************************************************************/
TBD *TBDrn(TBD *w)
{
        TBD *a,*b,*c,*r,*r0;
        int x;

        if (istoo(w)) return w; 
	getctb(r0,TBDCSTLB,NULL,w);
        a=TBDrn(w->a); b=TBDrn(w->b); c=TBDrn(w->c);
	x=w->x; if (x>0) x--; else x++; r=TBDnewnode(x,a,b,c); 
	putctb(r0,r);
}
/*******************************************************************/
TBD *TBDsprn(TBD *w,char *vlist) 
{ /* x not in list -> x+1; odd2even */
        TBD *a,*b,*c,*r,*r0;
        int x;

        if (istoo(w)) return w; 
	getctb(r0,TBDCSTLB,vlist,w);
        a=TBDsprn(w->a,vlist); b=TBDsprn(w->b,vlist); c=TBDsprn(w->c,vlist);
	x=abs(w->x); if (vlist[x]==0) x++; if (w->x<0) x=-x;
	r=TBDnewnode(x,a,b,c); putctb(r0,r);
}
/*******************************************************************/
TBD *TBDasrn(TBD *w,char *vlist) 
{ /* x in list -> x-1; even2odd */
        TBD *a,*b,*c,*r,*r0;
        int x;

        if (istoo(w)) return w; 
        a=TBDasrn(w->a,vlist); b=TBDasrn(w->b,vlist); c=TBDasrn(w->c,vlist);
	x=abs(w->x); if (vlist[x]==1) x--; if (w->x<0) x=-x;
	r=TBDnewnode(x,a,b,c); return r;
}
/*******************************************************************/

/************************************************************************/

/************************************************************************/
TBD *TBDsp(TBD *r0,TBD *a,char *vlist)
{
	TBD *t0=TBDsprn(r0,vlist);

 	if (r0==TBDomg) return r0;
	t0=TBDrp(t0,a,1,0); 

	return t0;
}
/************************************************************************/
#define conspos(x) \
        if ((x%2)==odd) { if (pos) r=TBDds(a,b); else r=TBDcn(a,b); } else \
        r=TBDnewnode(x,a,b,TBDtau); \
/************************************************************************/
#define consneg(x) \
        if ((-x%2)==odd) { if (pos) r=TBDcn(TBDng(a),TBDng(b)); \
                        else r=TBDng(TBDcn((a),(b))); } else \
        r=TBDnewnode(x,a,b,TBDtau); \
/************************************************************************/
#define RPcn(a,b) 	TBDrp(a,b,pos,odd)
#define RPnnd(a,b) 	TBDng(TBDrp(a,b,!pos,odd))
/************************************************************************/
TBD *TBDrp(TBD *s,TBD *t,int pos,int odd)
{
        TBD *r,*r0,*a,*b,*c;
/*
	if (pos) s=TBDng(TBDcn(s,t)); 
	s=TBDiterativeabseven(s,!odd); if (pos) s=TBDng(s); return s;
*/

	if (isomg(s)||isomg(t)) return TBDomg;
	if ((istau(s)&&(s=t))||istau(t)) {
		if (pos) s=TBDng(s); 
		s=TBDiterativeabseven(s,!odd); if (pos) s=TBDng(s); return s;
	}

        getctb(r0,TBDCSTRP,s,t); 
	switch (TBDcase(s->x,t->x)) {
	    case 0: a=RPcn(TBDnnd(s->a,s->c),TBDnnd(t->a,t->c));
		    b=RPcn(TBDnnd(s->b,s->c),TBDnnd(t->b,t->c)); 
		    conspos(-s->x); break;
	    case 1: a=RPcn(TBDcn(t->a,t->c),TBDnnd(s->a,s->c));
                    b=RPcn(TBDcn(t->b,t->c),TBDnnd(s->b,s->c)); 
		    conspos(-s->x); break;
	    case 2: a=RPcn(TBDcn(s->a,s->c),TBDnnd(t->a,t->c));
                    b=RPcn(TBDcn(s->b,s->c),TBDnnd(t->b,t->c)); 
		    conspos(-t->x); break;
	    case 3: a=TBDrp(TBDcn(s->a,s->c),TBDcn(t->a,t->c),pos,odd); 
	            b=TBDrp(TBDcn(s->b,s->c),TBDcn(t->b,t->c),pos,odd); 
		    conspos(s->x); break;
	    case 4: a=RPcn(t,TBDng(TBDcn(s->a,s->c))); 
                    b=RPcn(t,TBDng(TBDcn(s->b,s->c))); 
		    conspos(-s->x); break;
	    case 5: a=RPcn(TBDcn(s->a,s->c),t);
		    b=RPcn(TBDcn(s->b,s->c),t);
		    conspos(s->x); break;
	    case 6: a=RPcn(s,TBDng(TBDcn(t->a,t->c))); 
                    b=RPcn(s,TBDng(TBDcn(t->b,t->c))); 
		    conspos(-t->x); break;
	    case 7: a=RPcn(TBDcn(t->a,t->c),s);
		    b=RPcn(TBDcn(t->b,t->c),s);
		    conspos(t->x); break;
	}
	putctb(r0,r);
}
/************************************************************************/
