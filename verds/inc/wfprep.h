char *PREPfile2txt(char *fn);
void PREPreplaceSTEP(char *txt);
void PREPreplaceASSEQ(char *txt);
char *PREPmatch0(char *t0);
char *PREPmatch1(char *t0);
tree *PREPcreatevlist(char *t0,char *t1);
void PREPwritevlist(tree *tt,char *t0,char *t1,char *t2);
char *PREPreplaceENUM(char *s);
void repinit();
void repclose();
int repgetw(char *s,int cc1,int i);
int repgetdef(char *s,int cc1);
void stringkstring(char *s,char **a,char **b,int *c,int k,char *s1);
int reprewriteok(char **a,char **b,int *c,int k);
void repproc(char *s);
int repgetproc(char *s);
int repdefinternal();
void repadddef(char *s);
int eval(tree *tr,char **ll,char **rr,int cc1);
extern char **cleft;;
extern char **cright;;
extern int *clen;;
extern int cc;;
int REPforp(char *s);
int REPforp1(tree *t0,char *pn1);
char *PREPreplaceRECORD(char *ss);
char *PREPreplaceRECORD1(char *ss,char *r0);