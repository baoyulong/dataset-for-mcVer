#include "clk.h"
#include "tlf.h"


/************************************************************************/
#define progfair	prog->right->right->right->left
/************************************************************************/
tree *clkcheckvlist(tree *prog)
{
	tree *vlist=prog->left;
	tree *t;
	int lc=0;
	int i;
	int t0;
	tree *a0,*a1,*a2,*a3,*x1,*x0;
	tree *cond=NULL,*act=NULL,*ac0=NULL;
	tree *stutter;
	int lcm=1,mx;

	while ((t=listgetone(vlist,lc++))!=NULL) 
	if (strncmp(t->right->node,"char",4)==0) {
		sprintf(t->right->node,"0..127");
	} else if (strncmp(t->right->node,"i",1)==0) {
		t0=atoi(t->right->node+1); 
		switch (t0) {
			case  0: mx=1;
			case 31: mx=0x8fffffff;
			default: for (mx=1,i=0;i<t0;i++) mx*=2; mx-=1;
		}
		sprintf(t->right->node,"0..%i",mx);
	}
	lc=0;
	while ((t=listgetone(vlist,lc++))!=NULL) 
	if (strncmp(t->right->node,"clock",5)==0) {
		t0=atoi(t->right->node+5); if (t0==0) t0=1;
		i=clkcheckcst(t->left->node,prog->right,t0);
		lcm=(lcm*i)/gcd(lcm,i);
		sprintf(t->right->node,"clock%i",i);
	}
	lc=0;
	while ((t=listgetone(vlist,lc++))!=NULL) 
	if (strncmp(t->right->node,"clock",5)==0) {
		i=clkcheckcst(t->left->node,prog->right,-lcm); 
		sprintf(t->right->node,"clock%i",i);
	}
	lc=0;
	while ((t=listgetone(vlist,lc++))!=NULL) 
	if (strncmp(t->right->node,"clock",5)==0) {
		mx=atoi(t->right->node+5); t0=1;
		sprintf(t->right->node,"0..%i",mx+t0);
		a1=treecompose(t->left->node,NULL,NULL);
		a3=treecompose("{",a1,treenum(mx)); 

		if (cond==NULL) cond=a3; else cond=treecompose("|",a3,cond);

		a1=treecompose(t->left->node,NULL,NULL);
		x0=treecompose("{",treecopy(a1),treenum(mx)); 
		a2=treecompose("N",x0,treecompose("+",a1,treenum(t0)));

		a1=treecompose(t->left->node,NULL,NULL);
		a3=treecompose("=",treecopy(a1),treenum(mx+t0)); 
		a3=treecompose("N",a3,a1);

		a1=treecompose(t->left->node,NULL,NULL);
/*
		a3=treecompose("=",a1,a2->right);
*/
		a3=treecompose("=",a1,treecompose("M",a2,a3));
		
		if (act==NULL) act=a3; else act=treecompose("&",a3,act);
                a0=treecompose("=",treecopy(a1),treecopy(a1));
                if (ac0==NULL) ac0=a0; else ac0=treecompose("&",a0,ac0);

		for (a1=progfair;a1!=NULL;) {
			if (a1->node[0]==';') x1=a1->left; else x1=a1;
			if (x1->node[0]==':') {
			   treesubstitute(x1->right,x0->left->node,a2->right);
			}
			if (a1->node[0]==';') a1=a1->right; else a1=NULL;
		}
		a1=treecompose("=",treecopy(x0->left),treenum(0));
	        prog->right->left=listmerge(prog->right->left,a1);
	}
	if (act==NULL) return NULL;
        list2cn(prog->right->left);
	for (a2=a3=NULL,a1=progfair;a1!=NULL;) {
		if (a1->node[0]==';') x1=a1->left; else x1=a1;
		if (x1->node[0]!=':') a2=listmerge(a2,treecopy(x1)); 
		else {
			x1->node[0]='|'; x1->left=TLFnegate(x1->left);
			a3=listmerge(a3,treecopy(x1));
		}
		if (a1->node[0]==';') a1=a1->right; else a1=NULL;
	}
        treefree(progfair); progfair=a2;
        stutter=treecompose(":",TLFnegate(treecopy(cond)),ac0);
        if (a3!=NULL) { list2cn(a3); cond=treecompose("&",a3,cond); }
        return treecompose(";",stutter,treecompose(":",cond,act));

}
/************************************************************************/
int clkcheckcst(char *node,tree *t,int i)
{
	char *x;
	float f;
	int x0,x1,y;
	tree *t0,*t1;

	if (t==NULL) return 1;
	if (t->left==NULL||t->right==NULL) return 1;

	if (strcmp(t->left->node,node)==0) { t0=t->left; t1=t->right; }
	else if (strcmp(t->right->node,node)==0) { t1=t->left; t0=t->right; }
	else {
		x0=clkcheckcst(node,t->left,i);
		x1=clkcheckcst(node,t->right,i);
		if (i>0) return (x0*x1)/gcd(x0,x1);
		return (x0>x1?x0:x1);
	}

	if (isdigit(t1->node[0])) treereplace(t1,fractal(atof(t1->node)));
	if (strcmp(t1->node,"/")) { error("syntax of fractals"); }
	if (i<0) {
		x=t1->node;
		x0=atoi(t1->left->node);
		x1=atoi(t1->right->node);
		y=(-i)*(x0)/(x1);
		sprintf(x,"%i",y);
		free(t1->left); t1->left=NULL;
		free(t1->right); t1->right=NULL;
		return y;
	}
	sprintf(t1->right->node,"%i",i*atoi(t1->right->node));
	y=atoi(t1->right->node);
	return y;
}
/************************************************************************/
int gcd(int a,int b)  
{  
	int r=a%b;  
    	while (r!=0)  {  a=b;  b=r;  r=a%b;  }  
    	return b;  
}  
/************************************************************************/
tree *fractal(float a)
{
        int y;
        int x=(int)(a*100000);

        if (x==0) { x=0; y=1; } else { y=gcd(x,100000); x=x/y; y=100000/y; }
        return treecompose("/",treenum(x),treenum(y));
}
/************************************************************************/
void treesubstitute(tree *t,char *node,tree *b)
{
	if (t==NULL) return;
	if (strcmp(t->node,node)==0) { treereplace(t,treecopy(b)); return; }
	treesubstitute(t->left,node,b); treesubstitute(t->right,node,b);
}
/************************************************************************/
void list2cn(tree *t)
{
	if (t->node[0]==';') { t->node[0]='&'; list2cn(t->right); }
}
/************************************************************************/


