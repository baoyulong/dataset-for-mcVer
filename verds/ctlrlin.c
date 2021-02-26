#include "bp2s.h"
#include "tlf.h"
/*******************************************************************/
#define max(a,b)	(a>b?a:b)
#define ORG		/*zwh20160111*/
/*******************************************************************/
tree *SPECactl2f0LIN(tree *t,int k,int pn,tree *tr)
{
	tree *t0,*t1;

#ifdef ORG 
	t1=LINactl2f0a(t0,k,pn,0,0,0,tr); 
#else
	t0=TLFsimplifyLTL(treecopy(t)); 
			/* this is for CTL, R-->U, some problem with R*/
	t1=LINactl2f0a(t0,k,pn,0,0,0,tr); 
	treefree(t0); 
#endif
	return t1;
}
/*******************************************************************/
tree *LINactl2f0a(tree *t,int k,int pn,int pa,int x,int y,tree *tr)
{
	int pak;
	tree *t0,*t1,*t2,*t2a; 
	switch (t->node[0]) {
		case 'E': if (k>0&&isLIN(t)&&(depLIN(t))>1) {
/*
 infos("LIN"); infoi(pak);
*/
				pak=kdepLIN(t,k);
				t1=SPEClin2f0(t,k,-1,pn,pa+1,pak,tr);
			  } else {
				t1=LINactl2f0a(t->right,k,pn,pa+1,pa+1,0,tr);
			  }
			  if (x==0) return t1;
			  t0=eqstate(x,y,pa+1,0,k);
			  return treecompose(":",t0,t1);
		case 'X': if (y==k) return treeconst(0);
			  return t1=LINactl2f0a(t->right,k,pn,pa,x,y+1,tr);
		case 'F': t0=LINactl2f0a(t->right,k,pn,pa,x,y,tr); 
			  if (y==k) return t0;
			  t2=LINactl2f0a(t,k,pn,pa,x,y+1,tr); 
			  return treecompose("|",t0,t2);
		case 'G': if (t->left!=NULL) 
			  return makeform_LIN(LINactl2f0a,
				t->right,k,pn,pa,x,y,atoi(t->left->node),tr);
			  if (k==0) return treeconst(0);
			  t0=LINactl2f0a(t->right,k,pn,pa,x,y,tr); 
/*
			  if (y==k-1) return treecompose("&",t0,eqpath(x,k));
*/
			  if (y==k-1) 
			  return treecompose("&",t0,eqpathj(x,k,y+1));
			  pa+=gkxLIN(t->right,k);
			  t2=LINactl2f0a(t,k,pn,pa,x,y+1,tr); 
/*
			  if (y<k-1) t2=treecompose("|",eqpathj(x,k,y+1),t2);
*/
			  return treecompose("&",t0,t2);
		case 'U': t0=LINactl2f0a(t->right,k,pn,pa,x,y,tr); 
			  if (y==k) return t0;
			  t1=LINactl2f0a(t->left,k,pn,pa,x,y,tr); 
			  pa+=gkxLIN(t->left,k);
			  t2=LINactl2f0a(t,k,pn,pa,x,y+1,tr); 
			  return treecompose("|",t0,treecompose("&",t1,t2));
		case 'R': t0=LINactl2f0a(t->right,k,pn,pa,x,y,tr); 
			  pa+=gkxLIN(t->right,k);
			  t1=LINactl2f0a(t->left,k,pn,pa,x,y,tr); 
		/*below,zwh20160111*/
			  t2a=eqpathj(x,k,y); /*in fact: meaning 0 upto y*/
			  if (y==k) 
			  return treecompose("&",t0,treecompose("|",t2a,t1));

			  t2=LINactl2f0a(t,k,pn,pa,x,y+1,tr); 
			  return treecompose("&",t0,
				treecompose("|",t2a,treecompose("|",t1,t2)));
/*
			  if (y==k) return treecompose("&",t0,t1);
			  t2=LINactl2f0a(t,k,pn,pa,x,y+1,tr); 
			  if (y<=k-1) t2=treecompose("|",eqpathj(x,k,y+1),t2);
			  return treecompose("&",t0,treecompose("|",t1,t2));
*/
		case '&': t0=LINactl2f0a(t->left,k,pn,pa,x,y,tr); 
			  pa+=fkx(t->left,k); /*201503zwh*/
			  t1=LINactl2f0a(t->right,k,pn,pa,x,y,tr); 
			  return treecompose("&",t0,t1);
		case '|': t0=LINactl2f0a(t->left,k,pn,pa,x,y,tr); 
			  t1=LINactl2f0a(t->right,k,pn,pa,x,y,tr); 
			  return treecompose("|",t0,t1);
		case '!': 
		default:  
			if (k==0||x==0) x=1;
			return treeaddnumber(treecopy(t),(x-1)*pn*(k+1)+y*pn);
	}
}
/*******************************************************************/
int gkxLIN(tree *t,int k)
{       
        int x,y,z;
        if (t==NULL) return 0;
	if (isLIN(t)) return depLIN(t);
        switch (t->node[0]) {
                case '&': return gkxLIN(t->left,k)+gkxLIN(t->right,k); 
                case '|': return max(gkxLIN(t->left,k),gkxLIN(t->right,k)); 
                case 'E': return gkxLIN(t->right,k)+1;
                case 'X': return gkxLIN(t->right,k);
                case 'F': return gkxLIN(t->right,k);
                case 'G': return (k)*gkxLIN(t->right,k);
                case 'U': return (k)*gkxLIN(t->left,k)+gkxLIN(t->right,k); 
                case 'R': gkxLIN(t->left,k)+(k+1)*gkxLIN(t->right,k); 
		default: return 0;
        }
}
/*******************************************************************/
int isLINu(tree *t)
{       
	int r=0;

/*
	if (isProp(t)) return 1;
*/
        switch (t->node[0]) {
                case '&': r=(isProp(t->left)&&isLINu(t->right))||
                	    (isProp(t->right)&&isLINu(t->left)); 
			  break;
                case 'E': switch (t->right->node[0]) {
                		case 'G': r=isLINu(t->right->right)|| 
                			    isProp(t->right->right); 
					  break;
                		case 'R': r=(isProp(t->right->left)&& 
					     isLINu(t->right->right))||
                	  		    (isProp(t->right->right)&&
					     isLIN(t->right->left));
					  break;
				default:  break;
			}
	}
	return r;
}
/*******************************************************************/
int isLIN(tree *t)
{       
	int r=0;

        if (t==NULL) return 0;
	if (isProp(t)||isLINu(t)) return 1;

        switch (t->node[0]) {
                case '|': r=isLIN(t->left)&&isLIN(t->right); break;
                case '&': r=(isProp(t->left)&&isLIN(t->right))||
                	    (isProp(t->right)&&isLIN(t->left)); 
			  break;
                case 'E': switch (t->right->node[0]) {
                		case 'X': 
                		case 'F': r=isLIN(t->right->right); break;
                		case 'U': r=(isProp(t->right->left)&&
					     isLIN(t->right->right)); 
			  		   break;
				default:  break;
			}
        }
	return r;
}
/*******************************************************************/
int isProp(tree *t)
{       
	int p=0;

        if (t==NULL) return 1;
        switch (t->node[0]) {
                case '|': 
		case '&': p= isProp(t->left)&&isProp(t->right); break;
                case 'X': 
		case 'G': 
		case 'F': 
		case 'R': 
		case 'U': 
                case 'A': 
		case 'E': p=0; break;
		default:  p=1; break;
        }
/*
 infoi(13); treewrite(t,stdout); infoi(13); infoi(p);
*/
	return p;
}
/*******************************************************************/
int depLIN(tree *t)
{       
        if (t==NULL) return 0;
	if (isProp(t)) return 0;
        switch (t->node[0]) {
                case '&': 
                case '|': return max(depLIN(t->left),depLIN(t->right)); 
                case 'A': 
                case 'E': switch (t->right->node[0]) {
                		case 'F': 
                		case 'G': 
                		case 'X': return 1+depLIN(t->right->right); 
                		case 'U': 
                		case 'R': return 1+max(
						depLIN(t->right->left), 
						depLIN(t->right->right)); 
			}
	}
	error("depLIN");
}
/*******************************************************************/
tree *makeform_LIN(tree *f(),tree *t,int k,int pn,int pa,int x,int y,int j,tree *tr)
{
        int i;
        tree *t0;

        if (k<j) return treeconst(0);
        t0=f(t,k,pn,pa,x,y,tr);
        for (i=1;i<=j;i++) t0=treecompose("&",f(t,k,pn,pa,x,y+i),t0);
        return t0;
}
/*******************************************************************/
int kdepLIN(tree *t,int k)
{       
        if (t==NULL) return 0;
	if (isProp(t)) return 0;
        switch (t->node[0]) {
                case '&': 
                case '|': return max(kdepLIN(t->left,k),kdepLIN(t->right,k)); 
                case 'A': 
                case 'E': switch (t->right->node[0]) {
                		case 'X': return 1+kdepLIN(t->right->right,k); 
                		case 'F': 
                		case 'G': return k+kdepLIN(t->right->right,k); 
                		case 'U': 
                		case 'R': return k+max(
						kdepLIN(t->right->left,k), 
						kdepLIN(t->right->right,k)); 
			}
	}
	error("depLIN");
}
/*******************************************************************/
