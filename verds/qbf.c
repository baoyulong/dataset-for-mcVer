#include "q2nt.h"
#include "qbf.h"

/************************************************************************/
extern int gSELflag;
extern FILE *gLog;
/************************************************************************/
int simpbranching(qinfo *qi)
{
	int i=UNKN;

/*
	i=simp(qi,0); 
*/
	if (i==UNKN) { i=qbranching(0,qi,qi->clauseinfo,qi->wd->cLength[0]); }
	freeqinfo(qi); return i; 
}
/************************************************************************/
int qbranching(int f0,qinfo *qi,qcdata *cd,int *clen)
{
	int i;
/*
	if ((i=simp(qi,f0))!=UNKN) return i;
*/
	switch (cd[f0].t) {
		case 'e': return qbranchingea(TRUE,f0,qi,cd,clen);
		case 'a': return qbranchingea(FALSE,f0,qi,cd,clen);
		case 'd': return qbranchingdc(TRUE,f0,qi,cd,clen);
		case 'c': return qbranchingdc(FALSE,f0,qi,cd,clen);
	}
}
int qbranchingea(int e,int f0,qinfo *qi,qcdata *cd,int *clen)
{
	int i,c;
	int *lclen;
	float *lcd;
	int *cur;
	qwdata *wd=qi->wd;


/*
	if ((i=simp(qi,f0))!=UNKN) return i;
*/
	c=sel(f0,cd[f0].Clause,qi,clen); 
	if (c==0) return qbranching(f0+1,qi,cd,clen);

	if (gSELflag) {
		if (e==FALSE) c=-c;
		clen=wdbranch0(qi->m,wd,c); 
		i=qunitp(f0,qi,clen,wd->cur[wd->depth]); 
		if (i==UNKN) i=qbranchingea(e,f0,qi,cd,clen); 
        	wdbranch0restore(wd); /* for branchingdc, wdbranch0back(wd); */
		return i; 
	}
	lclen=wdbranch0(qi->m,wd,c); 
	i=qunitp(f0,qi,lclen,wd->cur[wd->depth]); 

	if (i==UNKN) i=qbranchingea(e,f0,qi,cd,lclen);
	if (i==e) { 
/*??causing consistency problem in branchingdc, replaced 20110324.
		wdbranch0back(wd); 
*/
		wdbranch0restore(wd); 
		return i; 
	}

	lclen=wdbranch1(qi->m,wd,-c); 
	i=qunitp(f0,qi,lclen,wd->cur[wd->depth]); 
	if (i==UNKN) i=qbranchingea(e,f0,qi,cd,lclen);

       	wdbranch0restore(wd); /* for qbranchingdc,  wdbranch1back(wd); */
	return i; 
}
/**********************************************************************/
int qbranchingdc(int d,int f0,qinfo *qi,qcdata *cd,int *clen)
{
	int c;
	int i;
	int *p0=cd[f0].Clause;
	qwdata *wd=qi->wd;

	while (1) {
		c=seldc(p0,qi,clen); 
		if (c==0) return -d-3;
		*wd->Current++=c; *wd->Current=0;
		wd->InactiveMatrix[c]=1; wd->InactiveMatrix[-c]=3;
		i=qbranching(c-qi->n,qi,cd,clen); 
		if (i==d) return i;
	}
}
/**********************************************************************/

