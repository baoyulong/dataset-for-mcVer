#include "bp2q.h"
#include "tlf.h"

/************************************************************************/
#define bpi bp->right->left
#define bpt bp->right->right->left
/************************************************************************/
#define R1					/*not as good as ORIGINAL*/
#define R2x					/*not as good as R1*/
#define ORIGINALx
/************************************************************************/
qmdata *bp2dm(qinfo *qi,tree *bp,tree *fness,tree *bs,int booln,int k)
{
	int i,j,sj,cur,si;
	qmdata *m0,*m0a,*m1,*m1a,*md;
	int *p0;

	if (k==0) return tree2md(treeconst(0));

#ifdef R2
	m1=tree2md(bpi);
	for (i=0;i<=k-1;i++) {
                m0=tree2md(bpt); mdstep1(m0,i*booln,i*booln,booln); 
		m1=mdcompose('c',m0,m1);
        m1a=(qmdata *)calloc(1,sizeof(qmdata));
        p0=m1a->Clause=(int *)malloc((booln+2)*sizeof(int));
        *p0++='e'; for (j=i*booln+1,sj=j-1+booln;j<=sj;j++) *p0++=j; 
	*p0=0; m1a->q=m1; m1=m1a;
		m0=tree2md(bpi); mdrepl(m0,0,i+1,booln); 
		m1=mdcompose('d',m0,m1);
	}
	mdrepl(m1,k,2*(k+1),booln); 
	cur=qi->n=(k+1)*booln; m0=bp2qPATH(qi,bp,booln,k+1,cur,1);
        si=(k+1)*booln;
#else
#ifdef ORIGINAL
	qi->n=0; m1=tree2md(bpi); mdrepl(m1,0,2*(k+1),booln); 
	for (i=1;i<=k+1;i++) {
		cur=qi->n=0;
		if (i==k+1) { cur=qi->n=(k+1)*booln; }
		m0=bp2qPATH(qi,bp,booln,i,cur,1);
		if (i==k+1) break; 
		mdrepl(m0,i,2*(k+1),booln); m1=mdcompose('d',m1,m0);
	}
#endif
#ifdef R1
	m1=tree2md(bpi);
	for (i=0;i<=k-1;i++) {
                m0=tree2md(bpt); mdstep1(m0,i*booln,i*booln,booln); 
		m1=mdcompose('c',m0,m1);
		m0=tree2md(bpi); mdrepl(m0,0,i+1,booln); 
		m1=mdcompose('d',m0,m1);
	}
	mdrepl(m1,k,2*(k+1),booln); 
	cur=qi->n=(k+1)*booln; m0=bp2qPATH(qi,bp,booln,k+1,cur,1);
#endif

        m1a=(qmdata *)calloc(1,sizeof(qmdata));
        p0=m1a->Clause=(int *)malloc(((k+1)*booln+2)*sizeof(int));
        *p0++='e'; for (i=1,si=(k+1)*booln;i<=si;i++) *p0++=i; *p0=0; m1a->q=m1;
#endif

        m0a=(qmdata *)calloc(1,sizeof(qmdata));
        p0=m0a->Clause=(int *)malloc(((k+1)*booln+2)*sizeof(int));
        *p0++='e'; for (i=si+1,si+=si;i<=si;i++) *p0++=i; *p0=0; m0a->q=m0;

        md=(qmdata *)calloc(1,sizeof(qmdata));
        p0=md->Clause=(int *)malloc((booln+2)*sizeof(int));
        *p0++='a'; for (i=si+1,si+=booln;i<=si;i++) *p0++=i; *p0=0; 
	md->q=mdcompose('I',m0a,m1a);

	return md;
}
/************************************************************************/
void mdrepl(qmdata *m0,int i,int j,int booln)
{
        int *p0;
        int i0,i1,j0;
        if (m0==NULL) return;

        i0=i*booln; i1=(i+1)*booln; j0=(j-i)*booln;

        for (p0=m0->Clause+1;*p0;p0++) {
                if (*p0>0) { if (*p0>i0&&*p0<=i1) *p0+=j0; }
                else if (*p0>=-i1&&*p0<-i0) *p0-=j0;
        }
        mdrepl(m0->q,i,j,booln);
        while (m0->next!=NULL) { mdrepl(m0->next->q,i,j,booln); m0=m0->next; }
}
/************************************************************************/


