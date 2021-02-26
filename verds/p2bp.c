#include "p2bp.h"

/*******************************************************************/
#define istrue(x)	(x->node[0]=='^')
#define isfalse(x)	(x->node[0]=='_')
/*******************************************************************/
tree *ARITmod(tree *s1,tree *s2)
{
	tree *a1,*a2;
	s1=treedopost(s1,TLFnnfsimp); 
	s2=treedopost(s2,TLFnnfsimp); 
	a1=ARITdiv(s1,s2);
	a2=ARITmul(a1,s2); treefree(a1);
	a1=ARITapply(treecompose("-",treecopy(s1),a2));
	a1=treedopost(a1,TLFnnfsimp); 
	return a1;
}
/*******************************************************************/
tree *ARITdiv(tree *x1,tree *x2)
{
	int i=0;
        int a1=listlength(x1);
	tree **tpos;
        tree *s0,*s1,*s2,*s3,*s4,*s5,*r0;

	if ((r0=ARITdiv2(x1,x2))!=NULL) return r0;

	tpos=(tree **)malloc(a1*sizeof(tree *));

	s0=treecompose(FALSEs,NULL,NULL);
	for (i=0;i<a1;i++) {
		s1=ARITdoublekt(treecopy(x2),a1-i-1);
		s2=listcombine(ARITadd,s0,s1); s2->node[0]=';';
		s2=treedopost(s2,TLFnnfsimp); 
		s3=treecompose("}",treecopy(x1),s2);
		tpos[i]=ARITapply(s3); tpos[i]=treedopost(tpos[i],TLFnnfsimp); 
		s4=ARITmul(s1,tpos[i]); s4=treedopost(s4,TLFnnfsimp); 
		s5=listcombine(ARITadd,s0,s4); s5->node[0]=';';
		treefree(s0); treefree(s1); treefree(s4); 
		s0=s5; s0=treedopost(s0,TLFnnfsimp); 
		s0=listreduce(s0);
	}
	treefree(s0); 
	r0=tpos[a1-1];

	for (i=a1-2;i>=0;i--) r0=treecompose(";",tpos[i],r0); free(tpos);

	return listreduce(r0);
}
/*******************************************************************/
tree *ARITdoublekt(tree *s2,int k)
{
	int i;
        tree *a0=s2;
	tree *a1=treeconst(0);

	if (k==0) return s2;
        while (a0->node[0]==';') a0=a0->right;
	for (i=1;i<k;i++) a1=treecompose(";",treeconst(0),a1);
        treereplace(a0,treecompose(";",treecopy(a0),a1)); return s2;
}
/*******************************************************************/
tree *listreduce(tree *s1)
{
	tree *s2;
	if (s1==NULL) return s1;
	if (s1->node[0]!=';') return s1;
	if (s1->left->node[0]!='_') return s1;
	s2=s1->right; free(s1->left); free(s1); return listreduce(s2);
}
/*******************************************************************/
tree *ARITdiv2(tree *x1,tree *x2)
{
	int i=0;
        int a1=listlength(x1);
        int a2=listlength(x2);
	tree *r=NULL;


	if (!((a2==2)&(istrue(x2->left)&&isfalse(x2->right)))) return NULL;

	if (a1==1) return treecompose("_",NULL,NULL);
	for (;x1->node[0]==';';x1=x1->right) r=listmerge(r,treecopy(x1->left));
	return r;
}
/*******************************************************************/

