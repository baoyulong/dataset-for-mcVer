#include "bp2s.h"
/*******************************************************************/
tree *SPECactl2f0CTLR(tree *t,int k,int pn)
{
	return CTLRactl2f0a(t,k,pn,0,0,0); 
}
tree *SPECactl2f1CTLR(tree *t,int k,int pn)
{
	return CTLRactl2f1a(t,k,pn,0,0,0); 
}
/*******************************************************************/
tree *eqpathj(int pa,int k,int j)
{
        int i;
        tree *t0;

        if (j==0) return treeconst(0);

        t0=eqstate(pa,0,pa,j,k);
        for (i=1;i<j;i++) t0=treecompose("|",t0,eqstate(pa,i,pa,j,k));
        return t0;
}
/*******************************************************************/
tree *CTLRactl2f1a(tree *t,int k,int pn,int pa,int x,int y)
{
	tree *t0,*t1,*t2; 
	switch (t->node[0]) {
		case 'A': t1=CTLRactl2f1a(t->right,k,pn,pa+1,pa+1,0);
			  if (x==0) return t1;
			  t0=eqstate(x,y,pa+1,0,k);
			  return treecompose(":",t0,t1);
		case 'X': if (y==k) return treeconst(0);
			  return t1=CTLRactl2f1a(t->right,k,pn,pa,x,y+1);
		case 'F': t0=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=CTLRfkx(t->right,k);
			  t2=CTLRactl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,t2);
		case 'G': if (t->left!=NULL) 
			  return makeform_ctl(CTLRactl2f1a,
				t->right,k,pn,pa,x,y,atoi(t->left->node));
			  if (k==0) return treeconst(0);
			  t0=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k-1) 
			  return treecompose("&",t0,eqpathj(x,k,y+1));
			  t2=CTLRactl2f1a(t,k,pn,pa,x,y+1); 
			  if (y<k-1) t2=treecompose("|",eqpathj(x,k,y+1),t2);
			  return treecompose("&",t0,t2);
		case 'U': t0=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=CTLRfkx(t->right,k);
			  t1=CTLRactl2f1a(t->left,k,pn,pa,x,y); 
			  t2=CTLRactl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  t1=CTLRactl2f1a(t->left,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0,t1); 
			  pa+=CTLRfkx(t->left,k);
			  t2=CTLRactl2f1a(t,k,pn,pa,x,y+1); 
			  if (y<=k-1) t2=treecompose("|",eqpathj(x,k,y+1),t2);
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=CTLRactl2f1a(t->left,k,pn,pa,x,y); 
			  t1=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("&",t0,t1);
		case '|': t0=CTLRactl2f1a(t->left,k,pn,pa,x,y); 
			  pa+=fkx(t->left,k); /*201503zwh*/
			  t1=CTLRactl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  
			if (k==0||x==0) x=1;
			return treeaddnumber(treecopy(t),(x-1)*pn*(k+1)+y*pn);
	}
}
/*******************************************************************/
tree *CTLRactl2f0a(tree *t,int k,int pn,int pa,int x,int y)
{
	tree *t0,*t1,*t2; 
	switch (t->node[0]) {
		case 'E': t1=CTLRactl2f0a(t->right,k,pn,pa+1,pa+1,0);
			  if (x==0) return t1;
			  t0=eqstate(x,y,pa+1,0,k);
			  return treecompose(":",t0,t1);
		case 'X': if (y==k) return treeconst(0);
			  return t1=CTLRactl2f0a(t->right,k,pn,pa,x,y+1);
		case 'F': t0=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  t2=CTLRactl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,t2);
		case 'G': if (t->left!=NULL) 
			  return makeform_ctl(CTLRactl2f0a,
				t->right,k,pn,pa,x,y,atoi(t->left->node));
			  if (k==0) return treeconst(0);
			  t0=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
/*
			  if (y==k-1) return treecompose("&",t0,eqpath(x,k));
*/
			  if (y==k-1) 
			  return treecompose("&",t0,eqpathj(x,k,y+1));
			  pa+=CTLRgkx(t->right,k);
			  t2=CTLRactl2f0a(t,k,pn,pa,x,y+1); 
/*
			  if (y<k-1) t2=treecompose("|",eqpathj(x,k,y+1),t2);
*/
			  return treecompose("&",t0,t2);
		case 'U': t0=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  t1=CTLRactl2f0a(t->left,k,pn,pa,x,y); 
			  pa+=CTLRgkx(t->left,k);
			  t2=CTLRactl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
			  pa+=CTLRgkx(t->right,k);
			  t1=CTLRactl2f0a(t->left,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0,t1);
			  t2=CTLRactl2f0a(t,k,pn,pa,x,y+1); 
			  if (y==k-1) t2=treecompose("|",eqpathj(x,k,y+1),t2);
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=CTLRactl2f0a(t->left,k,pn,pa,x,y); 
			  pa+=fkx(t->left,k); /*201503zwh*/
			  t1=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
			  return treecompose("&",t0,t1);
		case '|': t0=CTLRactl2f0a(t->left,k,pn,pa,x,y); 
			  t1=CTLRactl2f0a(t->right,k,pn,pa,x,y); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  
			if (k==0||x==0) x=1;
			return treeaddnumber(treecopy(t),(x-1)*pn*(k+1)+y*pn);
	}
}
/*******************************************************************/
int CTLRfkx(tree *t,int k)
{
        int x,y,z;
        if (t==NULL|k==0) return 0;
        switch (t->node[0]) {
                case '&': x=CTLRfkx(t->left,k); y=CTLRfkx(t->right,k); return x>y?x:y;
                case '|': x=CTLRfkx(t->left,k); y=CTLRfkx(t->right,k); return x+y;
                case 'A': return CTLRfkx(t->right,k)+1;
                case 'X': return CTLRfkx(t->right,k);
                case 'F': return (k+1)*CTLRfkx(t->right,k);
                case 'G': return CTLRfkx(t->right,k);
                case 'U': x=CTLRfkx(t->left,k); y=CTLRfkx(t->right,k);
                          z=(x>y?x:y); return k*y+z;
                case 'R': x=CTLRfkx(t->left,k); y=CTLRfkx(t->right,k);
                          z=(x>y?x:y); return k*x+z;
		default: return 0;
        }
}
/*******************************************************************/
int CTLRgkx(tree *t,int k)
{       
        int x,y,z;
        if (t==NULL) return 0;
        switch (t->node[0]) {
                case '&': x=CTLRgkx(t->left,k); y=CTLRgkx(t->right,k); return x+y;
                case '|': x=CTLRgkx(t->left,k); y=CTLRgkx(t->right,k); return x>y?x:y;
                case 'E': return CTLRgkx(t->right,k)+1;
                case 'X': return CTLRgkx(t->right,k);
                case 'F': return CTLRgkx(t->right,k);
/*
                case 'G': return (k+1)*CTLRgkx(t->right,k);
*/
                case 'G': return (k)*CTLRgkx(t->right,k);
                case 'U': x=CTLRgkx(t->left,k); y=CTLRgkx(t->right,k); return k*x+y;
                case 'R': x=CTLRgkx(t->left,k); y=CTLRgkx(t->right,k); return (k+1)*y+x;
		default: return 0;
        }
}
/*******************************************************************/