/************************************************************************/
extern int opLIN;
extern FILE *vdslog;
/************************************************************************/
qmdata *bp2q(qinfo *qi,tree *bp,tree *fness,tree *bs,int booln,int k,int bv);
qmdata *mdtrue(int cur,char a,char c);
/************************************************************************/
qmdata *bp2q(qinfo *qi,tree *bp,tree *fness,tree *bs,int booln,int k,int bv)
{
	qmdata *md;
	int newbs=0;

	if (k==0) return bp2qk0(qi,bp,fness,bs,booln,bv);
/*
	if (k==0) { bs=k0simplify1(treecopy(bs)); newbs=1; }
*/


	

/* if (k==0) { return mdtrue(1,'e','d'); } /*zwh20141105*/

	if (!bv) { newbs=1; bs=treecopy(bs); bs=TLFnegate(bs);}

	if (k==0) { if (newbs==0) {newbs=1;bs=treecopy(bs);} bs=bp2qclean(bs);}
	 /*there are some problem for QBFSAT, k>=1 is assumed, zwh20141115*/

	qi->n=0; md=bp2q0(qi,bp,fness,bs,booln,k,0,0); 
        if (vdslog!=NULL) { fprintf(vdslog,"\nSTA %i\n",qi->n/booln); }

	if (opLIN&128) { prep0md0(md); prep0md(md); qmdsimp(md,qi); }
/*
	qmdsimp(md,qi);
*/
	if (newbs) treefree(bs); 
	
	return md;
}
qmdata *bp2q0(qinfo *qi,tree *bp,tree *fness,tree *bs,int booln,int k,int cur,int lv)
{
        qmdata *m0,*m1,*m2,*mfair;
	int k1;
	int a,b,c;
        qmdata *ek=NULL;
	char dc='d';
	char ea='e';

        switch (bs->node[0]) {
                case 'E': if ((opLIN&64)&&(k>0&&isLIN(bs)&&(depLIN(bs))>1))
		  	  return bp2qlin(qi,bp,fness,bs,booln,k,cur,lv);
                case 'A': k1=k; lv++; 
			  if (lv==1) cur=qi->n;
			  if (bs->node[0]=='A') ea='a';
			  if (fness==NULL&bs->right->node[0]=='X') {
			     if (k==0) { /*problem---zwh20141105*/ }
			     k1=1;
			  }
			  m0=bp2qPATH(qi,bp,booln,k1,cur,lv); 
			  a=cur; b=qi->n-k1*booln; c=qi->n-booln; 
			  m1=bp2q0(qi,bp,fness,bs->right,booln,k,cur,lv);
/*
 qmdsimp(m0,qi);
 qmdsimp(m1,qi);
 qmdwrite((m0),qi,stdout);
 qmdwrite((m1),qi,stdout);
*/
			  mfair=bp2qfpc(ea,qi,bp,fness,booln,k,cur,b,lv,0);
			  m0=mdcompose('c',mfair,m0);
			  m1=bp2qEA(ea,qi,booln,k1,cur,b,lv,m0,m1);
			  return m1;
                case 'X': a=cur; b=qi->n-booln; 
			  return bp2q0(qi,bp,fness,bs->right,booln,k,b,lv);
                case 'F': 
                case 'G': a=cur; c=qi->n-booln; 
			  if (k==0) b=cur; else b=qi->n-k*booln; 
			  switch (bscase(bs)) {
			  case 'G': ek=bp2qeqPATH(a,b,c,booln);
			  case 'F': return bp2qFG(qi,bp,fness,bs,booln,k,cur,b,lv,ek);
			  case 'c': dc='c';
			  case 'd': ek=bp2qeqPATH(a,b,c,booln);
				    return bp2qGFFG(dc,qi,bp,fness,bs,booln,k,cur,b,lv,0,1,ek);
			  }
                case 'U': 
                case 'R': a=cur; c=qi->n-booln; 
			  if (k==0) b=cur; else b=qi->n-k*booln; 
			  if (bs->node[0]=='R') ek=bp2qeqPATH(a,b,c,booln);
			  return bp2qUR(qi,bp,fness,bs,booln,k,cur,b,lv,0,ek);
                case '&': m0=bp2q0(qi,bp,fness,bs->left,booln,k,cur,lv);
			  m1=bp2q0(qi,bp,fness,bs->right,booln,k,cur,lv);
                          return mdcompose('c',m0,m1);
                case '|': m0=bp2q0(qi,bp,fness,bs->left,booln,k,cur,lv);
			  m1=bp2q0(qi,bp,fness,bs->right,booln,k,cur,lv);
                          return mdcompose('d',m0,m1);
                case '!':
                default:
			m0=tree2md(bs); mdstep1(m0,cur,cur,booln);
			return m0;
        }
}
/************************************************************************/
qmdata *bp2qEA(char ea,qinfo *qi,int booln,int k,int cur,int cur1,int lv,qmdata *m0,qmdata *m1)
{
	qmdata *md;
	int *p0;
	int i;

	if (lv==1) {
		cur=cur1-booln;
		md=(qmdata *)calloc(1,sizeof(qmdata));
		p0=md->Clause=(int *)malloc((booln+2)*sizeof(int));
		*p0++=ea; for (i=cur+1;i<=cur+booln;i++) *p0++=i; *p0=0;
		md->q=bp2qEA(ea,qi,booln,k,cur,cur1,0,m0,m1);
	} else if (k==0) {
		if (ea=='e') md=mdcompose('c',m0,m1); 
			else md=mdcompose('I',m0,m1); 
	} else {
		md=(qmdata *)calloc(1,sizeof(qmdata));
		p0=md->Clause=(int *)malloc((booln+2)*sizeof(int));
		*p0++=ea; for (i=cur1+1;i<=cur1+booln;i++) *p0++=i; *p0=0;
		md->q=bp2qEA(ea,qi,booln,k-1,cur,cur1+booln,0,m0,m1);
	}
	return md;
}
qmdata *bp2qFG(qinfo *qi,tree *bp,tree *fness,tree *bs,int booln,int k,int cur,int cur1,int lv,qmdata *ek)
{
	int x0,i;
        qmdata *md0,*md,*md1;
	int cur0;

	if (ek==NULL) { x0='d'; } else { x0='c'; }

        md=(qmdata *)calloc(1,sizeof(qmdata));
        md->Clause=(int *)malloc((2)*sizeof(int));
        md->Clause[0]=x0; md->Clause[1]=0;

        md0=md;
        for (i=0;i<=k;i++) {
		if (i==0) { cur0=cur; } else { cur0=cur1; cur1+=booln; }
		md->q=bp2q0(qi,bp,fness,bs->right,booln,k,cur0,lv);
                md->next=(qmdata *)calloc(1,sizeof(qmdata)); md1=md;
                md=md->next;
        }
	if (ek==NULL) { free(md); md1->next=NULL; } else md->q=ek;
        return md0;
}
qmdata *bp2qUR(qinfo *qi,tree *bp,tree *fness,tree *bs,int booln,int k,int cur,int cur1,int lv,int start,qmdata *ek)
{
	int x0,x1;
        qmdata *m0,*m1,*m2;
	int cur0;

	if (start==0) { cur0=cur; } else { cur0=cur1; cur1+=booln; }
	if (ek==NULL) { x0='d'; x1='c'; } else { x0='c'; x1='d'; }

	m0=bp2q0(qi,bp,fness,bs->right,booln,k,cur0,lv);
	if (start==k) { 
		if (ek==NULL) return m0;
		m1=bp2q0(qi,bp,fness,bs->left,booln,k,cur0,lv); m2=ek;
		return mdcompose(x0,m0,mdcompose(x1,m1,m2)); 

	} 
	m1=bp2q0(qi,bp,fness,bs->left,booln,k,cur0,lv);
	m2=bp2qUR(qi,bp,fness,bs,booln,k,cur,cur1,lv,start+1,ek);

	return mdcompose(x0,m0,mdcompose(x1,m1,m2)); 
}
/************************************************************************/
qmdata *bp2qGFFG(char dc,qinfo *qi,tree *bp,tree *fness,tree *bs,int booln,int k,int cur,int cur1,int lv,int start,int start2,qmdata *ek)
{
	int j;
        qmdata *m0,*m1,*m2;
	int curj;
	int curii,curll;
	tree *t0;

	if (start==k) return ek;
	if (start==0) { curii=cur; } else { curii=cur1+(start-1)*booln; }
	curll=cur1+(start2-1)*booln;
	t0=eqstate(1,curii/booln,1,curll/booln,-1);
        eqstateconstruct(t0,booln); m0=tree2md(t0); treefree(t0);

	j=start+1;
	curj=cur1+(j-1)*booln;
	m2=bp2q0(qi,bp,fness,bs->right->right,booln,k,curj,lv);

	for (j=start+2;j<=start2;j++) {
		curj=cur1+(j-1)*booln;
		m1=bp2q0(qi,bp,fness,bs->right->right,booln,k,curj,lv);
		m2=mdcompose(dc,m2,m1); 
	}
	m2=mdcompose('I',m0,m2); 

	if (start2<k) { start2++; } else { start++; start2=start+1; }
	m1=bp2qGFFG(dc,qi,bp,fness,bs,booln,k,cur,cur1,lv,start,start2,ek);
	return mdcompose('c',m2,m1); 
}
/************************************************************************/
char bscase(tree *bs)
{
	if (bs->node[0]=='F'&&bs->right->node[0]=='G') return 'c'; 
	if (bs->node[0]=='G'&&bs->right->node[0]=='F') return 'd'; 
	return bs->node[0];
}
/************************************************************************/
qmdata *mdtrue(int cur,char ae,char cd)
{
	qmdata *md=(qmdata *)calloc(1,sizeof(qmdata));
	int *p0=md->Clause=(int *)malloc((1+2)*sizeof(int));
	qmdata *mdq=(qmdata *)calloc(1,sizeof(qmdata));
	int *p1=mdq->Clause=(int *)malloc((2+2)*sizeof(int));

	*p0++=ae; *p0++=cur; *p0=0;
	*p1++=cd; *p1++=cur; *p1++=-cur; *p1=0;

	md->q=mdq; return mdq;
}
/************************************************************************/
tree *bp2qclean(tree *bs)
{
	tree *bs1;

	if (bs==NULL) return bs; 

	/*return treeconstab(1,1); /*???*/

        switch (bs->node[0]) {
                case 'E': 
                case 'A': 
        		bs1=bs->right;
        		if (bs1->node[0]=='X') {
/*
        		if (bs1->node[0]=='X'||bs1->node[0]=='F') {
        			bs1->node[0]='G'; 
 infos("SSSS\n");
 treewrite(bs,stdout);
 infos("SS\n");
 treewrite(treeconstab(0,1),stdout);
*/
				treefree(bs); return treeconstab(0,1);
			}
	}
	bs->left=bp2qclean(bs->left); bs->right=bp2qclean(bs->right);
	return bs;
}
/************************************************************************/


