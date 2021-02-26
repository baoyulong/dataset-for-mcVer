#include "bp2s.h"
#include "tlf.h"
/*******************************************************************/
tree *SPECreplcolon(tree *t)
{
	if (t->node[0]==':') { t->node[0]='&'; } 
	return t;
}
tree *SPECreplcolonneg(tree *t)
{
	if (t->node[0]==':') { t->node[0]='|'; t->left=TLFnegate(t->left); } 
	return t;
}
/*******************************************************************/
int SPECnpath(tree *spec,int bv,int k,int ctl_opt)
{
        int npath;
        tree *ts;

        if (bv) { npath=fkx(spec,k); }
        else {
                ts=TLFnegate(treecopy(spec));
                if (ctl_opt&32) npath=CTLRgkx(ts,k); else npath=gkx(ts,k);
                treefree(ts);
        }

        if (k==0||npath==0) npath=1;

        return npath;
}
/*******************************************************************/
