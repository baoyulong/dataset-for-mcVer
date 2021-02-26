#include "bp2t.h"
#include "tbdmm.h"
#include "tbdop.h"
#include "tree.h"


/*******************************************************************/
TBDprg *prg2t(prg *prg1,char **v)
{
	TBDprg *jj;

	tree *i0,*init;
	process **prs;
	tree *fness;
	int booln=prg1->booln;

/*p.w.prg1->varconstr in procedures (involves too many variables):
	init=treecompose(";",prg1->varconstr,prg1->prog->right->left);
*/
	init=prg1->prog->right->left;
	for (i0=init;i0->node[0]==';';i0=i0->right) i0->node[0]='&';

	prs=prg1->prs;
	fness=prg1->prog->right->right->right->left;
	
	jj=bp2t(init,fness,prs,v,booln);
	jj->vlist=prg1->vlist;
	if (jj->puretrans==-1) jj->puretrans=prg1->puretrans;
	return jj;
}
/************************************************************************/
TBDprg *bp2t(tree *init,tree *fness,process **prs,char **v,int booln)
{
	TBDprg *jj;

	TBDmminitialize(booln);
	jj=TBDprginitialize(v,booln);
	jj->puretrans=-1;
	jj->i=TPainit(init,jj);
	jj->fairlist=TPafair(fness,jj);
	jj->trs=TPatrs(prs,jj);

	return jj;
}
/************************************************************************/
TBDprg *TBDprginitialize(char **v,int n)
{
	int i,k,m=2*n;
	char **v0,**v1;
	TBDprg *jj;

	jj=(TBDprg *)calloc(1,sizeof(TBDprg));


	if (v!=NULL) {
        	v0=(char **)calloc(n,sizeof(char *));
        	v1=(char **)calloc(n,sizeof(char *));
		for (k=i=0;i<n;i++) { v0[i]=v[k]; k++; v1[i]=v[k]; k++; }
	} else {
        	v0=(char **)calloc(n,sizeof(char *));
        	v1=(char **)calloc(n,sizeof(char *));
        	for (i=0;i<n;i++) v0[i]=(char *)malloc(MAXVARLEN*sizeof(char));
        	for (i=0;i<n;i++) v1[i]=(char *)malloc(MAXVARLEN*sizeof(char));
        	for (i=1;i<=n;i++) {
                	sprintf(v0[i-1],"%i",i);
                	sprintf(v1[i-1],"%i",n+i);
        	}
	}
        jj->vval=(int *)malloc((m+1)*sizeof(int));
        jj->vname=(char **)malloc((m+1)*sizeof(char*));
	jj->v0=v0; jj->v1=v1; 
	jj->booln=n; 

	TPvar(jj,0); return jj;
}
/**********************************************************************/

/************************************************************************/
int TBDtreelength(tree *trs,char *del)
{
        while (*del&&*del!=trs->node[0]) del++;
	if (*del==0) return 1;
        return TBDtreelength(trs->left,del)+TBDtreelength(trs->right,del);
}
/************************************************************************/
int TBDlistlength(tree *t1)
{
        int i=1;
        while (t1->node[0]==';') { t1=t1->right; i++; }
        return i;
}
/*******************************************************************/
tree *TBDlistgetone(tree *t,int next)
{
        tree *t1;
        static tree *t0;

        if (next==0) t0=t;
        if (t0==NULL) return NULL;
        if (t0->node[0]==';')
        { t1=t0->left; t0=t0->right; } else { t1=t0; t0=NULL; } return t1;
}
/*******************************************************************/

/************************************************************************/
TBDtrs **TPatrs(process **prs,TBDprg *lggt)
{
	int i,k;
	TBDtrs **trs;

	if (prs==NULL) return NULL;
	for (k=0,i=0;prs[i]!=NULL;i++) k+=prs[i]->n;

	lggt->trscnt=k;
	trs=(TBDtrs **)malloc((k+1)*sizeof(TBDtrs *)); trs[k]=NULL;
	for (i=0;i<k;i++) trs[i]=calloc(1,sizeof(TBDtrs));

	TPtrs(trs,prs,lggt,1); TPtrs(trs,prs,lggt,0); return trs;
}
/**********************************************************************/
TBD **TPafair(tree *fn,TBDprg *lggt)
{
	int lc=0;
	int k;
	tree *l0;
	TBD **fair;

	if (fn==NULL) return NULL; k=TBDlistlength(fn);
	fair=(TBD **)malloc((k+1)*sizeof(TBD *)); fair[k]=NULL;
        while ((l0=TBDlistgetone(fn,lc++))!=NULL) {
		fair[lc-1]=TPfml(lggt,l0); TBDkp(fair[lc-1]);
	}
	return fair;
}
/**********************************************************************/
TBD *TPainit(tree *init,TBDprg *lggt)
{
	TBD *i;
	if (init==NULL) return NULL;
	i=TPfml(lggt,init); TBDkp(i);
	return i;
}
/**********************************************************************/
TBD *TPfml(TBDprg *lggt,tree *p)
{
        TBD *r;
/*
 static i=0;
 i++; infoi(i);
*/
        switch (p->node[0]) {
            case ';':
            case '|': r= TBDds(TPfml(lggt,p->left),TPfml(lggt,p->right)); break;
            case ':':
            case '&': r= TBDcn(TPfml(lggt,p->left),TPfml(lggt,p->right)); break;
            case '!': r= TBDng(TPfml(lggt,p->right)); break;
            default:  r= TPtbdvar(lggt,p->node); break;
        }
/*
 i--; infoi(i);
*/
        return r;
}
/************************************************************************/



