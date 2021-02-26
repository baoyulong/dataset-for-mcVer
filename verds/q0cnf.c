#include <ctype.h>
#include <math.h>
#include "q0cnf.h"
#include "q1rd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>

/*******************************************************************/
#define isLiteral(c)     (isdigit(c)||(c=='-'))
#define isNumbers(ll)	 ((strncmp(ll,"p cnf",5)==0))
#define moveLiteral(ll)  while (isLiteral(*ll)) ll++; 
#define moveNLiteral(ll) while (*ll&&!isLiteral(*ll)&&*ll!='\n') ll++;
/*******************************************************************/
void qmmodelwrite(qmodel *mo,FILE *g)
{
        int i;
        int *mstring=mo->mstring;
        int mstringc=mo->l;

        fprintf(g,"p cnf %i %i\n",mo->n,mo->m);
        for (i=0;i<mstringc;i++)
        if (mstring[i]) fprintf(g," %i ",mstring[i]);
        else fprintf(g," %i\n",mstring[i]);
}
/*******************************************************************/
void read_numbers(char *ll,qmodel *mm)
{
	int n,m,l;

	m=n=l=0;
	while (*ll) {
		if (*ll=='c') { while (*ll&&*ll!='\n') ll++; }
		else if (isNumbers(ll)) { break; }
		else ll++;
	}
	if (*ll) {
		moveNLiteral(ll); n=atoi(ll); moveLiteral(ll); 
		moveNLiteral(ll); m=atoi(ll); moveLiteral(ll); 
		moveNLiteral(ll); if (isLiteral(*ll)) l=atoi(ll); 
	}
	if (!(n>0&&m>0)) error("numbers required.");

	if (l==0) l=2*strlen(ll); 
	mm->n=n; mm->m=m; mm->l=l;
        mm->mstring=malloc(l*sizeof(int)); 
}
void read_clauses(char *ll,qmodel *mm)
{
        int i=0,j=0,k=0,len,n;
        int clen,c;
	int modm=mm->m;
	int modl=mm->l;
	int *p0=mm->mstring;
	
	while (*ll) {
		if (*ll=='c') { while (*ll&&*ll!='\n') ll++; }
		else if (isNumbers(ll)) { while (*ll&&*ll!='\n') ll++; break; }
		else ll++;
	}
	for (;;) {
		while (*ll&&(!isLiteral(*ll)||*ll=='0')) ll++;
		if (*ll==0) break;
                if (j==modm) error("too many clauses");
                for (clen=0,i=0;i<modl;i++) {
			c=atoi(ll); moveLiteral(ll); moveNLiteral(ll);
			if (!c) {
				n=remove_red(p0-clen,clen); 
				p0-=n; k-=n;
				if (clen>n) *p0++=c; else j--; break;
			}
        		if (k==modl) error("too many literals"); 
			k++; *p0++=c; clen++;
                }
                j++;
        }
	mm->m=j; mm->l=k+j; 
	mm->mstring=realloc(mm->mstring,mm->l*sizeof(int)); 
}
/************************************************************************/
int remove_red(int *p0,int k)
{
	int i,j;

	for (i=0;i<k;i++)
	for (j=i+1;j<k;j++) {
		if (p0[i]==p0[j]) { 
			p0[i]=p0[k-1]; p0[k-1]=0;
			return remove_red(p0,k-1)+1; 
		}
		if (p0[i]==-p0[j]) { return k; }
	}
	return 0;
}
int remove_red2(int *p0)
{
	int *p1=p0;
	int k;
	
	while (*p1) p1++; k=p1-p0;
	if (k==remove_red(p0,k)) return 0; return 1;
}
/************************************************************************/
void modelclean(qmodel *mm)
{
        int i;
	int *p0=mm->mstring;
	int *p2=mm->mstring;
	int m=0;
	int n,k;

	for (i=0;i<mm->m;i++) {
		k=0;
		while (*p0) { *p2++=*p0++; k++; }
		n=remove_red(p2-k,k);
		p2-=n; k-=n; if (k>0) { *p2++=0; m++; } 
		p0++;	/*zwh*/
	}
	mm->m=m;
	mm->l=p2-mm->mstring;
	mm->mstring=realloc(mm->mstring,mm->l*sizeof(int));
}
/************************************************************************/
int intin(int a,int *b,int x,int y)
{
	int i;
	for (i=x;i<=y;i++) if (a==b[i]) return i; return 0;
}
/************************************************************************/
int intlen(int *ss)
{
        int i=0;
	while (ss[i]) i++; return i;
}
/*******************************************************************/