/************************************************************************/
qmdata *bp2qk0(qinfo *qi,tree *bp,tree *fness,tree *bs,int booln,int bv);
/************************************************************************/
qmdata *bp2qk0(qinfo *qi,tree *bp,tree *fness,tree *bs1,int booln,int bv)
{
	qmdata *md;
	int newbs=0;
	tree *bs=treecopy(bs1);

	if (bv) bs=k0simplify(bs); 
	else { bs=k0simplify(TLFnegate(bs)); }

	if (bv) bs=treecompose("A",NULL,treecompose("F",NULL,bs));
	else bs=treecompose("A",NULL,treecompose("F",NULL,bs));

	/*this approach does not work, and has been replaced.*/

/*
	this approach does not work, and has been replaced.

	bs=treecompose(ea,NULL,treecompose("F",NULL,bs));
	without this QBF 0 instead of QBF qi->n is produced

	if (bv) bs=(treecopy(bs1)); 
	else bs=(TLFnegate(treecopy(bs1))); 
*/
	qi->n=0; md=bp2q0(qi,bp,fness,bs,booln,0,0,0); 

	treefree(bs); return md;
}
/************************************************************************/
tree *k0simplify(tree *bs)
{
	tree *bs1;

	if (bs==NULL) return bs; 
        switch (bs->node[0]) {
        case 'E': 
        case 'A': 
        	bs1=bs->right;
        	switch (bs1->node[0]) {
                case 'F': 
			if (bs1->right->node[0]!='G') { 
				bs1=k0simplify(treecopy(bs1->right)); 
				treefree(bs); return bs1;
			} 
                case 'X': 
                case 'G': treefree(bs); return treeconstab(0,1);
                case 'U': 
			bs1=k0simplify(treecopy(bs1->right)); 
			treefree(bs); return bs1;
                case 'R': 
			bs1=treecompose("&",
				k0simplify(treecopy(bs1->left)),
				k0simplify(treecopy(bs1->right))); 
			treefree(bs); return bs1;
		}
	}
	bs->left=k0simplify(bs->left);
	bs->right=k0simplify(bs->right); return bs;
}
/************************************************************************/
tree *k0simplify1(tree *bs,int bv)
{
	tree *bs1;

	if (bs==NULL) return bs; 
        switch (bs->node[0]) {
        case 'E': 
        case 'A': 
		bs1=bs->right; 
		bs->right=k0simplify(treecopy(bs->right)); 
        	if (bv) bs->node[0]='A'; else bs->node[0]='E';
		return bs;
	}
	bs->left=k0simplify1(bs->left,bv); bs->right=k0simplify1(bs->right,bv); 
	return bs;
}
/************************************************************************/
tree *k0simplify2(tree *bs,int bv)
{
	if (bv) { bs=k0simplify1(bs,bv); }
	else { bs=TLFnegate(bs); bs=k0simplify1(bs,bv); bs=TLFnegate(bs); }
	return bs;
}
/************************************************************************/

