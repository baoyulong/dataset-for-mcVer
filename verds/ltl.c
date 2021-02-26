#include "bp2s.h"
#include "tlf.h"
/*******************************************************************/
extern int opLIN;
extern int opbSTEP;
/*******************************************************************/
tree *SPECltl2f0(tree *t,int k,int lstart,int pn,tree *tr,int bv)
{
	int i;
	tree *t0,*t1,*te; 
	if (lstart==-1) {
		t0=(tree *)malloc(sizeof(tree));	
		t0->node[0]='|'; t0->node[1]=0; 
		t0->left=ltl2f0b(t,k,0,pn,lstart,0); 
		t0->right=SPECltl2f0(t,k,lstart+1,pn,tr,bv); 
		if (bv==0&&k>=opbSTEP) {
			if (opLIN&1) {
				te=thf(t,k,pn,0);	
				for (i=1;i<opbSTEP;i++) {
					te=treecompose("|",te,thf(t,k-i,pn,0));
				}
				t0->left=treecompose("&",t0->left,te);
			}
/*
 treewrite(te,stdout); exit(0);
 infoi(pn);
 treewrite(te,stdout); 
 infoi(pn);
 treewrite(t0,stdout); exit(0);
*/
		}
		return t0;
	} else { 
		t0=(tree *)malloc(sizeof(tree));
		t0->node[0]='&'; t0->node[1]=0; 
		t0->left=treetr(tr,k,lstart,pn);
		t0->right=ltl2f0b(t,k,0,pn,lstart,0); if (lstart==k) return t0;
		t1=(tree *)malloc(sizeof(tree));
		t1->node[0]='|'; t1->node[1]=0; 
		t1->left=t0; 
		t1->right=SPECltl2f0(t,k,lstart+1,pn,tr,bv); 
		if (bv==0&&lstart==1&&k>=opbSTEP) {
		    if (opLIN&2) {
				te=tef2(t,k,pn);		/*epc1a*/
				for (i=1;i<opbSTEP;i++) {
					te=treecompose("|",te,tef2(t,k-i,pn));
				}
				t1=treecompose("&",t1,te);
		    }
		    if (opLIN&4) {
				te=tef4(t,k,pn);
				t1=treecompose("&",t1,te);
		    }
		    if (opLIN&8) {
				te=eqstate(1,lstart-1,1,k,k);
				eqstateconstruct(te,pn); te=TLFnegate(te);
				t1=treecompose("&",t1,te);
		    }
		}
/*
 infoi(lstart-1); infoi(k); treewrite(te,stdout); exit(0);
*/
		return t1;
	}
}
/*******************************************************************/
tree *SPECltl2f1(tree *t,int k,int i,int pn)
{
	tree *t0,*t1,*t2; 

	switch (t->node[0]) {
		case 'X': if (k==i) return treeconst(1);
			  else return SPECltl2f1(t->right,k,i+1,pn); 
		case 'G': return makeform_ll(SPECltl2f1,t->right,k,i,k,'&',pn); 
		case 'F': return treeconst(1);
		case 'U': t0=SPECltl2f1(t->right,k,i,pn); 
			  t1=SPECltl2f1(t->left,k,i,pn);
			  if (i==k) return treecompose("|",t0,t1);
			  t2=SPECltl2f1(t,k,i+1,pn); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=SPECltl2f1(t->right,k,i,pn); 
			  if (i==k) return t0;
			  t1=SPECltl2f1(t->left,k,i,pn);
			  t2=SPECltl2f1(t,k,i+1,pn); 
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=SPECltl2f1(t->left,k,i,pn); 
			  t1=SPECltl2f1(t->right,k,i,pn); 
			  return treecompose("&",t0,t1);
		case '|': t0=SPECltl2f1(t->left,k,i,pn); 
			  t1=SPECltl2f1(t->right,k,i,pn); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  return treeaddnumber(treecopy(t),i*pn);
	}
}
/*******************************************************************/
tree *makeform_ll(tree *f(),tree *t,int k,int i,int j,char c,int pn)
{
	tree *t0,*t1;;

	if (i>j) return NULL;
	if (i==j) return f(t,k,i,pn); 
	t0=(tree *)malloc(sizeof(tree));	
	t0->node[0]=c; t0->node[1]=0; 
	t0->left=f(t,k,i,pn); t0->right=makeform_ll(f,t,k,i+1,j,c,pn); 
	return t0;
}
/*******************************************************************/
tree *makeform_l0(tree *f(),tree *t,int k,int i,int j,char c,int pn,int ll)
{
	tree *t0,*t1;;

	if (i>j) return NULL;
	if (i==j) return f(t,k,i,pn,ll); 
	t0=(tree *)malloc(sizeof(tree));	
	t0->node[0]=c; t0->node[1]=0; 
	t0->left=f(t,k,i,pn,ll,0); t0->right=makeform_l0(f,t,k,i+1,j,c,pn,ll); 
	return t0;
}
/*******************************************************************/
tree *ltl2f0b(tree *t,int k,int i,int pn,int ll,int j)
{
	tree *t0,*t1,*t2,*t3; 

	switch (t->node[0]) {
		case 'X': if (k==i&&ll>=0) i=ll; else i=i+1;
			  if (i>k) return treeconst(0); 
			  return ltl2f0b(t->right,k,i,pn,ll,j); 
			  /*zwh201503(i+1->i)*/
		case 'G': if (ll<0) return treeconst(0); 
			  if (ll<i) i=ll;
			  return makeform_l0(ltl2f0b,t->right,k,i,k,'&',pn,ll);
		case 'F': if (i>ll&&ll>=0) i=ll;
			  return makeform_l0(ltl2f0b,t->right,k,i,k,'|',pn,ll); 
		case 'U': t->node[0]++; t0=ltl2f0b(t,k,i,pn,ll,k); t->node[0]--;
			  if (!(i>ll&&ll>=0)) return t0;
			  t1=makeform_l0(ltl2f0b,t->left,k,i,k,'&',pn,ll); 
			  t->node[0]++; t2=ltl2f0b(t,k,ll,pn,ll,i-1); 
			  t->node[0]--;
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': 
			  error("Unexpected LTL operator");
			  /*problems not fixed, but replace with U*/
			  t->node[0]++; t0=ltl2f0b(t,k,i,pn,ll,k); t->node[0]--;
			  if (!(ll>=0)) return t0;
			  t1=makeform_l0(ltl2f0b,t->left,k,i,k,'&',pn,ll); 
		          if (i<=ll) return treecompose("|",t0,t1);
			  t->node[0]++; t2=ltl2f0b(t,k,ll,pn,ll,i-1); 
			  t->node[0]--;
			  t3=makeform_l0(ltl2f0b,t->right,k,ll,i-1,'&',pn,ll); 
			  t2=treecompose("|",t2,t3);
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'V': t0=ltl2f0b(t->right,k,i,pn,ll,j); 
			  if (i==j) return t0;
			  t1=ltl2f0b(t->left,k,i,pn,ll,j); 
			  t2=ltl2f0b(t,k,i+1,pn,ll,j); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'S': t0=ltl2f0b(t->right,k,i,pn,ll,j); 
			  t1=ltl2f0b(t->left,k,i,pn,ll,j); 
			  if (i==j) return treecompose("&",t0,t1);
			  t2=ltl2f0b(t,k,i+1,pn,ll,j); 
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=ltl2f0b(t->left,k,i,pn,ll,j); 
			  t1=ltl2f0b(t->right,k,i,pn,ll,j); 
			  return treecompose("&",t0,t1);
		case '|': t0=ltl2f0b(t->left,k,i,pn,ll,j); 
			  t1=ltl2f0b(t->right,k,i,pn,ll,j); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  return treeaddnumber(treecopy(t),i*pn);
	}
}
/*******************************************************************/
