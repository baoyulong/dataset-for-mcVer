#include "bp2s.h"
#include "tlf.h"
/*******************************************************************/
tree *thf(tree *t,int k,int pn,int j)
{
	int i;
	tree *t0,*t1; 

	t0=thf0(t,k,pn,j);
	t1=suCond(t0,k,pn,j); t1=TLFsimplifyFALSE(t1);
/*
        treewrite(t0,stdout); infoi(100);
        treewrite(t1,stdout); exit(0);
*/

	treefree(t0); return t1;
}
/*******************************************************************/
tree *tef2(tree *t,int k,int pn)
{
	tree *t0,*t1; 

	t0=tef2i(t);
/*
	t1=neCond(t0,k,pn); t1=TLFsimplifyFALSE(t1);
*/
	t1=tef4Cd(t0,k,pn); t1=TLFsimplifyFALSE(t1);

/*
        treewrite(t0,stdout); infoi(100);
        treewrite(t1,stdout); exit(0);
*/

	treefree(t0); return t1;
}
/*******************************************************************/
tree *tef2i(tree *t)
{
	int i;
	tree *t0,*t1; 

	if (isProp(t)) return treeconst(0);
	switch (t->node[0]) {
	  case 'X': return tef2i(t->right);
	  case 'G': return treecopy(t->right);
	  case 'F': return treecompose("|",treecopy(t->right),tef2i(t->right));
	  case 'U': t0=treecompose("|",treecopy(t->right),tef2i(t->right));
	  	    return treecompose("|",t0,tef2i(t->left));
	  case '&': return treecompose("|",tef2i(t->left),tef2i(t->right));
	  case '|': return treecompose("|",tef2i(t->left),tef2i(t->right));
	  default:  error("tef1");
	}
}
/*******************************************************************/
#define thf0x(t0)	thf0(t0,k,pn,j)
/*******************************************************************/
tree *thf0(tree *t,int k,int pn,int j)
{
	int i;
	tree *t0,*t1; 

	if (j==k) return treecopy(t);
	if (isProp(t)) { return treeconst(0); }
	switch (t->node[0]) {
	  case 'X': if (j<k) return thf0(t->right,k,pn,j+1);return treeconst(0);
	  case 'G': return treeconst(0);
	  case 'U': t0=thf0(t->right,k,pn,k);
		    for (i=0;i<k;i++) {
			t0=treecompose("|",thf0(t->left,k,pn,i),t0);
			t0=treecompose("|",thf0(t->right,k,pn,i),t0);
		    }
		    return t0;
	  case 'F': t0=thf0(t->right,k,pn,k);
		    for (i=0;i<k;i++) {
			t0=treecompose("|",thf0(t->right,k,pn,i),t0);
		    }
		    return t0;
	  case '&': if (isProp(t->left)) return thf0x(t->right);
	  	    if (isProp(t->right)) return thf0x(t->left);
	  	    return treecompose("|",thf0x(t->left),thf0x(t->right));
	  case '|': return treecompose("|",thf0x(t->left),thf0x(t->right));
	  default:  error("thf0");
	}
}
/*******************************************************************/
tree *tef4(tree *t,int k,int pn)
{
	tree *t0,*t1; 

	t0=tef4i(t);
	t1=tef4Cd(t0,k,pn); t1=TLFsimplifyFALSE(t1);
/*
        treewrite(t0,stdout); infoi(100);
        treewrite(t1,stdout); exit(0);
*/

	treefree(t0);
	return t1;
}
/*******************************************************************/
#define neCondx(t) neCond(t,k,pn)
/*******************************************************************/
tree *neCond(tree *t,int k,int pn)
{
	if (isProp(t)) return treeaddnumber(treecopy(t),k*pn);
	switch (t->node[0]) {
	  case 'G': return neCond(t->right,k,pn);
	  case 'F': 
	  case 'X': return treeconst(1);
	  case 'U': return treecompose("|",neCondx(t->left),neCondx(t->right));
	  case '|': return treecompose("|",neCondx(t->left),neCondx(t->right));
	  case '&': return treecompose("&",neCondx(t->left),neCondx(t->right));
	  default:  error("neCond");
	}
}
/*******************************************************************/
#define suCondx(t0)	suCond(t0,k,pn,j)
/*******************************************************************/
tree *suCond(tree *t,int k,int pn,int j)
{
	int i;
	tree *t0,*t1; 

	if (isProp(t)) return treeaddnumber(treecopy(t),k*pn);
	switch (t->node[0]) {
	  case 'G': return treeconst(0);
	  case 'X': return treeconst(0);
	  case 'F': 
	  case 'U': return suCondx(t->right);
	  case '|': return treecompose("|",suCondx(t->left),suCondx(t->right));
	  case '&': return treecompose("&",suCondx(t->left),suCondx(t->right));
	  default:  error("suCond");
	}
}
/*******************************************************************/
tree *tef4i(tree *t)
{
	int i;
	tree *t0,*t1; 

	if (isProp(t)) return treeconst(0);
	switch (t->node[0]) {
	  case 'X': return tef4i(t->right);
	  case 'G': return treecopy(t->right);
	  case 'F': return tef4i(t->right);
	  case 'U': return treecompose("|",tef4i(t->left),tef4i(t->right));
	  case '&': return treecompose("|",tef4i(t->left),tef4i(t->right));
	  case '|': return treecompose("|",tef4i(t->left),tef4i(t->right));
	  default:  error("tef41");
	}
}
/*******************************************************************/
#define tef4Cdx(t0)	tef4Cd(t0,k,pn)
/*******************************************************************/
tree *tef4Cd(tree *t,int k,int pn)
{
	int i;
	tree *t0,*t1; 

	if (isProp(t)) return treeaddnumber(treecopy(t),k*pn);
	switch (t->node[0]) {
	  case 'X': return tef4Cdx(t->right);
	  case 'G': return tef4Cdx(t->right);
	  case 'F': return tef4Cdx(t->right);
	  case 'U': return tef4Cdx(t->right);
/*
	  case '&': if (isProp(t->left)) return treecopy(t->left);
	  	    if (isProp(t->right)) return treecopy(t->right);
		    return treecompose("|",tef4Cdx(t->left),tef4Cdx(t->right));
*/
	  case '&': if (isProp(t->left)) return tef4Cdx(t->right);
	  	    if (isProp(t->right)) return tef4Cdx(t->left);
		    return treecompose("|",tef4Cdx(t->left),tef4Cdx(t->right));
	  case '|': return treecompose("|",tef4Cdx(t->left),tef4Cdx(t->right));
	  default:  error("tef4Cdx");
	}
}
/*******************************************************************
*old
********************************************************************
tree *tef4(tree *t,int k,int pn)
{
	tree *t0,*t1; 

        t0=teff0i(t);
        t0=teff0a(t0);
                        
        t1=neCond(t0,k,pn); t1=TLFsimplifyFALSE(t1);

	treefree(t0);
	return t1;
}
tree *tef4i(tree *t)
{
	int i;
	tree *t0,*t1; 

	if (isProp(t)) return treeconst(0);
	switch (t->node[0]) {
	  case 'X': return tef4i(t->right);
	  case 'G': return treecopy(t->right);
	  case 'F': return treecopy(t->right);
	  case 'U': return treecompose("|",treecopy(t->right),tef4i(t->left));
	  case '&': return treecompose("|",tef4i(t->left),tef4i(t->right));
	  case '|': return treecompose("|",tef4i(t->left),tef4i(t->right));
	  default:  error("tef41");
	}
}
tree *tef4a(tree *t)
{
	int i;
	tree *t0,*t1; 

	if (isProp(t)) return (t);
	switch (t->node[0]) {
	  case 'X': return tef4a(t->right);
	  case 'G': return treecopy(t->right);
	  case 'F': return treecopy(t->right);
	  case 'U': return treecopy(t->right);
	  case '&': if (isProp(t->left)) return tef4a(t->right);
	  	    if (isProp(t->right)) return tef4a(t->left);
		    return treecompose("|",tef4a(t->left),tef4a(t->right));
	  case '|': return treecompose("|",tef4a(t->left),tef4a(t->right));
	  default:  error("tef41");
	}
}
/*******************************************************************/