/************************************************************************/
void qmdsimp2(qmdata *md,qinfo *qi)
{
        int t0;
	qmdata *md0=md,*md1;
	int x=0;

        if (md==NULL) return; t0=md->Clause[0];
        switch (t0) {
            case 'a':
            case 'e':
                qmdsimp2(md->q,qi); break;
            case 'c':
               	md=md0; qmdsimp2(md->q,qi); 
               	for (;md->next!=NULL;md=md->next) qmdsimp2(md->next->q,qi);
               	md=md0; if (md->q!=NULL&&md->q->Clause[0]=='a') { x=1; }
                if (x==0)
                for (;md->next!=NULL;md=md->next) 
                if (md->next->q!=NULL&&md->next->q->Clause[0]=='a') {
			md1=md->next->q; md->next->q=md0->q; md0->q=md1;
			x=1; break;
		}
		if (x) { mdmoveupfirstquantifier(md,qi); qmdsimp2(md,qi); }
                break;
            case 'd':
               	md=md0; qmdsimp2(md->q,qi); 
               	for (;md->next!=NULL;md=md->next) qmdsimp2(md->next->q,qi);
               	md=md0; if (md->q!=NULL&&md->q->Clause[0]=='e') { x=1; }
                if (x==0)
		for (;md->next!=NULL;md=md->next) 
                if (md->next->q!=NULL&&md->next->q->Clause[0]=='e') {
			md1=md->next->q; md->next->q=md0->q; md0->q=md1;
			x=1; break;
		}
		if (x) { mdmoveupfirstquantifier(md,qi); qmdsimp2(md,qi); }
                break;
        }
}
/************************************************************************/
void mdmoveupfirstquantifier(qmdata *md,qinfo *qi)
{
        qmdata *md0;
        int *a1;

        a1=md->Clause; md->Clause=md->q->Clause; md->q->Clause=a1;
        md0=md->next; md->next=md->q->next; md->q->next=md0;
}
/*******************************************************************/
void qmdcdneg(qmdata *md,qinfo *qi)
{
        int t0; /*changes a-e, c-d*/

        if (md==NULL) return; t0=md->Clause[0];
        switch (t0) {
           case 'a': md->Clause[0]='e'; qmdcdneg(md->q,qi); break;
           case 'e': md->Clause[0]='a'; qmdcdneg(md->q,qi); break;
           case 'c': md->Clause[0]='d'; qmdcdneg(md->q,qi); 
               	     for (;md->next!=NULL;md=md->next) qmdcdneg(md->next->q,qi);
		     break;
           case 'd': md->Clause[0]='c'; qmdcdneg(md->q,qi); 
               	     for (;md->next!=NULL;md=md->next) qmdcdneg(md->next->q,qi);
		     break;
        }
}
/************************************************************************/
int qmdcdnegok(qmdata *md,qinfo *qi)
{
	qmdcdneg(md,qi); return 1;

	if (md->Clause[0]=='a') { qmdcdneg(md,qi); return 1; }
	return 0;
}
/************************************************************************/

/************************************************************************/
#define SIMP
/************************************************************************/
#define empty(md) \
        ((md->q==NULL)&& \
         (md->Clause[0]=='c'||md->Clause[0]=='d')&& \
         (md->Clause[1]==0))
