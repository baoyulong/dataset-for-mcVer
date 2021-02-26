tree *DMCisef(tree *prop);
tree *DMCismixag(TBDprg *lggt,tree *prop);
tree *DMCismixaf(TBDprg *lggt,tree *prop);
tree *DMCismixagaf(TBDprg *lggt,tree *prop);
int DMCmixaf(TBD *s0,TBD *nq);
int DMCmixagaf(TBD *s0,TBD *p,TBD *nq);
int DMCpAF(TBD *p,TBD *nq);
int DMCpAF(TBD *p,TBD *nq);
int DMCmixag(TBD *s0,TBD *es,int opt);
TBD *TBDrsetckerror(TBD *s0,TBD *es,int opt);
int DMCprocedurewait;
TBD *TBDrset(TBD *s0,int opt);
void CEXreg(CEXtype *cex,TBD *s,int fw);
extern CEXtype *CEXinit(TBD *s0,TBD *es);
extern TBD *TBDomg;;
extern int opNOREP;;
int dmcDFTT(int dftt);
int dmc(tree *prop,int opt);
TBD *m2prop(tree *prop);
TBD *m2prope(tree *prop);
TBD *m2propa(tree *prop);
TBD *dmc2(TBD *s0,int booln,int opt);
TBD *dmc3(TBD *s0,tree *ag);
void TBDgetvarlist(TBD *g0,char *varl);
TBD *DMCfg(TBD* p0,TBD *ff());
TBD *DMCur(TBD* p0,TBD *q0,TBD *ff(),TBD *fg());
extern TBD *TBDtau;;
extern TBDprg *ggt;;
TBD *DMCex(TBD* p0);
TBD *DMCeu(TBD* p0,TBD *q0);
TBD *DMCer(TBD* p0,TBD *q0);
TBD *DMCef(TBD* p0);
TBD *DMCeg(TBD* p0);
TBD *DMCegik(TBD* r0,int i,int k);
void CEXoptcex(CEXtype *cex,OPT *op0);
void CEXoptcexprepare(tree *vlist,OPT *op0);
TBD *SYMred(TBD *t0);
void SYMcreate(char *fn,tree *vlist);
TBD *SYMredcheck(TBD *t0);
int optDMCef(TBD *init,TBD* p0);
TBD *TBDfwpo(TBD *frontier,TBD *stack,char **POdependent,char **POenable,char *atomT,char *POvisible, char **POpre);
extern char *atomicTrs;;
extern char *POvisibleTrs;;
extern char **POdependent;;
extern char **POenable;;
extern char **POpredecessor;;
TBD *TBDrsetckerrorpo(TBD *s0,TBD *es,int opt);
void getStr(tree *t);
TBD *getPCTBD(int i);
TBD *TBDpo(TBD *s0,int opt);
TBDtrspo **enableset(TBD *frontier,TBDtrspo **te0);
TBDtrspo **alreadyinc(TBD *frontier,TBDtrspo **ta0);
TBDtrspo **ampleset(TBD *frontier,char **POdep,char **POen, char *atT,char *POvis,char **POpre);
TBD *TBDpost(TBD *p0,TBDtrspo **ta);
void freetrspo(TBDtrspo **a);
void POread(char **pod,char *fn,int n);
int tesize(TBDtrspo **te);
extern char *atomicTrs;;
extern char *POvisibleTrs;;
extern char **POdependent;;
extern char **POenable;;
extern char **POpredecessor;;
char *getAtomicTrs(ggbase *ggb);;
char *getRelatedTrs(ggbase *ggb,char *atomicTrs,int a);;
char *getVisibleTrs(ggbase *ggb,char *atomicTrs,char **relatedTrs,tree *sp);;
void invPOinit(ggbase *ggb);
char *getAtomicTrs(ggbase *ggb);
char *getRelatedTrs(ggbase *ggb,char *atomicTrs,int a);
int strIsPC(char *str);
void getPC(tree *t,char *pc);
void getPCWrite(tree *t,char *pc);
int POatomic(tree *t);
int getTrscount(tree *trs);
int dependentvar(tree *t,char **vv,int k);
int dependentassign(tree *t,char **vv,int k);
int writevarPO(tree *t,char **vv,int k);
int strin(char *v0,char **vv,int k);
int emptyintersection(char **v0,int k0,char **v1,int k1);
char *getVisibleTrs(ggbase *ggb,char *atomicTrs,char **relatedTrs,tree *sp);
int sameProcess(tree *trs, int a, int b);
int dependent(ggbase *ggb,char *atomicTrs,char **relatedTrs,int a,int b);
int enable(ggbase *ggb,char *atomicTrs,char **relatedTrs,int a,int b);
int predecessor(char *atomicTrs,char **relatedTrs,int a,int b);
int strIsPC(char *str);
void getPCWrite(tree *t,char *pc);
tree *getprocedurename(prg *pr);