/************************************************************************/
int *qinitorder(qinfo *qi,int *clen)
{
	int i;
	int n=qi->n;
        char *gim=qi->wd->InactiveMatrix;
	float *ff=(float *)malloc((n+1)*sizeof(float));
	int *io=(int *)malloc((n+1)*sizeof(int));

	for (i=1;i<=n;i++) { io[i]=i; }
	for (i=1;i<=n;i++) if (gim[i]==0) ff[i]=ev0(qi,clen,i); else ff[i]=0.0; 
	quicksort_abf(ff,io,1,n); 
	return io;
}
/**********************************************************************/
float ev0(qinfo *qi,int *clen,int literal)
{
	sdata *sd=qi->sd;
	int *p0,*p1;
	float val0=0.0;
	float val1=0.0;

        p0=&(sd->LiteralMatrix[sd->LS[literal]]);
        for (p1=p0+sd->LL[literal];p0<p1;p0++) if (clen[*p0]) {
        	switch (clen[*p0]) {
        		case 0: break;
        		default: 
				if (clen[*p0]>10) val0+=1.0/(1<<10); 
				else val0+=1.0/(1<<(clen[*p0])); break;
        	}
	}
	if (val0==0.0) return -1.0;
        p0=&(sd->LiteralMatrix[sd->LS[-literal]]);
        for (p1=p0+sd->LL[-literal];p0<p1;p0++) if (clen[*p0]) {
        	switch (clen[*p0]) {
        		case 0: break;
        		default: 
				if (clen[*p0]>10) val1+=1.0/(1<<10); 
				else val1+=1.0/(1<<(clen[*p0])); break;
        	}
	}
	if (val1==0.0) return -2.0; return val0*val1;
}
/************************************************************************/

/************************************************************************/
int lookuplist(int rt,qinfo *qi,int *clist)
{
        int i0,i1,v0,v1;
        int *lclen;
        qwdata *wd=qi->wd;
	int c;

	for (c=0,v0=-1;*clist;clist++) {
        	lclen=wdbranch0(qi->m,wd,*clist);
        	i0=LUqunitp(rt,qi,lclen,wd->cur[wd->depth]);
        	wdbranch0restore(wd); if (i0<0) return *clist;
        	lclen=wdbranch0(qi->m,wd,-*clist);
        	i1=LUqunitp(rt,qi,lclen,wd->cur[wd->depth]);
        	wdbranch0restore(wd); if (i1<0) return -*clist;
		v1=i0*i1; if (v1>v0) { v0=v1; c=*clist; }
	}
	return c;
}
/************************************************************************/

/************************************************************************/
LOCAL int LUcount;
#define decode(a) 	if (a&1) a=-((-a)>>1); else a=(-a)>>1;
/************************************************************************/
#define isliteralor0(c0)   (c0<=qi->n&&c0>=-qi->n)
/************************************************************************/
#define callu0(a,b,c,d,e,f) \
	r=LUqunitprop0(qi,rt,a,b,c,d,e,f); if (r<0) return r; e+=r; \
/************************************************************************/
#define myret(r) {\
	return r;\
	}\