/*******************************************************************/
qmdata *bp2qmakef(qinfo *qi,tree *bs,int booln,int k,int cur,int ll,int k1,int i,char c)
{
	int x;
	qmdata *m0,*m1;

       	m0=bp2qlin2(qi,bs,booln,k,cur,ll,k1,i);
        for (x=i+1;x<=k1;x++) {
              	m1=bp2qlin2(qi,bs,booln,k,cur,ll,k1,x);
              	m0=mdcompose(c,m0,m1);
	}
        return m0;
}
/*******************************************************************/

/************************************************************************/
#define bpt bp->right->right->left
/*******************************************************************/
qmdata *bp2qlin(qinfo *qi,tree *bp,tree *fness,tree *bs,int booln,int k,int cur,int lv)
{
        qmdata *m0,*m1,*m2,*mfair;
	int k1;
	int a,b,c;
	char ea='e';

 infos("bp2q - LIN");

        lv++; 
	if (lv==1) cur=qi->n;
	k1=kdepLIN(bs,k);

  	m0=bp2qPATH(qi,bp,booln,k1,cur,lv); 
	a=cur; b=qi->n-k1*booln; c=qi->n-booln; 
	m1=bp2qlin1(qi,bp,fness,bs,booln,k,cur,-1,k1);
	m1=bp2qEA(ea,qi,booln,k1,cur,b,lv,m0,m1);
	return m1;
}
/************************************************************************/
qmdata *bp2qlin1(qinfo *qi,tree *bp,tree *fness,tree *bs,int booln,int k,int cur,int lstart,int k1)
{
	int i;
	int a,b,c;
        qmdata *m0,*m1,*m2,*mfair;

	if (lstart==-1) {
		if (fness!=NULL) {
		   return bp2qlin1(qi,bp,fness,bs,booln,k,cur,lstart+1,k1);
		} else {
		   m1=bp2qlin2(qi,bs,booln,k,cur,lstart,k1,0);
		   m2=bp2qlin1(qi,bp,fness,bs,booln,k,cur,lstart+1,k1);
		   /*zwh201503 (possible loops start from 1 instead of 0)*/
		   m2=mdcompose('d',m1,m2); return m2;
		}
	} else { 
		a=qi->n-booln;
		if (lstart==0) b=cur-booln; else b=a-(k1-lstart)*booln-booln;
		m0=tree2md(bpt); mdstep1(m0,a,b,booln);
/*
		mfair=bp2qlinfpc(qi,bp,fness,booln,k,cur,k1,lstart);
		m0=mdcompose('c',mfair,m0);
 infoi(checkmdvalue(m0,1,35,0));
*/
		m1=bp2qlin2(qi,bs,booln,k,cur,lstart,k1,0);
		m1=mdcompose('c',m0,m1);
		if (lstart==k1) return m1;
		m2=bp2qlin1(qi,bp,fness,bs,booln,k,cur,lstart+1,k1);
		m2=mdcompose('d',m1,m2); return m2;
	}
}
/*******************************************************************/
qmdata *bp2qlin2(qinfo *qi,tree *bs,int booln,int k,int cur,int ll,int k1,int i)
{
        qmdata *m0,*m1,*m2,*m3,*mfair;

	switch (bs->node[0]) {
		case 'E': return bp2qlin2(qi,bs->right,booln,k,cur,ll,k1,i);
		case 'X': if (k1==i&&ll>=0) i=ll; else i=i+1;
			  if (i>k1) return tree2md(treeconst(0)); 
			  return bp2qlin2(qi,bs->right,booln,k,cur,ll,k1,i);
		case 'G': if (ll<0) return tree2md(treeconst(0)); 
			  if (ll<i) i=ll;
			  m0=bp2qmakef(qi,bs->right,booln,k,cur,ll,k1,i,'c');
			  return m0;
		case 'F': if (i>ll&&ll>=0) i=ll;
			  m0=bp2qmakef(qi,bs->right,booln,k,cur,ll,k1,i,'d');
			  return m0;
		case 'U': bs->node[0]++; 
			  m0=bp2qlin2(qi,bs,booln,k,cur,ll,k1,i);
			  bs->node[0]--;
			  if (!(i>ll&&ll>=0)) return m0;
			  m1=bp2qmakef(qi,bs->left,booln,k,cur,ll,k1,i,'c');
			  bs->node[0]++; 
			  m2=bp2qlin2(qi,bs,booln,k,cur,ll,k1,i-1);
			  bs->node[0]--;
			  return mdcompose('d',m0,mdcompose('c',m1,m2));
		case 'R': bs->node[0]++; 
			  m0=bp2qlin2(qi,bs,booln,k,cur,ll,k1,i);
			  bs->node[0]--;
			  if (!(ll>=0)) return m0;
			  m1=bp2qmakef(qi,bs->left,booln,k,cur,ll,k1,i,'c');
		          if (i<=ll) return mdcompose('d',m0,m1);
			  bs->node[0]++; 
			  m2=bp2qlin2(qi,bs->right,booln,k,cur,ll,k1,i-1);
			  bs->node[0]--;
			  m3=bp2qmakef(qi,bs->right,booln,k,cur,ll,k1,i-1,'c');
			  m2=mdcompose('d',m2,m3);
			  return mdcompose('d',m0,mdcompose('c',m1,m2));
		case 'V': m0=bp2qlin2(qi,bs->right,booln,k,cur,ll,k1,i);
			  if (i==k1) return m0;
			  m1=bp2qlin2(qi,bs->left,booln,k,cur,ll,k1,i);
			  m2=bp2qlin2(qi,bs,booln,k,cur,ll,k1,i+1);
			  return mdcompose('d',m0,mdcompose('c',m1,m2));
		case 'S': m0=bp2qlin2(qi,bs->right,booln,k,cur,ll,k1,i);
			  m1=bp2qlin2(qi,bs->left,booln,k,cur,ll,k1,i);
			  if (i==k1) return mdcompose('c',m0,m1);
			  m2=bp2qlin2(qi,bs,booln,k,cur,ll,k1,i+1);
			  return mdcompose('c',m0,mdcompose('d',m1,m2));
		case '&': m0=bp2qlin2(qi,bs->left,booln,k,cur,ll,k1,i);
			  m1=bp2qlin2(qi,bs->right,booln,k,cur,ll,k1,i);
			  return mdcompose('c',m0,m1);
		case '|': m0=bp2qlin2(qi,bs->left,booln,k,cur,ll,k1,i);
			  m1=bp2qlin2(qi,bs->right,booln,k,cur,ll,k1,i);
			  return mdcompose('d',m0,m1);
		case '!': 
		default:  
			  if (i>0) { cur=qi->n-(1+k1-i)*booln; }
		  	  m0=tree2md(bs); mdstep1(m0,cur,cur,booln);
			  return m0;
	}
}
/*******************************************************************/

