#include "bp2s.h"
#include "tlf.h"
/*******************************************************************/
model *CNFnonrepetition(tree *init,tree *trans,int bn,int k,int auxp)
{
	int *tmstring,*tm2string,*mm,*m1;
	int i,j,n,a,s,q,np;
	int aux1,aux0;
	model *mo;
	tree *tr;

	mo=(model *)malloc(sizeof(model)); 
	mo->mstring=(int *)malloc(sizeof(int)*MAXSTRING); 
	mo->n=mo->m=mo->l=0; 

	tmstring=(int *)malloc(sizeof(int)*MAXSTRING); *tmstring=0;
	tm2string=(int *)malloc(sizeof(int)*MAXSTRING); 

	tr=TLFnegate(treecopy(init)); tr=treedopost(tr,TLFnnf);
	aux1=auxp; auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); treefree(tr);
	aux0=auxp-aux1;
	for (n=0,i=2;i<=k+1;i++,n++) {
		CNFrotate(tmstring+1,tm2string+*tmstring*n,
			*tmstring,1,1,i,i,bn,n*aux0);
	}
	auxp=aux1+n*aux0;
	CNFaddstring(tm2string,*tmstring*n,mo);

	*tmstring=0; 
	tr=TLFnegate(treecopy(trans)); tr=treedopost(tr,TLFnnf); 
	treedoprevoid(tr,TLFlcnf);
	aux1=auxp; auxp=CNFnnf2cnf(tr,0,0,auxp,tmstring); treefree(tr);
	aux0=auxp-aux1;

	for (n=0,i=1;i<=k+1;i++) {
		for (j=i+2;j<=k+1;j++,n++) {
			CNFrotate(tmstring+1,tm2string+*tmstring*n,
				*tmstring,1,2, i,j,bn,n*aux0);
		}
	}
	auxp=aux1+n*aux0;
	CNFaddstring(tm2string,*tmstring*n,mo);

	free(tmstring); free(tm2string);
	mo->n=auxp; mo->mstring=(int *)realloc(mo->mstring,sizeof(int)*(mo->l));
	return mo;
}
