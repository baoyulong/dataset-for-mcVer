#include "bp2s.h"
#include "tlf.h"
/*******************************************************************/
#define bpi 	bp->right->left
#define bpt 	bp->right->right->left
/*******************************************************************/
#define spectrue()      treecompose("|",neg(treenum(1)),treenum(1))
#define specfalse()     treecompose("&",neg(treenum(1)),treenum(1))
/*******************************************************************/
model *bp2s(tree *bp,tree *vconstr,tree *bs,int booln,int k,int bv,int opt)
{
	model *mo,*mo2;

        mo=bp2s0(bp,bs,booln,k,bv,opt); 
	if (vconstr!=NULL) mo=bp2sADDconstr(mo,bp,vconstr,booln,k);
	return mo;
}
/*******************************************************************/
model *bp2s0(tree *bp,tree *bs,int booln,int k,int bv,int opt)
{
        int tp=TLFnnftype(bs);

        if (tp&2) { error("bp2s specificaiton problem"); }
        if ((tp&1)==0) return bp2sLTL(bp,bs,booln,k,bv,opt); 
	return bp2sCTL(bp,bs,booln,k,bv,opt); 
}
/*****************************************************************/
model *bp2sLTL(tree *bp,tree *bs,int booln,int k,int bv,int opt)
{
	model *mo,*mo2;
	tree *t0=TLFnegate(treecopy(bs));
	tree *t1;
	int npath=1;

	t0=TLFsimplifyLTL(t0); /* R-->U, some problem with R*/
/*
 treewrite(t0,stdout); infoi(100);
*/
	if (bv) t1=SPECltl2f1(t0,k,0,booln); 
		else t1=SPECltl2f0(t0,k,-1,booln,bpt,bv);
        t1=treedopost(t1,TLFnnfsimp);
        if (t1->node[0]==TRUE) treereplace(t1,spectrue());
        if (t1->node[0]==FALSE) treereplace(t1,specfalse());

        mo=CNFsys2model(bpi,bpt,booln,k,npath);
        mo2=CNFspec2model(t1,mo->n); 
        mo=CNFconcatmodel(mo,mo2);

	treefree(t0); treefree(t1); return mo;
}
model *bp2sCTL(tree *bp,tree *bs,int booln,int k,int bv,int opt)
{
	model *mo,*mo2;
	tree *t0=treecopy(bs);
	tree *t1;
	int npath=SPECnpath(bs,bv,k,opt);


	switch (bv) {
	    case 0: t0=TLFnegate(t0);
		    if (opt&32) t1=SPECactl2f0CTLR(t0,k,booln);
		    else if (opt&64) t1=SPECactl2f0LIN(t0,k,booln,bpt);
       		    else t1=SPECactl2f0(t0,k,booln);
		    eqstateconstruct(t1,booln); treedopost(t1,SPECreplcolon);
		    break;
	    case 1: if (opt&16) t1=SPECactl2f1CTLR(t0,k,booln);
                    else t1=SPECactl2f1(t0,k,booln);
		    eqstateconstruct(t1,booln); treedopost(t1,SPECreplcolonneg);
		    t1=TLFnegate(t1);
		    break;
	}

        t1=treedopost(t1,TLFnnfsimp);
        if (t1->node[0]==TRUE) treereplace(t1,spectrue());
        if (t1->node[0]==FALSE) treereplace(t1,specfalse());

        mo=CNFsys2model(bpi,bpt,booln,k,npath);
        mo2=CNFspec2model(t1,mo->n); 
        mo=CNFconcatmodel(mo,mo2);

	treefree(t0); treefree(t1); return mo;
}

/*****************************************************************/
model *bp2sADDconstr(model *mo,tree *bp,tree *varconstr,int booln,int k)
{
	model *mo2;
	tree *ts1,*ts2;

 return mo; 
/* no add for testing zwh*/

        ts1=treecompose("F",NULL,TLFnegate(treecopy(varconstr)));
        ts2=bpLTLspec2s(bp,ts1,booln,k,1,0);

        mo2=CNFspec2model(ts2,mo->n);
        mo=CNFconcatmodel(mo,mo2);

        treefree(ts1); treefree(ts2); return mo;
}
/*****************************************************************/
tree *bpLTLspec2s(tree *bp,tree *bs,int booln,int k,int bv,int opt)
{
	tree *t0=TLFnegate(treecopy(bs));
	tree *t1;

	if (bv) t1=SPECltl2f1(t0,k,0,booln); 
		else t1=SPECltl2f0(t0,k,-1,booln,bpt,bv); /*bv=1? not needed*/
        t1=treedopost(t1,TLFnnfsimp);
        if (t1->node[0]==TRUE) treereplace(t1,spectrue());
        if (t1->node[0]==FALSE) treereplace(t1,specfalse());

	treefree(t0); return t1;
}