/************************************************************************/
qmdata *bp2qfpc(char ea,qinfo *qi,tree *bp,tree *fness,int booln,int k,int cur,int cur1,int lv,int start)
{
	int n;
	tree *t0;
	tree **fn;
	qmdata *fpc;

/*
 return NULL;
 treewrite(fness,stdout);
*/

	if (fness==NULL) return NULL;

	if (k==0) {
		t0=treeconstab(ea=='a',1); fpc=tree2md(t0); 
		treefree(t0); return fpc;
	}

	for (n=1,t0=fness;t0->node[0]==';';t0=fness->right) n++;
	fn=(tree **)malloc((n+1)*sizeof(tree *));
	for (n=0,t0=fness;t0->node[0]==';';t0=fness->right) { 
		fn[n]=t0->left; n++; 
	}
	fn[n]=t0; n++; fn[n]=NULL;
	
	if (lv==1) cur=cur1-booln;
	if (ea=='a') fpc=bp2qwfpc(qi,bp,fness,fn,booln,k,cur,cur1,lv,0);
	if (ea=='e') fpc=bp2qsfpc(qi,bp,fness,fn,booln,k,cur,cur1,lv,0);

	free(fn); return fpc;
}
/************************************************************************/
qmdata *bp2qsfpc(qinfo *qi,tree *bp,tree *fness,tree **fn,int booln,int k,int cur,int cur1,int lv,int start)
{
	int jj,j;
        qmdata *m0,*m1,*m2;
	int curj;
	int curii,curll;
	tree *t0;

	if (start==0) { curii=cur; } else { curii=cur1+(start-1)*booln; }

	curll=cur1+(k-1)*booln;
	t0=eqstate(1,curii/booln,1,curll/booln,-1);
        eqstateconstruct(t0,booln); m0=tree2md(t0); treefree(t0);

	j=start+1;
	curj=cur1+(j-1)*booln;
/*
	m2=bp2q0(qi,bp,fness,fn[0],booln,k,curj,lv);
*/
	m2=tree2md(fn[0]); mdstep1(m2,curj,curj,booln);

	for (jj=1;fn[jj]!=NULL;jj++) {
		for (j=start+2;j<=k;j++) {
			curj=cur1+(j-1)*booln;
/*
			m1=bp2q0(qi,bp,fness,fn[jj],booln,k,curj,lv);
*/
			m1=tree2md(fn[jj]); mdstep1(m1,curj,curj,booln);
			m2=mdcompose('d',m2,m1); 
		}
	}

	m2=mdcompose('c',m0,m2); 

	if (start==k-1) return m2;
	m1=bp2qsfpc(qi,bp,fness,fn,booln,k,cur,cur1,lv,start+1);
	return mdcompose('c',m2,m1); 
}
/************************************************************************/
qmdata *bp2qntm(int booln,int k,int cur,int cur1,int i,int x,int y)
{
	tree *t0;
        qmdata *m0,*m1,*m2;
	int curii,curll;
	
	if (y==0||x==k+1) {
		if (y==0) t0=treeconstab(0,1); else t0=treeconstab(1,1);
		m1=tree2md(t0); treefree(t0); 
		return m1;
	}

	if (i==0) { curii=cur; } else { curii=cur1+(i-1)*booln; }

	curll=cur1+(x-1)*booln;
	t0=eqstate(1,curii/booln,1,curll/booln,-1);
        eqstateconstruct(t0,booln); 
	m0=mdnegate(tree2md(t0)); m1=tree2md(t0); treefree(t0);

	m2=bp2qntm(booln,k,cur,cur1,i,x+1,y); m0=mdcompose('c',m0,m2);
	m2=bp2qntm(booln,k,cur,cur1,i,x+1,y-1); m1=mdcompose('c',m1,m2);

	return mdcompose('d',m0,m1);
}
qmdata *bp2qwfpc(qinfo *qi,tree *bp,tree *fness,tree **fn,int booln,int k,int cur,int cur1,int lv,int start)
{
	int n,x;
        qmdata *m0,*m1,*m2;
	int curj;
	int curii,curll;
	tree *t0;

	for (n=0;fn[n]!=NULL;n++);
	m0=bp2qsfpc(qi,bp,fness,fn,booln,k,cur,cur1,lv,start);

	x=0;
	m2=bp2qntm(booln,k,cur,cur1,x,x+1,n+1); 
	for (x=1;x<=k;x++) {
		m1=bp2qntm(booln,k,cur,cur1,x,x+1,n+1); 
		m2=mdcompose('c',m1,m2);
	}

	return mdcompose('d',m0,m2); 
}
/************************************************************************/