/************************************************************************/
int LUqunitp(int rt,qinfo *qi,int *clen,int *cur)
{
	char *gim=qi->wd->InactiveMatrix;
	int *p0,*p1;
	int *cure,*p2;
	int i,r;

        for (cure=cur;*cure;cure++) { gim[*cure] =4; gim[-*cure] =8; }
	for (LUcount=0;*cur;cur++) {
                gim[*cur] =1; gim[-*cur] =3; 
		if (!isliteralor0(*cur)) continue;
		r=LUqunitprop1(qi,rt,1,-*cur,qi->sd,qi->clauseinfo,clen,cure,gim);
		if (r<0) myret(r) else cure+=r;
		r=LUqunitprop1(qi,rt,0,*cur,qi->sd,qi->clauseinfo,clen,cure,gim); 
		if (r<0) myret(r) else cure+=r;
        }
	qi->wd->Current=cur;
	myret(LUcount)
}
/************************************************************************/
int LUqunitprop1(qinfo *qi,int rt,int neg,int c,sdata *sd,qcdata *cd,int *clen,int *cure,char *gim)
{
	int r;
	int *p0,*p1,*p2;
	int x;
	int k=0;
	int c0;

        p0=&(sd->LiteralMatrix[sd->LS[c]]);
        for (p1=p0+sd->LL[c];p0<p1;p0++) {
		if (clen[*p0]==0) continue;
		if (clen[*p0]==1) {
		    callu0(*p0,!neg,cd,clen,cure,gim); k+=r; continue;
		}
		if (neg) x='c'; else x='d';
		if (cd[*p0].t==x) {
		    callu0(*p0,!neg,cd,clen,cure,gim); k+=r; continue;
		}
	       	clen[*p0]--; LUcount++;
		if (clen[*p0]==1) {
			p2 =cd[*p0].Clause; 
			while (gim[*p2]&1) { p2++; } 
			c0=*p2;
			if (c0==0) {
				p2 =cd[*p0].Clause; 
				if (pnclause(p2)) {
		    		   callu0(*p0,cd[*p0].t=='d',cd,clen,cure,gim); 
				   k+=r; continue;
				}
				if (ppclause(p2)) {
		    		   callu0(*p0,!neg,cd,clen,cure,gim); 
				   k+=r; continue;
				}
				checkconsistency(qi); error("qunit 3"); 
				while (*p2!=-c) p2++; c0=*p2;
				if (c0==0) error("qunit 2"); 
		    		callu0(*p0,cd[*p0].t=='d',cd,clen,cure,gim); 
				k+=r; continue;
			}
		    	if (!isliteralor0(c0)) {
				c0=LUqunitgetliteral(qi,c0-qi->n,cd,clen,gim); 
				if (c0==0) continue;
		    	} 
			if ((gim[c0]&12)!=0) continue;
		    	if (r=assignable(qi,*p0,c0,cd,clen)) {
				if ((gim[r]&12)==0) { 
					k++; addqunit(cure,r); continue;
				}
		    	} 
		}
	}
	return k;
}
/************************************************************************/
int LUqunitprop0(qinfo *qi,int rt,int c0,int val,qcdata *cd,int *clen,int *cure,char *gim)
{
	int pp=cd[c0].p; 	
	int x;
	int r;


	if (c0==rt) return val-2; 
	if (pp==-1) return val-2; 
	if (clen[c0]==0) error("qunitprop0 clen");

	clen[c0]=0; 
	c0+=qi->n; addqunit(cure,(c0)); 
	if (clen[pp]==0) return 1;

	switch (cd[pp].t) {
	case 'a': 
	case 'e': callu0(pp,val,cd,clen,cure,gim); return r+1;
	case 'c': 
	case 'd': if (cd[pp].t=='c') x=0; else x=1;
		  if (val==x||clen[pp]==1) { 
		   	callu0(pp,val,cd,clen,cure,gim); return r+1;
		  }
		  clen[pp]--; LUcount++; return 1; 
	}
}
/************************************************************************/
int LUqunitgetliteral(qinfo *qi,int c0,qcdata *cd,int *clen,char *gim)
{
	int *p2;
	switch (cd[c0].t) {
	    case 'a':
	    case 'e': return LUqunitgetliteral(qi,c0+1,cd,clen,gim);
	    default:
			if (clen[c0]!=1) return 0;
			p2 =cd[c0].Clause; while (gim[*p2]&1) p2++; 
			if (isliteralor0(*p2)) return *p2;
			return LUqunitgetliteral(qi,*p2-qi->n,cd,clen,gim); 
	}
}
/************************************************************************/

