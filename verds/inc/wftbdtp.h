int TBDtreelength(tree *trs,char *del);
int TBDlistlength(tree *t1);
tree *TBDlistgetone(tree *t,int next);
int main(int argc, char *argv[]);
extern TPtype *ggtp;
int TPavar(char **v,int n);
int TPatrs(tree *tr);
TBD **TPafair(tree *fn);
TBD *TPainit(tree *init);
TBD *TPfml(tree *p);
int TPtrs(tree *trs,int n,int fbnb);
int TPtrs1(TPtype *j,tree *p,TBD **tr,TRtbd **tr2,int i,char *or);
int TPcheckp(tree *tt,int n,int fw);
int TPtrsfw2full();
void TPvar(int bw);
void TPsv1(char **a,int *b,int l,int r) /* increasing */;
int TPsnm(char *s,char **a,int *b,int l,int r);
TBD *TPtbdvar(char *var);