/************************************************************************/
#define bpi bp->right->left
#define bpt bp->right->right->left
/************************************************************************/
qmdata *bp2qPATH(qinfo *qi,tree *bp,int booln,int k,int cur,int lev)
{
	int i;
	qmdata *md0,*md,*md1;
	int cur1=qi->n;

	if (k==0&&lev!=1) return NULL;

	md=(qmdata *)calloc(1,sizeof(qmdata));
	md->Clause=(int *)malloc((2)*sizeof(int));
	md->Clause[0]='c'; md->Clause[1]=0;

	md0=md;
	if (lev==1) {
		md->q=tree2md(bpi); mdstep1(md->q,cur,cur,booln); 
		md->next=(qmdata *)calloc(1,sizeof(qmdata)); md1=md;
		md=md->next;
		qi->n+=booln; 
	} 

	cur1=qi->n-booln;
	for (i=0;i<k;i++) {
		md->q=tree2md(bpt); 
		if (i==0) { mdstep1(md->q,cur,cur1,booln); }
		else { mdstep1(md->q,cur1,cur1,booln); }
		md->next=(qmdata *)calloc(1,sizeof(qmdata)); md1=md;
		md=md->next;
		cur1+=booln; 
		qi->n+=booln;
	}
	free(md); md1->next=NULL; 
	return md0;
}
/************************************************************************/
qmdata *bp2qeqPATH(int a,int b,int c,int booln)
{
        int i,j;
        tree *t0;
	int pa=1;
	qmdata *md;

	a=a/booln; b=b/booln; c=c/booln;

        if (a==b) {
		t0=treeconstab(0,a*booln+1);
		md=tree2md(t0); treefree(t0); return md;
	}
        t0=eqstate(pa,a,pa,b,-1);

        for (i=b-1;i<c;i++)
        for (j=i+1;j<=c;j++) {
		if (i==b-1) {
                	if (j==b) continue;
                	t0=treecompose("|",t0,eqstate(pa,a,pa,j,-1));
		} else { 
                	t0=treecompose("|",t0,eqstate(pa,i,pa,j,-1));
		}
	}
	eqstateconstruct(t0,booln);

	md=tree2md(t0); treefree(t0);
	return md;
}

