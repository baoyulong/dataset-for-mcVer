#include "coi.h"
#include "coi.h"
#include "f2p.h"

/************************************************************************/
#define worder(j,i)		(w[j]>w[i])
#define COTSvarchangein(s,node) (node[0]!='V'||strcmp(node+1,s)!=0)
#define ispred(s) \
        (s[0]=='='||s[0]=='>'||s[0]=='<'|| s[0]=='B'||s[0]=='}'||s[0]=='{') \
/************************************************************************/
int DEPanalysis(tree *prog,int pp)
{
        int i,j,k;
        int lc=0;
        tree *t;
        int *DEPorder;
        int *DEPweight;
        int **DEPtable;
        int **DEPctltable;
        int *varlist;
        tree *vlist=prog->left;
        tree *trs=prog->right->right->left; 
	tree *ff=treecopy(prog->right->right->right->left);
	tree *spec=treecopy(listget1n(prog->right->right->right->right,pp));
        int nvar=listlength(vlist);

	if (ff!=NULL) spec=treecompose(";",ff,spec);

        DEPtable=(int **)calloc(nvar,sizeof(int *));
        DEPctltable=(int **)calloc(nvar,sizeof(int *));
        for (i=0;i<nvar;i++) {
                DEPtable[i]=(int *)calloc(nvar,sizeof(int));
                DEPctltable[i]=(int *)calloc(nvar,sizeof(int));
        }

        while ((t=listgetone2(trs,lc++))!=NULL) {
                if (t->node[0]!=':') error("DEPanalysis");
                DEPadd(t->right,t->left,vlist,DEPtable,DEPctltable);
        }
/*
 inttablewrite(DEPtable,nvar,nvar,stdout);
 inttablewrite(DEPctltable,nvar,nvar,stdout);
*/

	if (pp) {
        	varlist=(int *)calloc(nvar,sizeof(int));
		i=DEPvarlist(prog,spec,DEPtable,DEPctltable,nvar,varlist);
		if (i<nvar) DEPsimp(prog,varlist);
/*
 treewrite(prog,stdout);
		infos("VARLIST"); infoi(i); infoi(nvar);
		infos("VARLIST"); for (i=0;i<=nvar;i++) { infoi(varlist[i]); }
*/
		free(varlist);
	} else {
        	DEPweight=(int *)calloc(nvar,sizeof(int));
        	DEPorder=(int *)calloc(nvar,sizeof(int));

        	for (i=0;i<nvar;i++) { DEPorder[i]=i; }
        	DEPweight=DEPcalcweight(DEPweight,nvar,DEPtable,DEPctltable);
/*
		infos("ITEM"); 
		for (i=0;i<nvar;i++)
		if (DEPweight[i]==0) { treewrite(listitem(i,vlist),stdout); infos(" ");}
*/

        	DEPsort(DEPorder,DEPweight,nvar);
        	DEPrefine(DEPorder,DEPweight,nvar,DEPtable,DEPctltable);

        	t=DEPreorder(DEPorder,nvar,vlist); treereplace(vlist,t);
        	free(DEPweight); free(DEPorder); 
	}

        for (i=0;i<nvar;i++) { free(DEPtable[i]); free(DEPctltable[i]); }
	free(DEPtable); free(DEPctltable);
}
/************************************************************************/
void DEPadd(tree *t0,tree *tl,tree *vlist,int **DEPtable,int **DEPctltable)
{
        int k;

        if (t0==NULL) return;
        if (t0->node[0]=='=') {
            DEPadddep(t0->left,t0->right,tl,vlist,DEPtable,DEPctltable);
            return;
        }
        DEPadd(t0->left,tl,vlist,DEPtable,DEPctltable);
        DEPadd(t0->right,tl,vlist,DEPtable,DEPctltable);
}
/************************************************************************/
void DEPadddep(tree *t0l,tree *t0r,tree *tl,tree *vlist,int **DEPtable,int **DEPctltable)
{
        int k;

        if (t0l==NULL) return;
        if (t0l->node[0]=='O') {
                if (COTSvarchangein(t0l->node+1,t0r->node)) {
                        k=treesearchvarlistnum(t0l->node+1,0,vlist);
                        if (k==-1) error("DEPadd");
                        DEPaddnum(DEPctltable[k],tl,vlist);
                        DEPaddnum(DEPtable[k],t0r,vlist);
                }
            	return;
        }
        DEPadddep(t0l->left,t0r,tl,vlist,DEPtable,DEPctltable);
        DEPadddep(t0l->right,t0r,tl,vlist,DEPtable,DEPctltable);
}
/************************************************************************/
void DEPaddnum(int *ilist,tree *t1,tree *vlist)
{
        int k;

        if (t1==NULL) return;
        if (t1->node[0]=='V') {
		k=treesearchvarlistnum(t1->node+1,0,vlist);
		if (k==-1) error("DEPaddnum");
		ilist[k]++; return;
        }
        DEPaddnum(ilist,t1->left,vlist);
        DEPaddnum(ilist,t1->right,vlist);
}
/************************************************************************/
void DEPsort(int *ord,int *w,int nvar)
{
	int i,j,x;
	for (i=0;i<nvar;i++) {
		for (j=i+1;j<nvar;j++) {
			if (worder(j,i)) {
				x=w[j]; w[j]=w[i]; w[i]=x;
				x=ord[j]; ord[j]=ord[i]; ord[i]=x;
			}
		}
	}
}
/************************************************************************/
tree *DEPreorder(int *ord,int nvar,tree *vlist)
{
	int i,lc,pp;
	tree *t,*nl=NULL;

	for (i=0;i<nvar;i++) {
		lc=0; pp=ord[i]+1; 
		while (lc<pp) t=listgetone(vlist,lc++);
		nl=listmerge(nl,treecopy(t));
	}
	return nl;
}
/************************************************************************/
#define val(i,j)	(DEPtable[i][j]+DEPctltable[i][j])
#define val2(i,j)	(val(i,j)+val(j,i))
#define dep(i,j)	(DEPtable[i][j]+DEPctltable[i][j]>0)
#define dep2(i,j)	(dep(i,j)||dep(j,i))
/*
/************************************************************************/
int *DEPcalcweight(int *w,int nvar,int **DEPtable,int **DEPctltable)
{
	int i,j;
	int *w1=(int *)calloc(nvar,sizeof(int));

	for (i=0;i<nvar;i++) {
		for (j=0;j<nvar;j++) 
		if (dep2(i,j)) { w[i]+=val2(i,j); w[j]+=val2(i,j); }
	}
	for (i=0;i<nvar;i++) w1[i]=w[i];
	for (i=0;i<nvar;i++) {
		for (j=0;j<nvar;j++) 
		if (dep2(i,j)) { w[i]+=w1[j]; w[j]+=w1[i]; }
	}

	free(w1); return w;
}
/************************************************************************/
void DEPrefine(int *ord,int *w,int nvar,int **DEPtable,int **DEPctltable)
{
	int i,j,x;
	int *w1=(int *)calloc(nvar,sizeof(int));

	for (i=1;i<nvar;i++) {
		w1[i]=dweight(i-1,i,ord,DEPtable,DEPctltable);
		for (j=i+1;j<nvar;j++) {
			w1[j]=dweight(i-1,j,ord,DEPtable,DEPctltable);
			if (w1[j]>w1[i]||(w1[j]=w1[i]&&worder(j,i))) {
				x=w[j]; w[j]=w[i]; w[i]=x; w1[i]=w1[j];
				x=ord[j]; ord[j]=ord[i]; ord[i]=x;
			}
		}
	}
	free(w1);
}
/************************************************************************/
int dweight(int i,int j,int *ord,int **DEPtable,int **DEPctltable)
{
	int k,x,y,z;

	y=ord[j];
	for (z=k=0;k<=i;k++) { 
		x=ord[k]; 
		if (dep2(x,y)) z+=(0x100000/(k+1)/(k+1))/* *val2(x,y)*/ ;
	}
	return z;
}
/************************************************************************/
tree *listitem(int i,tree *vlist)
{
	tree *t;
	int lc=0,pp=i+1;

	while (lc<pp) t=listgetone(vlist,lc++); return t;
}
/************************************************************************/

