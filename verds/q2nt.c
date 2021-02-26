#include "q2nt.h"

/************************************************************************/
int inclausepn(int c0,int *clause)
{
	if (c0<0) c0=-c0;
	for (;*clause;clause++) if (*clause==c0) break; return *clause;
}
int intclausewritelen(int level,qinfo *qi,FILE *g)
{
	int i;
	for (i=0;i<qi->m;i++) fprintf(g,"%i ",qi->wd->cLength[level][i]); 
	fprintf(g,"\n"); 
}
int intclausewriteall(int c0,qinfo *qi,FILE *g)
{
	qcdata *cd=qi->clauseinfo;
	char *gim=qi->wd->InactiveMatrix;
	int *p1,*p2=cd[c0].Clause;
	int i;
	int *clen=qi->wd->cLength[qi->wd->depth];

	fprintf(g,"%i/%i(%c): ",c0,clen[c0],cd[c0].t);
	for (;*p2&&*p2<=qi->n;p2++) if ((gim[*p2]&1)==0) fprintf(g,"%i ",*p2); 
	for (p1=p2;*p2;p2++) if ((gim[*p2]&1)==0) fprintf(g,"%i ",*p2); 
	fprintf(g,"\n"); 

	for (;*p1;p1++) if ((gim[*p1]&1)==0) intclausewriteall(*p1-qi->n,qi,g); 
}

/************************************************************************/
int simp(qinfo *qi,int rt)
{
	int k;
	int *clen=qi->wd->cLength[qi->wd->depth];
	int *cur=qi->wd->Current;
	int *cure=cur;
	char *gim=qi->wd->InactiveMatrix;
	int r;
	int *c1;

 	k=1;
 	while (k>0) {
		k=simpprepare(qi,gim,clen,cure); 
		if (k>0) { 
			r=qunitp(rt,qi,clen,cur); 
			if (r!=UNKN) return r;
			cur=cure=qi->wd->Current;
		}
 	}
	return UNKN;
}
/************************************************************************/
int simpprepare(qinfo *qi,char *gim,int *clen,int *cure)
{
	sdata *sd=qi->sd;
	qcdata *cd=qi->clauseinfo;
	int *p0,*p1,*p2;
	int c,k=0;
	int r;

	for (c=1;c<=qi->n;c++) if (gim[c]==0) {
        	p0=&(sd->LiteralMatrix[sd->LS[c]]);
        	for (p1=p0+sd->LL[c];p0<p1;p0++) {
			if (clen[*p0]==0) continue;
			if (qi->e[c]) r=assignablee(*p0,c,cd,clen);
			else r=assignablea(*p0,c,cd,clen);
			if (r) { if ((gim[r]&12)==0) k++; addqunit(cure,r); }
		}
        	p0=&(sd->LiteralMatrix[sd->LS[-c]]);
        	for (p1=p0+sd->LL[-c];p0<p1;p0++) {
			if (clen[*p0]==0) continue;
			if (qi->e[-c]) r=assignablee(*p0,-c,cd,clen);
			else r=assignablea(*p0,-c,cd,clen);
			if (r) { if ((gim[r]&12)==0) k++; addqunit(cure,r); }
		}
	}
	return k;
}
/************************************************************************/

/************************************************************************/
int assignable(qinfo *qi,int c0,int val,qcdata *cd,int *clen)
{
        int pp=cd[c0].p;

	if (cd[c0].p==-1) error("assignable"); 
	if (qi->e[val]) 
	return assignablee(pp,val,cd,clen);
	return assignablea(pp,val,cd,clen);
}
int assignablee(int c0,int val,qcdata *cd,int *clen)
{
	if (clen[c0]==0) return 0;
	switch (cd[c0].t) {
	    case 'e': if (inclausepn(val,cd[c0].Clause)) return val;
	    case 'a': return assignablee(cd[c0].p,val,cd,clen);
	    case 'c': if (clen[c0]==1) return assignablee(cd[c0].p,val,cd,clen);
		      return assignableec(cd[c0].p,val,cd,clen);
	    case 'd': if (clen[c0]==1) return assignablee(cd[c0].p,val,cd,clen);
		      return assignableed(cd[c0].p,val,cd,clen);
	}
}
int assignablea(int c0,int val,qcdata *cd,int *clen)
{
	if (clen[c0]==0) return 0;
	switch (cd[c0].t) {
	    case 'a': if (inclausepn(val,cd[c0].Clause)) return -val;
	    case 'e': return assignablea(cd[c0].p,val,cd,clen);
	    case 'd': if (clen[c0]==1) return assignablea(cd[c0].p,val,cd,clen);
		      return assignablead(cd[c0].p,val,cd,clen);
	    case 'c': if (clen[c0]==1) return assignablea(cd[c0].p,val,cd,clen);
		      return assignableac(cd[c0].p,val,cd,clen);
	}
}
int assignableec(int c0,int val,qcdata *cd,int *clen)
{
	if (clen[c0]==0) return 0;
	switch (cd[c0].t) {
	    case 'd': if (clen[c0]==1) 
		      return assignableec(cd[c0].p,val,cd,clen); return 0;
	    case 'e': if (inclausepn(val,cd[c0].Clause)) return val;
	    default:  return assignableec(cd[c0].p,val,cd,clen); 
	}
}
int assignableed(int c0,int val,qcdata *cd,int *clen)
{
	if (clen[c0]==0) return 0;
	switch (cd[c0].t) {
	    case 'c': if (clen[c0]==1) 
		      return assignableed(cd[c0].p,val,cd,clen); return 0;
	    case 'e': if (inclausepn(val,cd[c0].Clause)) return val;
	    default:  return assignableed(cd[c0].p,val,cd,clen); 
	}
}
int assignablead(int c0,int val,qcdata *cd,int *clen)
{
	if (clen[c0]==0) return 0;
	switch (cd[c0].t) {
	    case 'c': if (clen[c0]==1) 
		      return assignablead(cd[c0].p,val,cd,clen); return 0;
	    case 'a': if (inclausepn(val,cd[c0].Clause)) return -val;
	    default:  return assignablead(cd[c0].p,val,cd,clen); 
	}
}
int assignableac(int c0,int val,qcdata *cd,int *clen)
{
	if (clen[c0]==0) return 0;
	switch (cd[c0].t) {
	    case 'd': if (clen[c0]==1) 
		      return assignableac(cd[c0].p,val,cd,clen); return 0;
	    case 'a': if (inclausepn(val,cd[c0].Clause)) return -val;
	    default:  return assignableac(cd[c0].p,val,cd,clen); 
	}
}