/************************************************************************/
void qmdsimp3a(qmdata *md,qinfo *qi)
{
	int t0;
	qmdata *md0; /* considers empty clauses*/
	qmdata *md1; 

	if (md==NULL) return; t0=md->Clause[0];

	switch (t0) {
	    case 'a': 
	    case 'e': 
		qmdsimp3a(md->q,qi); break;
	    case 'c': 
	    case 'd': 
		if (md->q!=NULL) {
			qmdsimp3a(md->q,qi); 
			if (empty(md->q)) {
				if (md->q->Clause[0]==t0) {
					freeqmdata(md->q);
					if (md->next==NULL) { md->q=NULL; }
					else {
						md->q=md->next->q;
						md1=md->next;
						md->next=md->next->next;
						free(md1);
						qmdsimp3a(md,qi); 
					}
				} else {
					md->Clause[0]=md->q->Clause[0];
					md->Clause[1]=0;
					freeqmdatacontents(md);
				}
				break;
			}
		}
		for (md0=md;md0->next!=NULL;md0=md0->next) {
			if (md0->next->q!=NULL) {
				qmdsimp3a(md0->next->q,qi); 
				if (empty(md0->next->q)) {
					md1=md0->next->q;
					md0->next->q=md->q;
					md->q=md1;
					qmdsimp3a(md,qi); break;
				}
			}
		}
		break;
	}
}
/************************************************************************/
int qmdsimp3m(qmdata *md)
{
	int t0;
	qmdata *md0; 
	int m=0;

	if (md==NULL) return 0; t0=md->Clause[0];

	switch (t0) {
	    case 'a': 
	    case 'e': m+=qmdsimp3m(md->q); break;
	    case 'c': 
	    case 'd': if (md->q!=NULL) m+=qmdsimp3m(md->q); 
		      for (md0=md;md0->next!=NULL;md0=md0->next) 
		      if (md0->next->q!=NULL) m+=qmdsimp3m(md0->next->q); 
		      break;
	}
	return m+1;
}
/************************************************************************/
int qmdsimp3l(qmdata *md)
{
	int t0;
	qmdata *md0; 
	int l=0;

	if (md==NULL) return 0; t0=md->Clause[0];

	switch (t0) {
	    case 'a': 
	    case 'e': l+=intlen(md->Clause); l+=qmdsimp3l(md->q); break;
	    case 'c': 
	    case 'd': l+=intlen(md->Clause); 
		      if (md->q!=NULL) l+=qmdsimp3l(md->q); 
		      for (md0=md;md0->next!=NULL;md0=md0->next) 
		      if (md0->next->q!=NULL) l+=qmdsimp3l(md0->next->q); 
		      break;
	}
	return l;
}
/************************************************************************/
void qmdsimp3(qmdata *md,qinfo *qi)
{
	int t0;
	qmdata *md0; 
	int i,j,k;
        int *a1=(int *)calloc(qi->n+1,sizeof(int)); a1[0]=1000;

	qmdsimp3o(md,qi,a1); qmdsimp3a(md,qi); qmdsimp(md,qi);
        qi->m=(qmdsimp3m(md)); qi->l=qi->m-1+qmdsimp3l(md);
}
void qmdsimp3o(qmdata *md,qinfo *qi,int *a1)
{
	int t0;
	qmdata *md0; 
	int i,j,k;
	int *a0;

	if (md==NULL) return; t0=md->Clause[0];
	switch (t0) {
	    case 'a': 
	    case 'e': 
		qmdsimp3o(md->q,qi,a1); break;
	    case 'c': 
	    case 'd': 
/*
 intwrite(md->Clause,0,intlen(md->Clause),stdout);
*/
 		if ((k=intlen(md->Clause))>1) {
			j=intlen(a1)-1;
			for (i=1;md->Clause[i];i++) {
				if (k<=1) break;
/*
				if (k<=2) break;
*/
				if (intin(md->Clause[i],a1,1,j)) {
					if (t0=='c') {
/*
 infoi(300);
 intwrite(md->Clause,0,intlen(md->Clause),stdout);
 intwrite(a1,0,j,stdout);
*/
						k--;
						md->Clause[i]=md->Clause[k];
						md->Clause[k]=0;
						i--;
					} else {
#ifdef SIMP
						md->Clause[0]='c';
						md->Clause[1]=0;
						freeqmdatacontents(md);
						k=1; 
#else
						k=2; 
						md->Clause[1]=md->Clause[i];
						md->Clause[2]=0;
#endif
					}
				} else if (intin(-md->Clause[i],a1,1,j)) {
					if (t0=='d') {
/*
 infoi(400);
 intwrite(md->Clause,0,intlen(md->Clause),stdout);
 intwrite(a1,0,j,stdout);
*/
						k--;
						md->Clause[i]=md->Clause[k];
						md->Clause[k]=0;
						i--;
					} else {
#ifdef SIMP
						md->Clause[0]='d';
						md->Clause[1]=0;
						freeqmdatacontents(md);
						k=1; 
#else 
						k=2; 
						md->Clause[1]=md->Clause[i];
						md->Clause[2]=0;
#endif
					}
				}
			}
		}
		a0=NULL;
 		if ((k>1)) {
			if (t0=='c') a0=mdintegrateneg(a1,md->Clause,0);
			else a0=mdintegrateneg(a1,md->Clause,1);
			a1=a0;
		}
		qmdsimp3o(md->q,qi,a1);
		for (;md->next!=NULL;md=md->next) qmdsimp3o(md->next->q,qi,a1); 
		if (a0!=NULL) free(a0);
		break;
	}
}
/************************************************************************/

