void imp2start(mstruct *cc,int mmn);;
int imp2branchstart(int c);;
int imp2branchskip(int c);;
int imp2branchend();;
int imp2close(int mmn);;
int impclauseadd(int *aa,int *aa1);;
int impclauseuse1(char *gim,int *cure);;
extern model *gmm;;
mstruct *imp2build(int **impclause,int mmm,int mmn);
void imp2start(mstruct *cc,int mmn);
int imp2close(int mmn);
int imp2branchstart(int c);
int imp2branchskip(int c) ;
int imp2branchend();
int impclauseadd(int *aa,int *aa1);
int impclauserm(int i);
int impclauseuse1(char *gim,int *cure);
int impclauseuse(char *gim,int c1,int *cure);
int impclausereduce(mstruct *cc,int *clause,int n);
int impres0(mstruct *cc,int *clause,int n);
int impres1(mstruct *cc,int *clause,int n);
int unitpres(mstruct *cc,char *gim,int *cl,int *cur);
int intlevel(int *aa);
int intinneg(int a,int *bb);
int intsetincludeneg(int *aa,int *bb);
int intinnegremove(int a,int *bb);
int intinpos(int a,int *bb);
int imp_res_simple(int *aa,int *bb);