/************************************************************************/
#define skip 		treecompose("=",treestr("1"),treestr("1"))
/************************************************************************/
#define isskip(t0) 	(\
	strcmp(t0->node,"=")==0&&\
	strcmp(t0->left->node,"1")==0&&\
	strcmp(t0->right->node,"1")==0)\
/************************************************************************/
tree *intcheckvlist(module *mm,tree *prog,char *rt)
{
	int k;
	tree *vlist=prog->left;
	tree *init=prog->right->left;
	tree *trans=prog->right->right->left;
	int lc=0;
	int max=0;
	tree *cond,*act,*t0,*t=NULL;

        for (;mm!=NULL;mm=mm->next) {
		k=intcheckcstprog(mm->prog); if (k>max) max=k;
	}
/*
	while ((t=listgetone(vlist,lc++))!=NULL) 
	if (strncmp(t->right->node,"i",1)==0) {
		k=intcheckcst(t->left->node,prog); if (k>max) max=k;
	}
*/
	lc=0;
	while ((t0=listgetone2(trans,lc++))!=NULL) {
		if ((cond=intconstruct(t0->right,vlist,max))==NULL) continue;
		cond=treecompose("&",cond,treecopy(t0->left));
		act=treecompose("=",treestr("err_x_"),treenum(1)); 
		if (rt!=NULL) act=treecompose("&",act,treecompose("=",treestr(rt),treenum(1))); 
		t0=treecompose(":",cond,act);
		if (t==NULL) t=t0; else t=treecompose(";",t0,t);
	}
 	lc=0;
	while ((t0=listgetone2(init,lc++))!=NULL) {
		if ((cond=intconstruct(t0,vlist,max))==NULL) continue;
		if (isskip(cond)) treereplace(t0,skip); /*see bubblesort*/
		act=treecompose("=",treestr("err_x_"),treenum(1)); 
		if (rt!=NULL) act=treecompose("&",act,treecompose("=",treestr(rt),treenum(1))); 
		t0=treecompose(":",cond,act);
		if (t==NULL) t=t0; else t=treecompose(";",t0,t);
	}
/*
 infoi(100); treewrite(t,stderr);
*/
	return t;
}
/************************************************************************/
int intcheckcst(char *node,tree *t)
{
	char *x;
	float f;
	int x0,x1,y;
	tree *t0,*t1;

	if (t==NULL) return 0;
	if (t->left==NULL) return intcheckcst(node,t->right);
	if (t->right==NULL) return intcheckcst(node,t->left);

	if (strcmp(t->left->node,node)==0) { t0=t->left; t1=t->right; }
	else if (strcmp(t->right->node,node)==0) { t1=t->left; t0=t->right; }
	else {
		x0=intcheckcst(node,t->left);
		x1=intcheckcst(node,t->right);
		return (x0>x1?x0:x1);
	}
	return atoi(t1->node);
}
/************************************************************************/
tree *intconstruct(tree *t,tree *vlist,int k)
{
	int i,i0,mx;
	tree *t0,*t1;

	if (t==NULL) return NULL;
	switch (t->node[0]) {
	    case '&':
	    case '|': t0=intconstruct(t->left,vlist,k);
		      t1=intconstruct(t->right,vlist,k);
		      if (t0==NULL) return t1;
		      if (t1==NULL) return t0; return treecompose("|",t0,t1);
	    case '>':
	    case '}':
	    case '<':
	    case '{':
	    case '=': t0=treesearchvarlist(t->left->node,0,vlist);
		      if (t0==NULL) {
			if (i=arrayindex(t->left->node))
	    		t0=treesearchvarlist(t->left->node,i,vlist);
		      }
/*
 infoi(100);
 treewrite(t->left,stderr);
 treew(vlist);
*/
		      if (t0==NULL) error("intconstruct");
		      if (strncmp(t0->right->node,"i",1)==0) {
			i0=atoi(t0->right->node+1); if (i0==0) i0=1;
                	switch (i0) {
                        	case  0: mx=1;
                        	case 31: mx=0x8fffffff;
                        	default: for (mx=1,i=0;i<i0;i++) mx*=2; mx-=1;
                	}
			if (mx<=k) { 
                        	printf("INFO:       CONSTANT=%i\n",k);
                        	reportresult(0,-2); exit(0);
			}
			if (t->right==NULL||t->right->node[0]==0) return NULL;
			if (isdigit(t->right->node[0])) {
				k=atoi(t->right->node);
				if (k>mx) return skip; return NULL;
			} 
			return treecompose(">",treecopy(t->right),treenum(mx));
		      } else if (strncmp(t0->right->node,"0..",3)==0) {
			i0=atoi(t0->right->node+3); 
			if (t->right==NULL||t->right->node[0]==0) return NULL;
			if (isdigit(t->right->node[0])) {
				k=atoi(t->right->node);
				if (k>i0) return skip; return NULL;
			} 
			return treecompose(">",treecopy(t->right),treenum(i0));
		      }
	}
	return NULL;
}
/************************************************************************/
int intcheckcstprog(tree *prog)
{
	int k;
	tree *vlist=prog->left;
	int lc=0;
	int max=0;
	tree *t;

	while ((t=listgetone(vlist,lc++))!=NULL) 
	if (strncmp(t->right->node,"i",1)==0) {
		k=intcheckcst(t->left->node,prog); 
		if (k>max) max=k;
	}
	return max;
}
/************************************************************************/