/************************************************************************/
#define SIMP
/************************************************************************/
void qmdsimp4signs(qmdata *md,qinfo *qi)
{
	int i,j=0,k=0;
	char *s0=(char *)calloc(2*qi->n+1,sizeof(char))+qi->n;
	char *s1=(char *)calloc(qi->n+1,sizeof(char));
	int *a1=(int *)calloc(qi->n+1,sizeof(int)); a1[0]=1000;

	qmdsimp4signs1(md,s0); 
	for (i=1;i<=qi->n;i++) {
		if (s0[i]==0&&s0[-i]==0) { s1[i]=1; j++; } 
		if (s0[i]==1&&s0[-i]==0) { k++; }
		if (s0[i]==0&&s0[-i]==1) { k++; }
	}
	infoi(qi->n); infoi(j); infoi(k);

	qmdsimp4pureliteral(md,qi,s0,a1);
	for (i=1;i<=qi->n;i++) if (s0[i]==0||s0[-i]==0) { s1[i]=1; } 
	qmdsimp4redundant(md,s1);
	free(s0-qi->n); free(s1); free(a1);
	qmdsimp3a(md,qi); 
	qmdsimp(md,qi);
        qi->m=(qmdsimp3m(md)); qi->l=qi->m-1+qmdsimp3l(md);
}

