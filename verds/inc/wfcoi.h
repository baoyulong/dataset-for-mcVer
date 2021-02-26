int DEPanalysis(tree *prog,int pp);
void DEPadd(tree *t0,tree *tl,tree *vlist,int **DEPtable,int **DEPctltable);
void DEPadddep(tree *t0l,tree *t0r,tree *tl,tree *vlist,int **DEPtable,int **DEPctltable);
void DEPaddnum(int *ilist,tree *t1,tree *vlist);
void DEPsort(int *ord,int *w,int nvar);
tree *DEPreorder(int *ord,int nvar,tree *vlist);
int *DEPcalcweight(int *w,int nvar,int **DEPtable,int **DEPctltable);
void DEPrefine(int *ord,int *w,int nvar,int **DEPtable,int **DEPctltable);
int dweight(int i,int j,int *ord,int **DEPtable,int **DEPctltable);
tree *listitem(int i,tree *vlist);
int DEPvarlist(tree *prog,tree *spec,int **tb,int **ctltb,int nvar,int *vlist) ;
int DEPvar(int *a,int **tb,int **ctltb,int nvar);
int DEPvarcore(tree *t0,int *a,tree *vlist);
int intadd2list(int k,int *ll);
int DEPsimp(tree *t0,int *varlist);
tree *DEPsimplify1(tree *l0,int i);
tree *DEPsimplify2(tree *t0,tree *vl);
tree *DEPsimplify3(tree *l0,tree *vl);
int DEPinlist(int i) ;
int DEPsomeninlist(tree *t0,tree *vl) ;
tree *DEPsimptrleft(tree *t0,tree *vl) ;
extern char *PAR_satsolver;;
void var_order(prg *pr,char *varfile);
void var_order_vn(char *l1,char *l2,tree *vlist_old);
int var_order_process(char *l1,char *l2);