/************************************************************************/
#define infosi(s,i)            {fprintf(stdout,"%s:%i\n",s,i);}
/************************************************************************/
void prep0md(qmdata *m0)
{	
	qmdata *m1=m0;
	if (m0==NULL) return;

	prep0md(m1->q); 
	while (m1->next!=NULL) { prep0md(m1->next->q); m1=m1->next; }

	switch (m0->Clause[0]) { 
	    case 'c': if (prep1md(m0,(int)'c',(int)'a')) { prep0md(m0); return;}
		      if (prep1md1(m0)) { prep0md(m0); return; }
		      break;
	    case 'd': if (prep1md(m0,(int)'d',(int)'e')) { prep0md(m0); return;}
		      if (prep1md1(m0)) { prep0md(m0);  return; }
		      break;
	}
}
/************************************************************************/
int prep1md(qmdata *m0,int cc,int aa)
{
	qmdata *m1,*m2,*m3=m0;
	if (m0->q==NULL) return 0;
	while (m0->q->Clause[0]!=aa&&m0->next!=NULL) { m0=m0->next; }
	if (m0->q->Clause[0]!=aa||m0->next==NULL) { return 0; }

	m1=m0; m2=m0; m0=m0->next;
	while (m0->q->Clause[0]!=aa&&m0->next!=NULL) { m2=m0; m0=m0->next; }
	if (m0->q->Clause[0]==aa) { 
		m2->next=m0->next;
		prep1merge(m1,m0,cc); 
		return 1; 
	}
	return 0;
}
/************************************************************************/
void prep1merge(qmdata *m0,qmdata *m1,int cc)
{
	qmdata *m0a,*m1a,*m2;
	int a0=intlen(m0->q->Clause);
	int a1=intlen(m1->q->Clause);


	m2=(qmdata *)calloc(1,sizeof(qmdata));
	m2->Clause=calloc(2,sizeof(int)); m2->Clause[0]=cc; 
	m2->q=m0->q->q; 
	m2->next=(qmdata *)calloc(1,sizeof(qmdata)); 
	m2->next->q=m1->q->q; m2->next->next=NULL;

	if (a0>=a1) { 
		prep1rename(m1->q->q,m1->q->Clause+1,m0->q->Clause+1); 
		free(m1->q->Clause); 
		free(m1->q); free(m1);
	} else { 
		prep1rename(m0->q->q,m0->q->Clause+1,m1->q->Clause+1); 
		free(m0->q->Clause); m0->q->Clause=m1->q->Clause; 
		free(m1->q); free(m1);
	}
 infos("0QBF-prep0md");
	m0->q->q=m2; m0->q->next=NULL;
}
/************************************************************************/
void prep1rename(qmdata *m0,int *c1,int *c2)
{
	int i,j,k,x;
	if (m0==NULL) return;
	switch (m0->Clause[0]) { 
		case 'c': 
		case 'd': 
			k=intlen(m0->Clause); x=intlen(c1);
			for (i=1;i<k;i++) for (j=0;j<x;j++) {
				if (m0->Clause[i]==c1[j]) m0->Clause[i]=c2[j];
				else if (m0->Clause[i]==-c1[j]) m0->Clause[i]=-c2[j];
			}
	}
	prep1rename(m0->q,c1,c2); 
	while (m0->next!=NULL) { prep1rename(m0->next->q,c1,c2); m0=m0->next; }
}
/************************************************************************/
void prep0md0(qmdata *m0)
{	/*consecutive quantifiers*/
	if (m0==NULL) return;
	switch (m0->Clause[0]) { 
		case 'a': if (prep1md0(m0,(int)'a')) prep0md0(m0); break;
		case 'e': if (prep1md0(m0,(int)'e')) prep0md0(m0); break;
	}
	prep0md0(m0->q); 
	while (m0->next!=NULL) { prep0md0(m0->next->q); m0=m0->next; }
}
/************************************************************************/
int prep1md0(qmdata *m0,int aa)
{
	int a0,a1; 
	int *c1;
	qmdata *q1;

	if (m0==NULL) return 0;
	if (m0->q->Clause[0]==aa) { 
		a0=intlen(m0->Clause); a1=intlen(m0->q->Clause);
		c1=calloc(a0+a1,sizeof(int)); 
		memcpy(c1,m0->Clause,sizeof(int)*a0);
		memcpy(c1+a0,m0->q->Clause+1,sizeof(int)*a1);
		free(m0->Clause); free(m0->q->Clause);
		m0->Clause=c1;
		q1=m0->q; m0->q=m0->q->q; free(q1); 
		return 1;
	}
	return 0;
}
/************************************************************************/
#define notae(c) (c!='a'&&c!='e')
/************************************************************************/
int prep1md1(qmdata *m0)
{ 	/* move quantifiers up*/
	qmdata *m1,*m0pq,*m0p=m0;

	if (m0->q==NULL) return 0;
	while (notae(m0p->q->Clause[0])&&m0p->next!=NULL) { m0p=m0p->next; }
	if (notae(m0p->q->Clause[0])) { return 0; }
/*
	prep1md1a(m0,m0p);
*/
	m0pq=m0p->q;
	m1=(qmdata *)calloc(1,sizeof(qmdata)); 
	m1->Clause=m0->Clause; m1->q=m0->q; m1->next=m0->next; 
	m0->Clause=m0p->q->Clause; m0->q=m1; m0->next=NULL;
	while (m1->q!=m0pq) { m1=m1->next; }
	m1->q=m0pq->q; free(m0pq); 
 infos("1QBF-prep1md1");
	return 1;
}
/************************************************************************/
void prep1md1a(qmdata *m0,qmdata *m0p)
{ 	/* move quantifiers up*/
	qmdata *m1,*m0pq;

	m0pq=m0p->q;
	m1=(qmdata *)calloc(1,sizeof(qmdata)); 
	m1->Clause=m0->Clause; m1->q=m0->q; m1->next=m0->next; 
	m0->Clause=m0p->q->Clause; m0->q=m1; m0->next=NULL;
	while (m1->q!=m0pq) { m1=m1->next; }
	m1->q=m0pq->q; free(m0pq); 
 infos("1QBF-prep1md1");
}
/************************************************************************/

