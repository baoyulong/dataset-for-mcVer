#include "bp2s.h"
/*******************************************************************/
#define myprints(f,s,m)   	{ m[++*m]=s; }
#define myprints0(f,s0,m) 	{ if (s0) m[++*m]=-s0; m[++*m]=0; }
/*******************************************************************/
int CNFnnf2cnf_1(tree *t,char c,int s0,int s,int *mstring)
{
	int i;
	if (t==NULL) return s;
	if (t->node[0]==c) { 
		i=CNFnnf2cnf_1(t->left,c,s0,s,mstring); 
		i=CNFnnf2cnf_1(t->right,c,s0,i,mstring); return i;
	} 
        if (t->node[0]=='!') {
                myprints(stdout,-atoi(t->right->node),mstring);
                if (c=='&') myprints0(stdout,s0,mstring);
        } else if (isdigit(t->node[0])) {
                myprints(stdout,atoi(t->node),mstring);
                if (c=='&') myprints0(stdout,s0,mstring);
        } else if (c=='&') {
                sprintf(t->node+1,"%i",s0);
        } else /*if (c=='|')*/ {
                myprints(stdout,++s,mstring); sprintf(t->node+1,"%i",s);
                if (c=='&') myprints0(stdout,s0,mstring);
        }
        return s;
}
int CNFnnf2cnf_2(tree *t,char c,int s,int *mstring)
{
	int i;
	if (t==NULL) return s;
	if (t->node[0]==c) { 
		i=CNFnnf2cnf_2(t->left,c,s,mstring); 
		i=CNFnnf2cnf_2(t->right,c,i,mstring); return i;
	} 
	if (t->node[0]=='!') return s; 
	else if (isdigit(t->node[0])) return s;
	else return CNFnnf2cnf(t,t->node[0],atoi(t->node+1),s,mstring);
}
int CNFnnf2cnf(tree *t,char c,int s0,int auxp,int *mstring)
{
	int i;

	if (t==NULL) return auxp;

	if (c==0) {
		if (t->node[0]=='!'||isdigit(t->node[0]))
			treereplace(t,treecompose("&",treecopy(t),treecopy(t)));
		c=t->node[0];
	}
	i=CNFnnf2cnf_1(t,c,s0,auxp,mstring); 
	if (c=='|') myprints0(stdout,s0,mstring); 
	i=CNFnnf2cnf_2(t,c,i,mstring); 
	return i;
}
/*******************************************************************/
int *CNFrotate(int *s1,int *s2,int n,int a1,int a2,int b1,int b2,int bn,int au)
{
        int j,s;
        int a1a,a1b,a2a,a2b,b1a,b2a;

        a1a=(a1-1)*bn+1; a1b=a1*bn;
        a2a=(a2-1)*bn+1; a2b=a2*bn;
        b1a=(b1-1)*bn+1-a1a; b2a=(b2-1)*bn+1-a2a;

        for (j=0;j<n;j++)
        if (s1[j]==0) s2[j]=0; else {
            if (s1[j]>0) s=1; else s=-1;
            if (a1a<=s*s1[j]&&s*s1[j]<=a1b) s2[j]=s1[j]+b1a*s;
            else if (a2a<=s*s1[j]&&s*s1[j]<=a2b) s2[j]=s1[j]+b2a*s;
            else s2[j]=s1[j]+au*s;
        }
	return s2;
}
/*******************************************************************/
