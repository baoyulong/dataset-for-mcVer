#include "bp2s.h"
/*******************************************************************/
tree *SPECactl2f0(tree *t,int k,int pn)
{
	tree *t0;

	t0=actl2f0a(t,k,pn,0,0,0); 
/*
	t0=actlreplaceeq(t0,vars,pn);
        t0=treetraverse(t0,NULL,NULL,cleanbool);
*/
	return t0;
}
tree *SPECactl2f1(tree *t,int k,int pn)
{
	tree *t0;

	t0=actl2f1a(t,k,pn,0,0,0); 
/*
	t0=actlreplaceeq(t0,vars,pn);
	return negate(t0);
*/
	return t0;
}
/*******************************************************************/
tree *eqpath(int pa,int k)
{
        int i,j;
        tree *t0;

        if (k==0) return treeconst(0);
        t0=eqstate(pa,0,pa,1,k);

        for (i=0;i<k;i++)
        for (j=i+1;j<=k;j++) {
                if (j==1) continue;
                t0=treecompose("|",t0,eqstate(pa,i,pa,j,k));
        }
        return t0;
}
/*******************************************************************/
tree *makeform_ctl(tree *f(),tree *t,int k,int pn,int pa,int x,int y,int j)
{
	int i;
	tree *t0;

	if (k<j) return treeconst(0);
	t0=f(t,k,pn,pa,x,y);
	for (i=1;i<=j;i++) t0=treecompose("&",f(t,k,pn,pa,x,y+i),t0);
	return t0;
}
/*******************************************************************/
tree *actl2f1a(tree *t,int k,int pn,int pa,int x,int y)
{
	tree *t0,*t1,*t2; 
	switch (t->node[0]) {
		case 'A': t1=actl2f1a(t->right,k,pn,pa+1,pa+1,0);
			  if (x==0) return t1;
			  t0=eqstate(x,y,pa+1,0,k);
			  return treecompose(":",t0,t1);
		case 'X': if (y==k) return treeconst(0);
			  return t1=actl2f1a(t->right,k,pn,pa,x,y+1);
		case 'F': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=fkx(t->right,k);
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,t2);
		case 'G': if (t->left!=NULL) 
			  return makeform_ctl(actl2f1a,
				t->right,k,pn,pa,x,y,atoi(t->left->node));
			  t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0,eqpath(x,k));
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,t2);
		case 'U': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=fkx(t->right,k);
			  t1=actl2f1a(t->left,k,pn,pa,x,y); 
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  t1=actl2f1a(t->left,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0, 
				treecompose("|",t1,eqpath(x,k)));
			  pa+=fkx(t->left,k);
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=actl2f1a(t->left,k,pn,pa,x,y); 
			  t1=actl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("&",t0,t1);
		case '|': t0=actl2f1a(t->left,k,pn,pa,x,y); 
			  pa+=fkx(t->left,k); /*201503zwh*/
			  t1=actl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  
			if (k==0||x==0) x=1;
			return treeaddnumber(treecopy(t),(x-1)*pn*(k+1)+y*pn);
	}
}
/*******************************************************************/
tree *actl2f0a(tree *t,int k,int pn,int pa,int x,int y)
{
	tree *t0,*t1,*t2; 

	switch (t->node[0]) {
		case 'E': t1=actl2f0a(t->right,k,pn,pa+1,pa+1,0);
			  if (x==0) return t1;
			  t0=eqstate(x,y,pa+1,0,k);
			  return treecompose(":",t0,t1);
		case 'X': if (y==k) return treeconst(0);
			  return t1=actl2f0a(t->right,k,pn,pa,x,y+1);
		case 'F': t0=actl2f0a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  t2=actl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,t2);
		case 'G': if (t->left!=NULL) 
			  return makeform_ctl(actl2f0a,
				t->right,k,pn,pa,x,y,atoi(t->left->node));
			  t0=actl2f0a(t->right,k,pn,pa,x,y); 
			  pa+=gkx(t->right,k);
			  if (y==k) return treecompose("&",t0,eqpath(x,k));
			  t2=actl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,t2);
		case 'U': t0=actl2f0a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  t1=actl2f0a(t->left,k,pn,pa,x,y); 
			  pa+=gkx(t->left,k);
			  t2=actl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=actl2f0a(t->right,k,pn,pa,x,y); 
			  pa+=gkx(t->right,k);
			  t1=actl2f0a(t->left,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0,
				treecompose("|",t1,eqpath(x,k)));
			  t2=actl2f0a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=actl2f0a(t->left,k,pn,pa,x,y); 
			  pa+=fkx(t->left,k); /*201503zwh*/
			  t1=actl2f0a(t->right,k,pn,pa,x,y); 
			  return treecompose("&",t0,t1);
		case '|': t0=actl2f0a(t->left,k,pn,pa,x,y); 
			  t1=actl2f0a(t->right,k,pn,pa,x,y); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  
			if (k==0||x==0) x=1;
			return treeaddnumber(treecopy(t),(x-1)*pn*(k+1)+y*pn);
	}
}
/*******************************************************************/
int fkx(tree *t,int k)
{
        int x,y,z;
        if (t==NULL|k==0) return 0;
        switch (t->node[0]) {
                case '&': x=fkx(t->left,k); y=fkx(t->right,k); return x>y?x:y;
                case '|': x=fkx(t->left,k); y=fkx(t->right,k); return x+y;
                case 'A': return fkx(t->right,k)+1;
                case 'X': return fkx(t->right,k);
                case 'F': return (k+1)*fkx(t->right,k);
                case 'G': return fkx(t->right,k);
                case 'U': x=fkx(t->left,k); y=fkx(t->right,k);
                          z=(x>y?x:y); return k*y+z;
                case 'R': x=fkx(t->left,k); y=fkx(t->right,k);
                          z=(x>y?x:y); return k*x+z;
		default: return 0;
        }
}
/*******************************************************************/
int gkx(tree *t,int k)
{       
        int x,y,z;
        if (t==NULL|k==0) return 0;
        switch (t->node[0]) {
                case '&': x=gkx(t->left,k); y=gkx(t->right,k); return x+y;
                case '|': x=gkx(t->left,k); y=gkx(t->right,k); return x>y?x:y;
                case 'E': return gkx(t->right,k)+1;
                case 'X': return gkx(t->right,k);
                case 'F': return gkx(t->right,k);
                case 'G': return (k+1)*gkx(t->right,k);
                case 'U': x=gkx(t->left,k); y=gkx(t->right,k); return k*x+y;
                case 'R': x=gkx(t->left,k); y=gkx(t->right,k); return (k+1)*y+x;
		default: return 0;
        }
}
/*******************************************************************/
/*
tree *actl2f1aOO(tree *t,int k,int pn,int pa,int x,int y)
{
	tree *t0,*t1,*t2; 

	switch (t->node[0]) {
		case 'A': t1=actl2f1a(t->right,k,pn,pa+1,pa+1,0);
			  if (x==0) return t1;
			  t0=eqstate(x,y,pa+1,0,k);
			  return treecompose(":",t0,t1);
		case 'X': if (y==k) return treeconst(0);
			  return t1=actl2f1a(t->right,k,pn,pa,x,y+1);
		case 'F': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=fkx(t->right,k);
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,t2);
		case 'G': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0,eqpath(x,k));
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,t2);
		case 'U': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  if (y==k) return t0;
			  pa+=fkx(t->right,k);
			  t1=actl2f1a(t->left,k,pn,pa,x,y); 
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=actl2f1a(t->right,k,pn,pa,x,y); 
			  t1=actl2f1a(t->left,k,pn,pa,x,y); 
			  if (y==k) return treecompose("&",t0, 
				treecompose("|",t1,eqpath(x,k)));
			  pa+=fkx(t->left,k);
			  t2=actl2f1a(t,k,pn,pa,x,y+1); 
			  return treecompose("&",t0,treecompose("|",t1,t2));
		case '&': t0=actl2f1a(t->left,k,pn,pa,x,y); 
			  t1=actl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("&",t0,t1);
		case '|': t0=actl2f1a(t->left,k,pn,pa,x,y); 
			  t1=actl2f1a(t->right,k,pn,pa,x,y); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  return treeaddnumber(treecopy(t),(x-1)*pn*(k+1)+y*pn);
	}
}
/*******************************************************************/