/********************************************************************/
int QBFapply(qinfo *qi,qmdata *qm)
{
        int r;

        if (qm!=NULL) {
        	qi->m=qmdclauses(qm);
        	qi->l=qmdlen(qm);
                qmdsimp(qm,qi);
                md2qi(qm,qi); qi->sd=qi2sd(qi); qi->wd=sd2wd(qi->sd,qi);
		freeqmdata(qm);
        }
        r=simpbranching(qi); 
	return r+2;
}
/********************************************************************/

/************************************************************************/
#define MAXLOOKUP 6
/************************************************************************/
GLOBAL int gSELflag;
/************************************************************************/
int sel(int rt,int *var,qinfo *qi,int *clen)
{
	char *gim=qi->wd->InactiveMatrix;

	return sel_ev1(rt,var,qi,clen);

	for (;*var<=qi->n;var++) if (gim[*var]==0) break;
	if (*var<=qi->n) return *var; return 0;
}
/************************************************************************/
int sel_ev0(int rt,int *var,qinfo *qi,int *clen)
{
	int c;
	float v0,v1;
	char *gim=qi->wd->InactiveMatrix;

	for (c=0,v0=-4;*var<=qi->n;var++) if (gim[*var]==0) {
		v1=ev0(qi,clen,*var); /* useMAXN(v1,c,*var); */
		if (v1>v0) { v0=v1; c=*var; }
	}
        if (v0<0.0) { gSELflag=1; if (v0==-1.0) return -c; else return c; }
	gSELflag=0; return c;
}
/************************************************************************/
int sel_ev1(int rt,int *var,qinfo *qi,int *clen)
{               
        int c;  
        float v0,v1;
        char *gim=qi->wd->InactiveMatrix;
	int *cand=(int *)malloc((qi->n+1)*sizeof(int));
	int *p0=cand;
	float *candv1=(float *)malloc((qi->n+1)*sizeof(float));
	float *p1=candv1;
	int k;

        for (k=0,c=0,v0=-4;*var<=qi->n;var++) if (gim[*var]==0) {
                v1=ev0(qi,clen,*var); 
                if (v1>v0) { v0=v1; c=*var; }
		if (v1>0.0) { *p1++=v1; *p0++=*var; k++; }
        }       

        if (c==0);
        else if (v0<0.0) { gSELflag=1; if (v0==-1.0) c=-c; } 
	else { 
		gSELflag=0; 
        	quicksort_abf(candv1,cand,0,k-1); 
		if (k>MAXLOOKUP) k=MAXLOOKUP; cand[k]=0;
		c=lookuplist(rt,qi,cand); 
	}
	free(cand); free(candv1); return c;
}
/************************************************************************/

/************************************************************************/
int seldc(int *var,qinfo *qi,int *clen)
{
	char *gim=qi->wd->InactiveMatrix;

	for (;*var;var++) if (gim[*var]==0) break;
	if (*var&&*var<=qi->n) error("seldc"); return *var;
}
/************************************************************************/