/************************************************************************/
int pnclause(int *p0)
{
	int i,j;
	for (i=0;p0[i];i++) for (j=i+1;p0[j];j++) if (p0[i]==-p0[j]) return 1;
	return 0;
}
int ppclause(int *p0)
{
	int i,j;
	for (i=0;p0[i];i++) for (j=i+1;p0[j];j++) if (p0[i]==p0[j]) return 1;
	return 0;
}
/************************************************************************/
void checkconsistency(qinfo *qi)
{
	int k,j;
	int nn=qi->n+qi->m;

 	for (k=1;k<=nn;k++) 
	if (qi->wd->InactiveMatrix[k]&1) {
 		for (j=0;qi->wd->InactiveList[j];j++) {
			if (qi->wd->InactiveList[j]==k) break;
			if (qi->wd->InactiveList[j]==-k) break;
		}
 		if (qi->wd->InactiveList[j]==0) {
			infos("CHECK0");
 			for (j=0;qi->wd->InactiveList[j];j++) 
			infoi(qi->wd->InactiveList[j]);
			infos("CHECK1");
			infoi(k);
			error("checkconsistency");
		}
	}
}

/************************************************************************/
#define encode(a) 	if (a<0) a=-(((-a)<<1)+1); else a=-(a<<1);
#define decode(a) 	if (a&1) a=-((-a)>>1); else a=(-a)>>1;
/************************************************************************/
#define isliteralor0(c0)   (c0<=qi->n&&c0>=-qi->n)
/************************************************************************/
#define callu0(a,b,c,d,e,f) \
	r=qunitprop0(qi,rt,a,b,c,d,e,f); if (r<0) return r; e+=r; \
/************************************************************************/
#define myret(r) {\
	return r;\
	}\
/************************************************************************/
int qunitp(int rt,qinfo *qi,int *clen,int *cur)
{
	char *gim=qi->wd->InactiveMatrix;
	int *p0,*p1;
	int *cure,*p2;
	int i,r;

        for (cure=cur;*cure;cure++) { gim[*cure] =4; gim[-*cure] =8; }
	for (;*cur;cur++) {
                gim[*cur] =1; gim[-*cur] =3; 
		if (!isliteralor0(*cur)) continue;
		r=qunitprop1(qi,rt,1,-*cur,qi->sd,qi->clauseinfo,clen,cure,gim);
		if (r<0) myret(r) else cure+=r;
		r=qunitprop1(qi,rt,0,*cur,qi->sd,qi->clauseinfo,clen,cure,gim); 
		if (r<0) myret(r) else cure+=r;
        }
	qi->wd->Current=cur;
	myret(UNKN)
}
/************************************************************************/
int qunitprop1(qinfo *qi,int rt,int neg,int c,sdata *sd,qcdata *cd,int *clen,int *cure,char *gim)
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
	       	clen[*p0]--;
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
				c0=qunitgetliteral(qi,c0-qi->n,cd,clen,gim); 
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
int qunitprop0(qinfo *qi,int rt,int c0,int val,qcdata *cd,int *clen,int *cure,char *gim)
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
		  clen[pp]--; return 1; 
	}
}
/************************************************************************/
int qunitgetliteral(qinfo *qi,int c0,qcdata *cd,int *clen,char *gim)
{
	int *p2;
	switch (cd[c0].t) {
	    case 'a':
	    case 'e': return qunitgetliteral(qi,c0+1,cd,clen,gim);
	    default:
			if (clen[c0]!=1) return 0;
			p2 =cd[c0].Clause; while (gim[*p2]&1) p2++; 
			if (isliteralor0(*p2)) return *p2;
			return qunitgetliteral(qi,*p2-qi->n,cd,clen,gim); 
	}
}
/************************************************************************/