/*******************************************************************/
#define simploc(s) s=treedopost(s,TLFnnf); s=treedopost(s,TLFnnfsimp); 
/*******************************************************************/
extern time_t t0,t1;
/*******************************************************************/
tree *ARITsub0(tree *x1,tree *x2,tree **s0)
{
        int a1=listlength(x1);
        int a2=listlength(x2);
        tree *r0,*r1,*t1,*t2;

        t1=treecopy(x1); t2=treecopy(x2);
	if (a1<a2) {
		r1=t2->left; 
		r0=t2; t2=t2->right; free(r0); a2--;
        	for (;a2>a1;a2--) { 
			r1=treecompose("&",r1,t2->left); 
			r0=t2; t2=t2->right; free(r0);
		}
		*s0=r1;
	}
	return treecompose("-",t1,t2);
}
/*******************************************************************/
tree *ARITadd(tree *s1,tree *s2,tree *s0)
{
	tree *a1,*b1,*c1,*a0,*c0,*r0;

	a1=s1; b1=s2;
	c0=land(treecopy(a1),treecopy(b1));
	a0=xor(treecopy(a1),treecopy(b1));
	if (s0==NULL) return treecompose("+",c0,a0);
	c1=s0->left;
	c0=lor(c0,land(lor(treecopy(a1),treecopy(b1)),treecopy(c1)));
	a0=xor(a0,treecopy(c1)); 
	r0=treecompose("+",c0,treecompose(";",a0,s0->right));
	r0->left=TLFred(r0->left);  /*new 120918*/
	treefree(s0->left); free(s0); return r0;
}
/*******************************************************************/
tree *ARITsub(tree *s1,tree *s2,tree *s0)
{
	tree *a1,*b1,*c1,*a0,*c0,*r0;

	a1=s1; b1=s2;
	c0=land(neg(treecopy(a1)),treecopy(b1));
	a0=xor(treecopy(a1),treecopy(b1));
	if (s0==NULL) { return treecompose("-",c0,a0); }
	c1=s0->left;
	treefree(c0);
	c0=land(treecopy(c1),treecopy(b1));
	c0=lor(c0,land(neg(treecopy(a1)),lor(treecopy(c1),treecopy(b1))));
	a0=xor(a0,treecopy(c1));
	r0=treecompose("-",c0,treecompose(";",a0,s0->right));
	treefree(s0->left); free(s0); return r0;
}
/*******************************************************************/
tree *ARITdouble(tree *s2)
{
        tree *a0=s2;

        while (a0->node[0]==';') a0=a0->right;
        treereplace(a0,treecompose(";",treecopy(a0),treeconst(0)));
	return s2;
}
tree *ARITmul(tree *x1,tree *x2)
{
	int i=0;
        tree *tt0,*r0,*r1,*r2;
        
        r0=NULL;
        while ((tt0=listgetone(x2,i++))!=NULL) {
                if (r0!=NULL) r0=ARITdouble(r0);
                switch (tt0->node[0]) {
                        case FALSE: r1=NULL; break;
                        case TRUE:  r1=treecopy(x1); break;
                        default:    r1=listcombinedist(ARITband,x1,tt0); 
				    r1=treedopost(r1,TLFnnfsimp);
				    break;
                }
		if (r1==NULL) continue;
                if (r0==NULL) { r0=r1; continue; }
                r2=listcombine(ARITadd,r0,r1); r2->node[0]=';'; 
                treefree(r0); treefree(r1); 
		r0=r2; 
		r0=treedopost(r0,TLFnnfsimp);
		r0=TLFred(r0); 
	}
        if (r0==NULL) return treeconst(0); 
	return ARITred(r0);
}
/*******************************************************************/
tree *ARITeq(tree *s1,tree *s2,tree *s0)
{
	tree *a0;

	a0=leq(treecopy(s1),treecopy(s2));
	if (s0==NULL) return a0;
	return treecompose("&",a0,s0); 
}
tree *ARITband(tree *s1,tree *s2,tree *s0)
{
        tree *a0;

        a0=land(treecopy(s1),treecopy(s2));
        if (s0==NULL) return a0;
        return treecompose(";",a0,s0);
}
tree *ARITbor(tree *s1,tree *s2,tree *s0)
{
        tree *a0;

        a0=lor(treecopy(s1),treecopy(s2));
        if (s0==NULL) return a0;
        return treecompose(";",a0,s0);
}
/*******************************************************************/
tree *ARITapply(tree *t)
{
        tree *t0,*t1=NULL;

        if (t==NULL) return NULL;

        t=TLFred(t);
        switch (t->node[0]) {
          case '+':
                t0=listcombine(ARITadd,t->left,t->right); treefree(t); 
/*
*/
		t0->node[0]=';'; simploc(t0); return t0;
/*remark
/*case 1: the extra bit, causes "TSTisassign" return 0
/*case 2: ignore the extra bit, causes mod-operation to fail*/
/*
		t1=t0->right; treefree(t0->left); free(t0); 
		simploc(t1); return t1;
*/
          case '-':
                t0=ARITsub0(t->left,t->right,&t1); treefree(t); t=t0;
                t0=listcombine(ARITsub,t->left,t->right); treefree(t); 
                t0->left=neg(t0->left);
/*
*/
/*when left < right, set t0 to 0*/
                if (t1!=NULL) t0->left=treecompose("&",t0->left,neg(t1));
                t0->right=listcombinedist(ARITband,t0->left,t0->right); 
		t=t0; t0=t0->right; treefree(t->left); free(t); 
		simploc(t0); return t0;
          case '*':
                t0=ARITmul(t->left,t->right); treefree(t); 
		simploc(t0); return t0;
          case '/':
                t0=ARITdiv(t->left,t->right); treefree(t); 
		simploc(t0); return t0;
          case '%':
                t0=ARITmod(t->left,t->right); treefree(t); 
		simploc(t0); return t0;
          case '=':
                t0=listcombine(ARITeq,t->left,t->right); treefree(t); 
		simploc(t0); return t0;
          case BOR:
                t0=listcombine(ARITbor,t->left,t->right); treefree(t); 
		simploc(t0); return t0;
          case DAND:
                t0=listcombinedist(ARITband,t->left,t->right); treefree(t); 
		simploc(t0); return t0;
          case '<':
                t0=listcombine(ARITsub,t->left,t->right); treefree(t); 
		t=t0; t0=t0->left; treefree(t->right); free(t); 
		simploc(t0); return t0;
          case '>':
                t0=listcombine(ARITsub,t->left,t->right); 
                t1=listcombine(ARITeq,t->left,t->right); treefree(t); 
		t=t0; t0=t0->left; treefree(t->right); free(t); 
		return treecompose("&",neg(t0),neg(t1));
          case 'B': t->node[0]='='; return neg(ARITapply(t));
          case '}': t->node[0]='<'; return neg(ARITapply(t));
          case '{': t->node[0]='>'; return neg(ARITapply(t));
        }
        return t;
}
/*******************************************************************/
tree *ARITred(tree *r0)
{
	tree *r2,*r1;
	int i=0;

	r2=NULL; 
	while ((r1=listgetone(r0,i++))!=NULL) { 
		r1=treecopy(r1);
		r1=TLFnnf(r1); 
		r1=TLFred(r1); 
		r1=TLFdnf(r1); 
		r2=listmerge(r2,r1);
	}
	treefree(r0); return r2;
}
/*******************************************************************/