/************************************************************************/
#define trs 	prog->right->right->left;
/************************************************************************/
int DEPvarlist(tree *prog,tree *spec,int **tb,int **ctltb,int nvar,int *vlist) 
{
	int n,k,r=1;

	if (spec==NULL) return nvar+1;
	vlist[0]=-1;

/*
	if (!xconservative(spec)) spec=addtrsleft(trs,spec);
*/
	k=n=DEPvarcore(spec,vlist,prog->left); treefree(spec);
	while (n) { 
		n=DEPvar(vlist,tb,ctltb,nvar); k+=n; 
/*
 infoi(k);
 infoi(r);
 infoi(PAR_tmp);
		if (PAR_tmp&&r==PAR_tmp) break; 
*/
		r++;
	}
	return k;
}
/************************************************************************/
int DEPvar(int *a,int **tb,int **ctltb,int nvar)
{
	int *ll=a;
	int i,n=0;
	int k0=0,k=0;

        while (*ll!=-1) { k0++; ll++; }

	ll=a;
        while (*ll!=-1&&k<k0) {
		for (i=0;i<nvar;i++) 
		if (tb[*ll][i]||ctltb[*ll][i]) n+=intadd2list(i,a);
		ll++; k++;
	}
	return n;
}
/************************************************************************/
int DEPvarcore(tree *t0,int *a,tree *vlist)
{
	int k,n=0;
	if (t0==NULL) return 0;
	if (t0->node[0]=='V') { 
		k=treesearchvarlistnum(t0->node+1,0,vlist);
		n+=intadd2list(k,a); return n; 
	}
	return DEPvarcore(t0->left,a,vlist)+DEPvarcore(t0->right,a,vlist);
}
/************************************************************************/
int intadd2list(int k,int *ll)
{
	while (*ll!=-1) { if (*ll==k) return 0; ll++; };
	*ll++=k; *ll=-1; return 1;
}
/************************************************************************/