void qmdsimp4signs1(qmdata *md,char *s0)
{
	int i;
	int t0;

	if (md==NULL) return; t0=md->Clause[0];

	switch (t0) {
	    case 'a': 
	    case 'e': qmdsimp4signs1(md->q,s0); break; 
	    case 'c': 
	    case 'd': for (i=1;md->Clause[i];i++) s0[md->Clause[i]]=1;
	    	      qmdsimp4signs1(md->q,s0); 
		      for (;md->next!=NULL;md=md->next)
	    	      qmdsimp4signs1(md->next->q,s0); 
		      break;
	}
}
/************************************************************************/
void qmdsimp4redundant(qmdata *md,char *s1)
{
	int i,k;
	int t0;

	if (md==NULL) return; t0=md->Clause[0];

	switch (t0) {
	    case 'a': 
	    case 'e': k=intlen(md->Clause)-1;
		      for (i=1;md->Clause[i];i++) {
			if (s1[md->Clause[i]]) {
				md->Clause[i]= md->Clause[k];  md->Clause[k]=0; 
				k--; i--; //if (k==2) break;
			}
		      }
		      qmdsimp4redundant(md->q,s1); break; 
	    case 'c': 
	    case 'd': qmdsimp4redundant(md->q,s1); 
		      for (;md->next!=NULL;md=md->next)
	    	      qmdsimp4redundant(md->next->q,s1); 
		      break;
	}
}
/************************************************************************/
int qmdsimp3binresolution(qmdata *md)
{
	int t0;
	int i=0;
	qmdata *md0;
	qmdata *md1;

	if (md==NULL) return 0; t0=md->Clause[0];

	switch (t0) {
	    case 'a': 
	    case 'e': return qmdsimp3binresolution(md->q); 
	    case 'c': 
	    case 'd': if (md->q==NULL) return 0; 
	    	      if (md->q->q==NULL) {
		      	for (md0=md;md0->next!=NULL;md0=md0->next) {
			     i+=qmdsimp3bin1(md0->next->q,md0->q->Clause);
			}
		      }
		      for (md0=md;md0->next!=NULL;md0=md0->next) {
	    	      if (md0->next->q->q==NULL) {
		      	for (md1=md;md1->next!=NULL;md1=md1->next) 
			     i+=qmdsimp3bin1(md1->next->q,md0->next->q->Clause);
			}
		      }
		      return i;
	}
}
/************************************************************************/
int qmdsimp3bin1(qmdata *md,int *b0)
{
	int i,x=0;
	int *a0=md->Clause;

	if (md->q!=NULL) return 0;

	a0=md->Clause; 
	if (intlen(a0)!=3) { if (intlen(b0)!=3) return 0; a0=b0; b0=md->Clause;}

	for (i=1;b0[i];i++)
	if (b0[i]==a0[1]) { x=1; break; } else if (b0[i]==-a0[1]) { x=2; break;} 
	if (x==1) for (i=1;b0[i];i++) if (b0[i]==-a0[2]) { return 1; }
	if (x==2) for (i=1;b0[i];i++) if (b0[i]== a0[2]) { return 1; }
	return 0;
}
/************************************************************************/
void qmdsimp4pureliteral(qmdata *md,qinfo *qi,char *s0,int *a1)
{
	int t0;
	qmdata *md0; 
	int i,j,k;
	int *a0;

	if (md==NULL) return; t0=md->Clause[0];
	switch (t0) {
	    case 'a': 
	    case 'e': k=intlen(a1);
                      for (i=1;md->Clause[i];i++) {
                        if (s0[md->Clause[i]]==0) {
				if (t0=='a') a1[k]=md->Clause[i]; 
				else a1[k]=-md->Clause[i]; 
				k++; a1[k]=0;
                        } else if (s0[-md->Clause[i]]==0) {
				if (t0=='a') a1[k]=-md->Clause[i]; 
				else a1[k]=md->Clause[i]; 
				k++; a1[k]=0;
                        } 
                      }
		      qmdsimp4pureliteral(md->q,qi,s0,a1); break;
	    case 'c': 
	    case 'd': 
 		if ((k=intlen(md->Clause))>1) {
			j=intlen(a1)-1;
			for (i=1;md->Clause[i];i++) {
				if (k<=1) break;
/*
				if (k<=2) break;
*/
				if (intin(md->Clause[i],a1,1,j)) {
					if (t0=='c') {
						k--;
						md->Clause[i]=md->Clause[k];
						md->Clause[k]=0;
						i--;
					} else {
#ifdef SIMP
						md->Clause[0]='c';
						md->Clause[1]=0;
						freeqmdatacontents(md);
						k=1; 
#else
						k=2; 
						md->Clause[1]=md->Clause[i];
						md->Clause[2]=0;
#endif
					}
				} else if (intin(-md->Clause[i],a1,1,j)) {
					if (t0=='d') {
/*
 infoi(400);
 intwrite(md->Clause,0,intlen(md->Clause),stdout);
 intwrite(a1,0,j,stdout);
*/
						k--;
						md->Clause[i]=md->Clause[k];
						md->Clause[k]=0;
						i--;
					} else {
#ifdef SIMP
						md->Clause[0]='d';
						md->Clause[1]=0;
						freeqmdatacontents(md);
						k=1; 
#else 
						k=2; 
						md->Clause[1]=md->Clause[i];
						md->Clause[2]=0;
#endif
					}
				}
			}
		}
		qmdsimp4pureliteral(md->q,qi,s0,a1);
		for (;md->next!=NULL;md=md->next) qmdsimp4pureliteral(md->next->q,qi,s0,a1); 
		break;
	}
}
/************************************************************************/