/*******************************************************************/
tree *BOOLconstructnum(int i)
{
	tree *t0=treeconst(i%2); 
	i/=2;
	while (i) { t0=treecompose(";",treeconst(i%2),t0); i/=2; }
	return t0;
}
/*******************************************************************/
tree *BOOLconstructvar(int a,int b)
{
	tree *t0=treenum(b); 
	b--;
	while (b>=a) { t0=treecompose(";",treenum(b),t0); b--; }
	return t0;
}
/*******************************************************************/
void BOOLreplace(tree *t,tree *vlist,int booln)
{
	int boolna=0;
	int n,low,high;
	tree *v;

	if (t==NULL) return;
	if (isdigit(t->node[0])) {
		treereplace(t,BOOLconstructnum(atoi(t->node)));
		return;
	}
	switch (t->node[0]) {
	  case 'O':
		boolna=booln;
	  case 'V':
		n=arrayindex(t->node+1);
		v=treesearchvarlist(t->node+1,n,vlist); 
/*
 infos(t->node);
*/
		if (v==NULL) error("vlist");
		low=atoi(v->left->left->node)+boolna;
		high=atoi(v->left->right->node)+boolna;
		treereplace(t,BOOLconstructvar(low,high)); 
		return; 
	}

	BOOLreplace(t->left,vlist,booln); BOOLreplace(t->right,vlist,booln);
}
/*******************************************************************/
tree *BOOLconverttrans(tree *t)
{
        if (t->node[0]==';') t->node[0]='|';
        else if (t->node[0]==':') t->node[0]='&';
        return t;
}
/*******************************************************************/

/*******************************************************************/
#define ptrans prog->right->right->left
/*******************************************************************/
#define repsimp(a,b,c)	\
	BOOLreplace(a,b,c); a=treedopost(a,ARITapply); \
	a=treedopost(a,TLFnnf); a=treedopost(a,TLFnnfsimp); \
/*******************************************************************/
prg *p2bp(prg *pr1)
{
	int i,j,n;
	prg *pr2=(prg *)malloc(sizeof(prg));
	tree *vlist;
	tree *t1;

	pr2->booln=pr1->booln;
	pr2->puretrans=pr1->puretrans;

	pr2->vlist=treecopy(pr1->vlist); 
	pr2->prog=treecopy(pr1->prog); vlist=pr2->prog->left;
	treefree(pr2->ptrans); pr2->ptrans=NULL; 

	repsimp(pr2->prog->right,vlist,pr2->booln);


	pr2->varconstr=treecopy(pr1->varconstr);
	repsimp(pr2->varconstr,vlist,pr2->booln);
	
	pr2->deadlockfreecond=treecopy(pr1->deadlockfreecond);
	repsimp(pr2->deadlockfreecond,vlist,pr2->booln);

	pr2->deadlocktr=treecopy(pr1->deadlocktr);
	repsimp(pr2->deadlocktr,vlist,pr2->booln);

	pr2->prscnt=pr1->prscnt;
	pr2->prs=(process **)malloc((pr2->prscnt+1)*sizeof(process *));
	pr2->prs[pr2->prscnt]=NULL;

	for (i=0;i<pr2->prscnt;i++) {
		pr2->prs[i]=prscr(pr1->prs[i]->n);
		for (n=pr2->prs[i]->n,j=0;j<n;j++) {
		    pr2->prs[i]->tslistpc[j]=treecopy(pr1->prs[i]->tslistpc[j]);
		    pr2->prs[i]->tslist[j]=treecopy(pr1->prs[i]->tslist[j]);
		    repsimp(pr2->prs[i]->tslist[j],vlist,pr2->booln);
/*

	tree *a,*b;
	int c;
	a=pr2->prs[i]->tslist[j]; b=vlist; c=pr2->booln;


	BOOLreplace(a,b,c); 
 infoi(21);
	a=treedopost(a,ARITapply); 
 infoi(22);
	a=treedopost(a,TLFnnf); 
 infoi(23);
	a=treedopost(a,TLFnnfsimp);
 infoi(24);

	pr2->prs[i]->tslist[j]=a;
*/

		}
		pr2->prs[i]->tslist[n]=NULL; pr2->prs[i]->tslistpc[n]=NULL;
	}
/*
 vlisttreewrite(pr2->prog->left,stdout);
 t1=treesearchvarlist("nf_idct",0,pr2->prog->left); 
 treewrite(t1->left,stdout);
*/
	return pr2;
}
/*******************************************************************/