/************************************************************************/
void TPtrs(TBDtrs **trs,process **prs,TBDprg *lggt,int bw)
{
	int i,j,k;
	int bn=lggt->booln;

	switch (bw) {
	    case 0: 
		for (k=0,i=0;prs[i]!=NULL;i++) for (j=0;j<prs[i]->n;j++) {
			trs[k]->p=TPfml(lggt,prs[i]->tslist[j]->left);
			if (trs[k]->p!=TBDomg) {
			    trs[k]->t=TPfml(lggt,prs[i]->tslist[j]->right);
			    trs[k]->pt=TBDcn(trs[k]->p,trs[k]->t);
			    trs[k]->uclist=(char *)calloc(2*bn+1,sizeof(char));
			    trs[k]->tc=TPtrstc(trs[k]->btt,trs[k]->uclist,bn);
			    trs[k]->pc=prs[i]->tslistpc[j];
			    TBDkp(trs[k]->p); TBDkp(trs[k]->t); 
			    TBDkp(trs[k]->pt); TBDkp(trs[k]->tc);
			}
			k++;
		}
		return;
	    case 1: 
		TPvar(lggt,1);
		for (k=0,i=0;prs[i]!=NULL;i++) for (j=0;j<prs[i]->n;j++) {
			trs[k]->bp=TPfml(lggt,prs[i]->tslist[j]->left);
			if (trs[k]->bp!=TBDomg) {
			    trs[k]->bt=TPfml(lggt,prs[i]->tslist[j]->right);
			    trs[k]->bpt=TBDcn(trs[k]->bp,trs[k]->bt);
			    trs[k]->btt=TBDgeq(trs[k]->bt,bn);
			    TBDkp(trs[k]->bp); TBDkp(trs[k]->bt);
			    TBDkp(trs[k]->bpt);
			    if (trs[k]->btt==NULL) lggt->puretrans=0;
			}
			k++;
		}

		TPvar(lggt,0);
		return;
	}
}
/************************************************************************/
#define equiv(a,b) TBDcn(TBDds(TBDap(-(a)),b),TBDds(TBDap(a),TBDng(b)))
/************************************************************************/
TBD *TPtrstc(TBD **btt,char *uclist,int n)
{
	TBD *t0;
        int i=0,j;
	TBD *tc=TBDtau;

	if (btt==NULL) return NULL;

	for (j=1;j<2*n;j+=2) {
		if ((btt[j]==TBDap(j+1))&&(btt[-j]==TBDap(-(j+1)))) 
		uclist[j+1]=uclist[j]=1;
	}

	for (j=1;j<=2*n;j+=2) if (uclist[j]==0) {
		t0=TBDasrn(btt[j],uclist); 
		tc=TBDcn(tc,equiv(j,t0));
	}
	return tc;
}
/************************************************************************/

/************************************************************************/
void TPvar(TBDprg *jj,int bw)
{
	int i,k;
	int n=jj->booln;
	int m=2*n;
	char **tmp0,**tmp1;

	if (bw==0) {tmp0=jj->v0;tmp1=jj->v1;} else {tmp0=jj->v1;tmp1=jj->v0;}

        for (k=0,i=0;i<n;i++) { 
		jj->vname[k]=tmp0[i]; jj->vval[k]=k+1; k++;
		jj->vname[k]=tmp1[i]; jj->vval[k]=k+1; k++;
	}
	TPsv1(jj->vname,jj->vval,0,m-1);
}
/************************************************************************/
void TPsv1(char **a,int *b,int l,int r) /* increasing */
{
        int i,j,k;
        char *v,*t;
        if (r>l) {

                v=a[r]; i=l-1;j=r;
                for (;;) {
                        for (i++;strcmp(a[i],v)<0;i++);
                        for (j--;j>=l&&strcmp(a[j],v)>0;j--); /*if (j<l) j=1;*/
                        if (i>=j) break;
                        t=a[i]; a[i]=a[j]; a[j]=t; k=b[i]; b[i]=b[j]; b[j]=k;
                }
                t=a[i]; a[i]=a[r]; a[r]=t; k=b[i]; b[i]=b[r]; b[r]=k;
                TPsv1(a,b,l,i-1); TPsv1(a,b,i+1,r);
        }
}
/**********************************************************************/
int TPsnm(char *s,char **a,int *b,int l,int r)
{
        int i;
        if (l>r) return 0; i=(l+r)/2;
        if (strcmp(s,a[i])<0) return TPsnm(s,a,b,l,i-1);
        if (strcmp(s,a[i])>0) return TPsnm(s,a,b,i+1,r);
        return b[i];
}
/**********************************************************************/
TBD *TPtbdvar(TBDprg *jj,char *var)
{
        int i;

        if (*var=='_') return TBDcn(TBDap(1),TBDap(-1));
        if (*var=='^') return TBDng(TBDcn(TBDap(1),TBDap(-1)));
        i=TPsnm(var,jj->vname,jj->vval,0,2*(jj->booln)-1);
        if (i==0) { infos(var); error("variable not in the given list"); }
        return TBDap(i);
}
/************************************************************************/