/************************************************************************/
void freeqmdatacontents(qmdata *md)
{
        if (md->q!=NULL) freeqmdata(md->q); md->q=NULL;
        if (md->next!=NULL) freeqmdata(md->next); md->next=NULL;
}
/************************************************************************/
void freeqmdata(qmdata *md)
{
        if (md->q!=NULL) freeqmdata(md->q);
        if (md->next!=NULL) freeqmdata(md->next);
        if (md->Clause!=NULL) free(md->Clause); free(md);
}
/************************************************************************/
int qmdwrite(qmdata *md,qinfo *qi,FILE *g)
{
	fprintf(g,"QBF\n%i\n",qi->n); qmdwrite1(md,g,0); fprintf(g,"QBF\n"); 
}
int qmdwrite1(qmdata *md,FILE *g,int mode)
{
	int t0;
	if (md==NULL) return; t0=md->Clause[0];
	switch (t0) {
		case 'a': 
		case 'e': 
			if (mode!='q') fprintf(g,"q\n"); 
			fprintf(g,"%c ",t0); 
			intwrite(md->Clause+1,0,intlen(md->Clause+1)-1,g);
			qmdwrite1(md->q,g,'q');
			if (mode!='q') fprintf(g,"/q\n"); 
			break;
		case 'c': 
		case 'd': 
			fprintf(g,"%c\n",t0); 
			pnintwrite(md->Clause+1,g);
			qmdwrite1(md->q,g,0);
			for (;md->next!=NULL;md=md->next)
				qmdwrite1(md->next->q,g,0);
			fprintf(g,"/%c\n",t0); 
			break;
	}
}
/*******************************************************************/
void pnintwrite(int *ss,FILE *g)
{
        int i=0;
        for (i=0;ss[i];i++) if (ss[i]>0) { fprintf(g,"%i ",ss[i]); }
        fprintf(g,"\n");
        for (i=0;ss[i];i++) if (ss[i]<0) { fprintf(g,"%i ",-ss[i]); }
        fprintf(g,"\n");
}
/*******************************************************************/
#define single(md) \
	((md->q==NULL)&& \
	 (md->Clause[0]=='c'||md->Clause[0]=='d')&& \
	 (md->Clause[2]==0))
/************************************************************************/
void qmdsimp(qmdata *md,qinfo *qi)
{
	int t0;
	qmdata *md0; /* empty clauses are not considered */

	if (md==NULL) return; t0=md->Clause[0];
	switch (t0) {
	    case 'a': 
	    case 'e': 
/*
		if (md->q->Clause[0]==t0) { 
			md->Clause=mdintegrate(md->Clause,md->q->Clause); 
			md0=md->q; md->q=md->q->q; free(md0->Clause); free(md0);
			qmdsimp(md,qi); break;
		}
*/
		qmdsimp(md->q,qi); break;
	    case 'c': 
	    case 'd': 
		while (md->q!=NULL) {
			if (md->q->Clause[0]==t0||single(md->q)) {
				qi->m--; qi->l-=2;
				mdmoveup1(md,md,md->q); continue;
			}
			qmdsimp(md->q,qi); break;
		}
		for (md0=md;md->next!=NULL;) {
			if (md->next->q==NULL) {
				free(md->next); md->next=NULL; break;
			}
			if (md->next->q->Clause[0]==t0||single(md->next->q)) {
				qi->m--; qi->l-=2;
				mdmoveup1(md0,md->next,md->next->q); 
				continue;
			}
			qmdsimp(md->next->q,qi); md=md->next;
		}
		break;
	}
}
/************************************************************************/
void mdmoveup1(qmdata *md0,qmdata *md,qmdata *mdq)
{
	qmdata *mdn=md->next;

	md0->Clause=mdintegrate(md0->Clause,mdq->Clause);

	if (mdq->q!=NULL) {
		md->q=mdq->q;
		md->next=mdq->next;
		while (md->next!=NULL) md=md->next; md->next=mdn;
		free(mdq->Clause); free(mdq);
	} else if (mdn!=NULL) {
		md->q=mdn->q; md->next=mdn->next; free(mdn);
		free(mdq->Clause); free(mdq);
	} else {
		free(mdq->Clause); free(mdq); 
		md->q=NULL;
	}
}

