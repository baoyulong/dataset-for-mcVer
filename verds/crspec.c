#include "bp2s.h"
/*******************************************************************/
#define ptrans  ws2->prog->right->right->left
/*******************************************************************/
tree *crspec(workspace *ws2,int pp)
{
	tree *spec=NULL;

	spec=listget1n(ws2->prog->right->right->right,pp);
	spec=treecopy(spec);
        if (pp==-1) spec=treeconst(1);
        if (pp==-2) spec=agTRUE();
        if (pp==-3) spec=agagTRUE();
        if (pp==-4) spec=afagTRUE();
        if (pp==-5) spec=afFALSE();
	if (spec==NULL) spec=agCompose(treecopy(ws2->deadlockfreecond));

	return spec;
}
/*******************************************************************/
tree *agTRUE()
{
        tree *t=treeconst(1); /*treecompose("|",neg(treenum(1)),treenum(1));*/
        return treecompose("A",NULL,treecompose("G",NULL,t));
}
tree *agagTRUE()
{
        tree *t=agTRUE();
        return treecompose("A",NULL,treecompose("G",NULL,t));
}
tree *afagTRUE()
{
        tree *t=agTRUE();
        return treecompose("A",NULL,treecompose("F",NULL,t));
}
tree *afFALSE()
{
        return treecompose("A",NULL,treecompose("F",NULL,treeconst(0)));
}
tree *agCompose(tree *prop)
{
        tree *t=treecompose("G",NULL,prop);
        return treecompose("A",NULL,t);
}
