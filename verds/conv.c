#include "bp2s.h"
/*******************************************************************/
model *CNFsys2model(tree *init,tree *trans,int bn,int k,int npath)
{
	int *tmstring,*mm,*m1;
	int i,j,n,a,s,q,np;
	int auxp;
	model *mo;
	tree *tr;

	mo=(model *)malloc(sizeof(model)); 
	mo->mstring=(int *)malloc(sizeof(int)*MAXSTRING); 
	mo->n=mo->m=mo->l=0; 

	tmstring=(int *)malloc(sizeof(int)*MAXSTRING); 
	auxp=bn*(k+1)*npath;

	tr=treecopy(init);
	*tmstring=0; auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); treefree(tr);
	CNFaddstring(tmstring+1,*tmstring,mo);

	if (k==0) {
		free(tmstring);
		mo->n=auxp;
		mo->mstring=(int *)realloc(mo->mstring,sizeof(int)*(mo->l));
		return mo;
	}

	tr=treecopy(trans);
	*tmstring=0; auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); treefree(tr);
	/* cnfwrite(tmstring+1,*tmstring,stdout); nl; */

	n=*tmstring*k*npath; /*tmstring=realloc(tmstring,sizeof(int)*n+1);*/
	mm=tmstring+1+*tmstring;

	q=auxp-bn*(k+1)*npath; a=2*bn; 
	for (np=0;np<npath;np++)
	for (i=0;i<k;i++) {
		if (np==0&&i==0) continue;
		for (j=1;j<=*tmstring;j++) 
		if (tmstring[j]==0) *mm++=0; else {
		    if (tmstring[j]>0) s=1; else s=-1; 
		    if (s*tmstring[j]<=a) *mm++=tmstring[j]+(i+(np)*(k+1))*bn*s;
		    else *mm++=tmstring[j]+(i+(np)*(k))*q*s; 
		}
	}
	auxp=auxp+q*(k-1)+q*(npath-1)*k;
	CNFaddstring(tmstring+1,n,mo);
/*
	tr=treecopy(spec);
	*tmstring=0; auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); treefree(tr);
	CNFaddstring(tmstring+1,*tmstring,mo);
*/
	free(tmstring);
	mo->n=auxp;
	mo->mstring=(int *)realloc(mo->mstring,sizeof(int)*(mo->l));

	return mo;
}
model *CNFspec2model(tree *spec,int auxp)
{
	int *tmstring;
	model *mo;
	tree *tr;

	mo=(model *)malloc(sizeof(model)); 
	mo->mstring=(int *)malloc(sizeof(int)*MAXSTRING); 
	mo->n=mo->m=mo->l=0; 

	tmstring=(int *)malloc(sizeof(int)*MAXSTRING); *tmstring=0; 
	tr=treecopy(spec); auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); 
	treefree(tr);
	CNFaddstring(tmstring+1,*tmstring,mo); free(tmstring);

	mo->n=auxp;
	mo->mstring=(int *)realloc(mo->mstring,sizeof(int)*(mo->l));
	return mo;
}
model *CNFconcatmodel(model *m1,model *m2)
{
	int i;
	int *p1,*p2;
	int *mstring;

	m1->mstring=(int *)realloc(m1->mstring,(m1->l+m2->l)*sizeof(int)); 
	m1->n=(m2->n>m1->n?m2->n:m1->n); m1->m+=m2->m; 

	p1=m1->mstring+m1->l; 
	for (p2=m2->mstring,i=0;i<m2->l;i++) { *p1++=*p2++; } m1->l+=m2->l; 
	free(m2->mstring); free(m2);

	return m1;
}
/*******************************************************************/
void CNFaddstring(int *mstring,int mstringc,model *mo)
{
        int i;
        int *mm=mo->mstring+mo->l;

        if ((mo->l+=mstringc)>MAXSTRING) error("MAXSTRING");
        for (i=0;i<mstringc;i++) { *mm++=mstring[i]; if (!mstring[i]) mo->m++; }
}
/*******************************************************************/