/************************************************************************/
qmdata *tree2md(tree *t)
{
	int x;
	qmdata *md;
	if (t==NULL) return NULL;

	md=(qmdata *)calloc(1,sizeof(qmdata));
	switch (t->node[0]) {
		case '|': 
		case '&': 
			  md->Clause=(int *)malloc((2)*sizeof(int));
			  if (t->node[0]=='|') x='d'; else x='c';
			  md->Clause[0]=x; md->Clause[1]=0;
			  md->q=tree2md(t->left);
			  md->next=(qmdata *)calloc(1,sizeof(qmdata));
			  md->next->q=tree2md(t->right);
			  return md;
		case '^': 
		case '_': 
			  md->Clause=(int *)malloc((4)*sizeof(int));
			  if (t->node[0]=='^') x='d'; else x='c';
			  md->Clause[0]=x; 
			  md->Clause[1]=1; md->Clause[2]=-1; md->Clause[3]=0;
			  return md;
		default: 
			  md->Clause=(int *)malloc((3)*sizeof(int));
			  if (t->node[0]=='!') x=-atoi(t->right->node);
			  else x=atoi(t->node); 
			  md->Clause[0]='d'; md->Clause[1]=x; md->Clause[2]=0;
			  return md;
	}
	
}
/************************************************************************/
qmdata *mdnegate(qmdata *m0)
{
	int *p0;
	qmdata *m1=m0;
	if (m0==NULL) return NULL;
	switch (m0->Clause[0]) {
		case 'a': m0->Clause[0]='e'; break;
		case 'e': m0->Clause[0]='a'; break;
		case 'c': m0->Clause[0]='d'; 
			  for (p0=m0->Clause+1;*p0;p0++) *p0=-*p0; break;
		case 'd': m0->Clause[0]='c'; 
			  for (p0=m0->Clause+1;*p0;p0++) *p0=-*p0; break;
	}
	mdnegate(m0->q);
	while (m0->next!=NULL) { mdnegate(m0->next->q); m0=m0->next; }
	return m1;
}
/************************************************************************/
void mdstep1(qmdata *m0,int i,int j,int booln)
{
	int *p0;
	if (m0==NULL) return;
	for (p0=m0->Clause+1;*p0;p0++) {
		if (*p0>0) { if (*p0<=booln) *p0+=i; else *p0+=j; }
		else if (*p0>=-booln) *p0-=i; else *p0-=j;
	}
	mdstep1(m0->q,i,j,booln);
	while (m0->next!=NULL) { mdstep1(m0->next->q,i,j,booln); m0=m0->next; }
}
/************************************************************************/
qmdata *mdcompose(int a,qmdata *m0,qmdata *m1)
{
	qmdata *md;

	if (m0==NULL) return m1; if (m1==NULL) return m0;
	if (a=='I') return mdcompose('d',mdnegate(m0),m1);
	
	md=(qmdata *)calloc(1,sizeof(qmdata));
	md->Clause=(int *)malloc((2)*sizeof(int));
	md->Clause[0]=a; md->Clause[1]=0; md->q=m0;
	md->next=(qmdata *)calloc(1,sizeof(qmdata)); md->next->q=m1;
	return md;
}
/************************************************************************/
tree *md2tree(qmdata *md)
{
	tree *t1,*t2;
	char c[2]="0";

	switch (md->Clause[0]) {
		case 'd': 
		case 'c': 
			if (md->Clause[0]=='c') c[0]='&'; else c[0]='|';
			t1=maketree1(c,md->Clause+1);
			t2=maketree2(c,md->q,md->next);
			return treecompose(c,t1,t2);
		case 'a': 
		case 'e': 
			return md2tree(md->q);
	}
}
/************************************************************************/
tree *maketree1(char *c,int *s)
{
	tree *t;
	if (*s==0) { if (c[0]=='&') return treeconst(1); return treeconst(0); }
	t=maketree1(c,s+1); if (s[0]>0) return treecompose(c,treenum(s[0]),t);
	return treecompose(c,treecompose("!",NULL,treenum(-s[0])),t);
}
tree *maketree2(char *c,qmdata *q1,qmdata *qn)
{
     if (q1==NULL) { if (c[0]=='&') return treeconst(1); return treeconst(0);}
     if (qn==NULL) { return md2tree(q1); }
     return treecompose(c,md2tree(q1),maketree2(c,qn->q,qn->next));
}
/************************************************************************/
int md2treetype(qmdata *md)
{
	int r=0;
	if (md==NULL) return r;
	switch (md->Clause[0]) { case 'a': r=1; break; case 'e': r=2; break; }
	r=r|md2treetype(md->q);
	while (md->next!=NULL) { r=r|md2treetype(md->next->q); md=md->next; }
	return r;
}
/************************************************************************/
/************************************************************************/
int checkmdvalue(qmdata *m0,int i,int j,int lev)
{
	int k=0;
	int *p0;

	if (lev==0) infos("checkmdvalue"); 
	if (m0==NULL) return 0;
	for (p0=m0->Clause+1;*p0;p0++) 
	if ((abs(*p0)<i||abs(*p0)>j)) { infoi(*p0); k++; error("CHECK"); }
	k+=checkmdvalue(m0->q,i,j,1);
	while (m0->next!=NULL) {k+=checkmdvalue(m0->next->q,i,j,1);m0=m0->next;}
	return k;
}
/************************************************************************/