/************************************************************************/
LOCAL int *DEPvlist;
/************************************************************************/
#define ispred(s) \
        (s[0]=='='||s[0]=='>'||s[0]=='<'||s[0]=='B'||s[0]=='}'||s[0]=='{') \
/************************************************************************/
int DEPsimp(tree *t0,int *varlist)
{
	int i;
	DEPvlist=varlist;

/*
 for (i=0;varlist[i]!=-1;i++) {
	infoi(varlist[i]);
	treewrite(listitem(varlist[i],t0->left),stdout);
 }
*/

	t0->left=DEPsimplify1(t0->left,0);
	t0->right->left=DEPsimplify2(t0->right->left,t0->left);
	t0->right->right->left=DEPsimplify3(t0->right->right->left,t0->left);
	t0->right->right->right->right=DEPsimplify2(t0->right->right->right->right,t0->left);
 
	t0->right=treedopost(t0->right,TLFnnfsimp);
}
tree *DEPsimplify1(tree *l0,int i)
{
	tree *l1;
	if (l0->node[0]==';') {
		l1=DEPsimplify1(l0->right,i+1);
		if (DEPinlist(i)) {
			if (l1==NULL) { l1=l0->left; free(l0); return l1; }
			l0->right=l1; return l0;
		}
		treefree(l0->left); free(l0); return l1;
	}
	if (DEPinlist(i)) return l0; treefree(l0); 
	return NULL;
}
tree *DEPsimplify2(tree *t0,tree *vl)
{
	if (t0==NULL) return NULL;
	if (ispred(t0->node)) {
		if (DEPsomeninlist(t0,vl)) { treefree(t0); return treeconst(1);}
		return t0;
	}
	t0->left=DEPsimplify2(t0->left,vl); 
	t0->right=DEPsimplify2(t0->right,vl);
	return t0;
}
tree *DEPsimplify3(tree *l0,tree *vl)
{
	if (l0->node[0]==';') {
		l0->left->left=DEPsimptrleft(l0->left,vl);
		l0->left->right=DEPsimplify2(l0->left->right,vl);
		l0->right=DEPsimplify3(l0->right,vl); 
/*
 prts("DEPsimplify3");
 treewrite(l0,stdout);
*/
		return l0;
	}
	l0->left=DEPsimptrleft(l0,vl);
	l0->right=DEPsimplify2(l0->right,vl); return l0;
}
/************************************************************************/
int DEPinlist(int i) 
{
	int *ll=DEPvlist;
	while (*ll!=-1) { if (*ll==i) return 1; ll++; }; return 0;
}
int DEPsomeninlist(tree *t0,tree *vl) 
{
	if (t0==NULL) return 0;
	if (t0->node[0]=='V'||t0->node[0]=='O') 
	return treesearchvarlist(t0->node+1,0,vl)==NULL;
	return DEPsomeninlist(t0->left,vl)||DEPsomeninlist(t0->right,vl);
}
/************************************************************************/
tree *DEPsimptrleft(tree *t0,tree *vl) 
{
	tree *t=NULL;
	if (t0->left->node[0]!='^'&&t0->left->node[0]!='_') {
		t=treecopy(t0->left);
		t0->left=DEPsimplify2(t0->left,vl);
/*
		if (treediff(t,t0->left)) { 
			treefree(t0->left); t0->left=treeconst(0); 
		}
*/
	}
	treefree(t); return t0->left;
/*new PROBLEM stuttering*/
}
/************************************************************************/


