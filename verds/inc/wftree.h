int listlength(tree *t1);
tree *listget1n(tree *t,int n);
tree *listgetone2(tree *t,int next) ;
tree *listgetone(tree *t,int next) ;
tree *listmerge(tree *t0,tree *t1);
tree *listcombone(tree *f(),tree *l1,tree *l2);
tree *listcombine(tree *f(),tree *x1,tree *x2);
tree *listcombinedist(tree *f(),tree *x1,tree *x2);
void listtraverse(tree *ts,void f());
tree *listaddstring(char *s,tree *v0);
int listrmstring(char *s,tree *v0);
int listsearchstring(char *s,tree *v0);
int istxtprocedure(char *s);
tree *eqstate(int pa,int i,int pb,int j,int k);
void eqstateconstruct(tree *t,int bn);
void stringcomments(char *s);
void stringcompress(char *s,char a,char b);
char *stringed(char *s,char *a,char *b);
char *stringsearch(char *s,char *a,char *b);
void *stringreplacechar(char *s,char a,char b);
int stringreplacestring(char *s,char *a,char *b);
int nodereplace(tree *t,tree *a,tree *b);
int stringreplacestringfixlength(char *s,char *a,char *b);
void treereplacestring(tree *t,char *a,char *b);
void stringreplacesubstring(char *s,char *a,char *b);
int ran(char *s,int left);
tree *string2tree_local(char *s,char *op);
tree *string2tree(char *s);
int treesize(tree *t0) ;
int treesizeof(char nd,tree *t0) ;
tree *treesearchvarlist(char *s,int n,tree *t);
int treesearchvarlistnum(char *s,int n,tree *t);
tree *treesearchstringlist(char *s,int n,tree *t);
tree *treesearchvarall(char *node,int n,tree *t);
tree *treedopost(tree *t,tree *f());
void treedopostvoid(tree *t,void f());
tree *treedopre(tree *t,int f0());
void treedoprevoid(tree *t,int f0());
void treefree(tree *t) ;
tree *treecopy(tree *t) ;
void treereplace(tree *t0,tree *t1) ;
tree *treecompose(char *s,tree *t0,tree *t1) ;
tree *treeconst(int positive);
tree *treeconstab(int positive,int i);
tree *treenum(int n);
tree *treestring(char *s);
tree *treevarconstr(char *s,int low,int high);
tree *treesearch(char *s,int n,tree *t);
tree *treetr(tree *tr,int a,int b,int pn);
tree *treeaddnumber(tree *t,int i);
tree *treeaddnumberconditional(tree *t,int i,int c);
int intlen(int *aa);
int treediff(tree *s,tree *t);
float utiltime(int finish);
int fileok(char *s);
char *file2string(char *fn);
int arrayindex(char *node);
int pararrayindex(char *node);
int marrayindex(char *node);
int chanindex(char *node);
char charsearch(char a,char* s);
void quicksort_ab(int *a,int *b,int l,int r ) /* decreasing */;
void gfilename(char *s,char *ss);
void treew(tree *t);
void treewrite(tree *t,FILE *f);
void stringwrite(char *ss,int a,int b,FILE *g);
void modulewrite(module *mm,FILE *f);
int treewrites(tree *t,char *f,int i);
void inttablewrite(int **tt,int mrows,int ncols,FILE *ff);
void prgwrite(prg *pr1,FILE *f);
void ggbwrite(ggbase *ggb,FILE *f);
int treewritep(tree *t,char *f,int i,int j);
void intstringwrite(int *a);