int *mdintegrate(int *a1,int *a2)
{
	int x=intlen(a1);
	int y=intlen(a2);
	int i;

	a1=(int *)realloc(a1,(x+y)*sizeof(int));
	for (i=1;i<=y;i++) a1[x+i-1]=a2[i]; return a1;
}
int *mdintegrateneg(int *a1,int *a2,int neg)
{
	int x=intlen(a1);
	int y=intlen(a2);
	int *a0=(int *)malloc((x+y)*sizeof(int));
	int i;

	for (i=0;i<x;i++) a0[i]=a1[i]; 
	if (neg) for (i=1;i<=y;i++) a0[x+i-1]=-a2[i]; 
	else for (i=1;i<=y;i++) a0[x+i-1]=a2[i]; 
	return a0;
}
int qmdlen(qmdata *md)
{
	int k=0;

	if (md==NULL) return 0; 
	switch (md->Clause[0]) {
		case 'a': 
		case 'e': k=intlen(md->Clause+1)+1; k+=qmdlen(md->q)+1;
			  break;
		case 'c': 
		case 'd': k=intlen(md->Clause+1)+1; 
			  if (md->q==NULL) break; k+=qmdlen(md->q)+1; 
			  for (;md->next!=NULL;md=md->next)
			   	k+=qmdlen(md->next->q)+1;
			  break;
	}
	return k;
}
/*******************************************************************/
/*
int qmdm(qmdata *md)
{
	int k=0;

	if (md==NULL) return 0; 
	switch (md->Clause[0]) {
		case 'a': 
		case 'e': k+=qmdm(md->q)+1;
			  break;
		case 'c': 
		case 'd': k++; if (md->q==NULL) break; k+=qmdm(md->q); 
			  for (;md->next!=NULL;md=md->next)
			   	k+=qmdm(md->next->q);
			  break;
	}
	return k;
}
/*******************************************************************/
int qmdclauses(qmdata *md)
{
        int k=0;

        if (md==NULL) return 0;
        switch (md->Clause[0]) {
                case 'a':
                case 'e': k+=qmdclauses(md->q); break;
                case 'c':
                case 'd': if (md->q==NULL) break; k+=qmdclauses(md->q);
                          for (;md->next!=NULL;md=md->next)
                                k+=qmdclauses(md->next->q);
                          break;
        }
        return k+1;
}
/*******************************************************************/








/**********************************************************************/
void quicksort_abf(float *a,int *b,int l,int r) /* decreasing */
{
	float v,t0;
        int i,j,t1;
        if (r>l) {
                v=a[r]; i=l-1;j=r;
                for (;;) {
			for (i++;(a[i]>v);i++);
                        for (j--;j>=l&&(a[j]<v);j--); if (j<l) j=1;
/*
                        while (a[++i] > v); while (a[--j] < v);
*/
                        if (i>=j) break;
                        t0=a[i]; a[i]=a[j];a[j]=t0; t1=b[i]; b[i]=b[j];b[j]=t1;
                }
                t0=a[i]; a[i]=a[r];a[r]=t0;
                t1=b[i]; b[i]=b[r];b[r]=t1;
                quicksort_abf(a,b,l,i-1); quicksort_abf(a,b,i+1,r);
        }
}
/*******************************************************************/

/*******************************************************************/
void intwrite(int *ss,int a,int b,FILE *g)
{
        int i;
        for (i=a;i<=b;i++) fprintf(g,"%i ",ss[i]); fprintf(g,"\n");
}
/*******************************************************************/
void charwrite(char *ss,int a,int b,FILE *g)
{
        int i;
        for (i=a;i<=b;i++) fprintf(g,"%i ",ss[i]); fprintf(g,"\n");
}
/*******************************************************************/
void prettyintwrite(int *ss,int a,int b,FILE *g)
{
        int i,j;
        for (j=0,i=a;i<=b;i++) {
		fprintf(g,"%i ",ss[i]); 
		if (++j%10==0) fprintf(g,"\n");
	}
	fprintf(g,"\n");
}
/*******************************************************************/
void cnfstringwrite(int *mstring,int mstringc,FILE *g)
{
        int i;
        for (i=0;i<mstringc;i++)
        if (mstring[i]) fprintf(g," %i ",mstring[i]);
        else fprintf(g," %i\n",mstring[i]);
}
/*******************************************************************/
