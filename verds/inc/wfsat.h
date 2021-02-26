int SATapply(model *mm);
int startbranching(mstruct *cc,mdata *wc,model *mm,int *ccCL);
int branching(mstruct *cc,mdata *wc,int *cl,int m,int n);
void backtrack(mdata *wc,int *lcl,int *cur);
float lvalue(mstruct *cc,int *cl,int literal);
void lvalue_n(mstruct *cc,int *cl,int literal,char *gim);
int LUcount;;
extern model *gmm;;
int lookuplist(mstruct *cc,int *cl,char *gim,int *clist);
int lookup(mstruct *cc,int *clen,char *gim,int c);
int LUunitp(mstruct *cc,char *gim,int *cl,int *cur);
extern model *gmm;;
int select_v(mstruct *cc,int *cl,mdata *wc,int n);
int select_v0(mstruct *cc,int *cl,mdata *wc,int n);
int select_v1(mstruct *cc,int *cl,mdata *wc,int n);
char *file2string(char *fn);
