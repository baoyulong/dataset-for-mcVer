#include "bp2s.h"
/*******************************************************************/
tree *SPEClin2f0(tree *t,int k,int lstart,int pn,int pa,int pak,tree *tr)
{
	int i;
	tree *t0,*t1; 
	int d1=depLIN(t);

	if (lstart==-1) {
		t0=(tree *)malloc(sizeof(tree));	
		t0->node[0]='|'; t0->node[1]=0; 
		t0->left=lin2f0b(t,k,pa,pak,0,pn,lstart,0); 
		t0->right=SPEClin2f0(t,k,lstart+1,pn,pa,pak,tr); 
		for (i=1;i<d1;i++) {
			t1=treetr(tr,(pa-1+i)*(k+1)-1,(pa-1+i)*(k+1),pn);
			t0=treecompose("&",t0,t1);
		}
		return t0;
	} else { 
		t0=(tree *)malloc(sizeof(tree));
		t0->node[0]='&'; t0->node[1]=0; 
/*
		if ((pa-1)*(k+1)+lstart<pak-k) lstart=pak-k-(pa-1)*(k+1);
*/
		t0->left=treetr(tr,(pa-1)*(k+1)+pak,(pa-1)*(k+1)+lstart,pn);
		t0->right=lin2f0b(t,k,pa,pak,0,pn,(pa-1)*(k+1)+lstart,0); 
		if (lstart==pak) return t0;
		t1=(tree *)malloc(sizeof(tree));
		t1->node[0]='|'; t1->node[1]=0; 
		t1->left=t0; 
		t1->right=SPEClin2f0(t,k,lstart+1,pn,pa,pak,tr); return t1;
	}
}
/*******************************************************************/
tree *makeform_lin0(tree *f(),tree *t,int k,int pa,int pak,int i,int j,char c,int pn,int ll)
{
	tree *t0,*t1;;

	if (i>j) return NULL;
	if (i==j) return f(t,k,pa,pak,i,pn,ll); 
	t0=(tree *)malloc(sizeof(tree));	
	t0->node[0]=c; t0->node[1]=0; 
	t0->left=f(t,k,pa,pak,i,pn,ll,0); 
	t0->right=makeform_lin0(f,t,k,pa,pak,i+1,j,c,pn,ll); 
	return t0;
}
/*******************************************************************/
tree *lin2f0b(tree *t,int k,int pa,int pak,int i,int pn,int ll,int j)
{
	tree *t0,*t1,*t2,*t3; 

	switch (t->node[0]) {
		case 'E': return lin2f0b(t->right,k,pa,pak,i,pn,ll,j); 
		case 'X': if (pak==i&&ll>=0) i=ll; else i=i+1;
			  if (i>pak) return treeconst(0); 
			  return lin2f0b(t->right,k,pa,pak,i,pn,ll,j); 
		case 'G': if (ll<0) return treeconst(0); 
			  if (ll<i) i=ll;
			  return makeform_lin0(lin2f0b,
					t->right,k,pa,pak,i,pak,'&',pn,ll);
		case 'F': if (i>ll&&ll>=0) i=ll;
			  return makeform_lin0(lin2f0b,
					t->right,k,pa,pak,i,pak,'|',pn,ll); 
		case 'U': t->node[0]++; t0=lin2f0b(t,k,pa,pak,i,pn,ll,pak); 
			  t->node[0]--;
			  if (!(i>ll&&ll>=0)) return t0;
			  t1=makeform_lin0(lin2f0b,t->left,k,pa,pak,i,k,'&',pn,ll); 
			  t->node[0]++; t2=lin2f0b(t,k,pa,pak,ll,pn,ll,i-1); 
			  t->node[0]--;
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t->node[0]++; t0=lin2f0b(t,k,pa,pak,i,pn,ll,pak); 
			  t->node[0]--;
			  if (!(ll>=0)) return t0;
			  t1=makeform_lin0(lin2f0b,t->left,k,pa,pak,i,pak,'&',pn,ll); 
		          if (i<=ll) return treecompose("|",t0,t1);
			  t->node[0]++; t2=lin2f0b(t,k,pa,pak,ll,pn,ll,i-1); 
			  t->node[0]--;
			  t3=makeform_lin0(lin2f0b,t->right,k,pa,pak,ll,i-1,'&',pn,ll); 
			  t2=treecompose("|",t2,t3);
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'V': t0=lin2f0b(t->right,k,pa,pak,i,pn,ll,j); 
			  if (i==j) return t0;
			  t1=lin2f0b(t->left,k,pa,pak,i,pn,ll,j); 
			  t2=lin2f0b(t,k,pa,pak,i+1,pn,ll,j); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'S': t0=lin2f0b(t->right,k,pa,pak,i,pn,ll,j); 
			  t1=lin2f0b(t->left,k,pa,pak,i,pn,ll,j); 
			  if (i==j) return treecompose("&",t0,t1);
			  t2=lin2f0b(t,k,pa,pak,i+1,pn,ll,j); 
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=lin2f0b(t->left,k,pa,pak,i,pn,ll,j); 
			  t1=lin2f0b(t->right,k,pa,pak,i,pn,ll,j); 
			  return treecompose("&",t0,t1);
		case '|': t0=lin2f0b(t->left,k,pa,pak,i,pn,ll,j); 
			  t1=lin2f0b(t->right,k,pa,pak,i,pn,ll,j); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  
			  return treeaddnumber(treecopy(t),(pa-1)*pn*(k+1)+i*pn);
	}
}
/*******************************************************************/