/********************************************************************/
void var_order(prg *pr,char *varfile)
{
	FILE *g=fopen(varfile,"r");
	char line[1000],vn[1000];
	tree *v0,*v1,*v2;
	int low,high;
	int i,j;
	char *l1,*l2;
	tree *vlist_old=pr->vlist;
	tree *vlist=pr->prog->left;
	tree *newlist=NULL,*n1=NULL;

	i=0;
	while (1) {
		fgets(line,1000,g); if (feof(g)) { fclose(g); break; }
		var_order_vn(line,vn,vlist_old);
		
		v0=treesearchvarlist(vn,0,vlist); 
		if (v0==NULL) { infos("in varfile:"); infos(vn); continue; }

		newlist=listmerge(newlist,treecopy(v0));
	}

	for (v1=vlist;;v1=v1->right) {
		if (v1->node[0]!=';') v2=v1; else v2=v1->left;
		v0=treesearchvarlist(v2->left->node,0,newlist); 
		if (v0==NULL) n1=listmerge(n1,treecopy(v2));
		if (v1->node[0]!=';') break;
	}

	newlist=listmerge(newlist,n1); treefree(vlist); 
	pr->prog->left=newlist;
}
void var_order_vn(char *l1,char *l2,tree *vlist_old)
{
	int i=0;
	char *l0=l1;
	char *l2a=l2;
	char *l1a,*l0a;
	int a0=0;
	int a1=1;
	tree *v;

	while (*l1&&*l1!='\n'&&*l1!='[') { *l2++=*l1++; i++; }
	if (*l1==0||*l1=='\n') { *l2=0; return; }
	v=treesearchvarlist(l0,i+1,vlist_old); 
	if (v==NULL&&var_order_process(l0,l2a)) {
		return var_order_vn(l0,l2a,vlist_old); 
	}
	if (v==NULL) { infos(l0); error("varfile 1"); } 
	l0=v->left->node;
	l1a=l1; l0a=l0; while (*l1) { l1++; } while (*l0) { l0++; }
	while (l1>l1a&&*l1!='[') { l1--; }

	while (1) {
		a0+=atoi(l1+1)*a1; l1--; while (l1>l1a&&*l1!='[') { l1--; }
		if (*l1!='[') { sprintf(l2,"_x_%i",a0); return; }
		while (l0>l0a&&*l0!='.') { l0--; } a1*=atoi(l0+1)+1; l0-=2;
	}
}
/********************************************************************/
int var_order_process(char *l1,char *l2)
{
	char *l1a=l1;
	char *l2a=l2;

	for (l1a=l1;*l1a&&*l1a!='[';l1a++); if (*l1a==0) return 0;
	for (l1a=l1;*l1a!='[';) *l2a++=*l1a++; strcpy(l2a,"_x_"); 
	for (l2a+=3,l1a++;*l1a!=']';) *l2a++=*l1a++; 
	for (l1a++;*l1a!=0;) *l2a++=*l1a++; *l2a=0; strcpy(l1,l2); return 1;
}
/********************************************************************/
