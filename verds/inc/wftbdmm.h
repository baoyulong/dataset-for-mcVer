extern TBD *TBDomg;
extern TBD *TBDtau;
extern TBDmm *TBDttb;
extern TBDmm *TBDctb;
void TBDmminitialize(int n);
extern TBD *TBDll;
void TBDmmfree(TBDmm *mmtb);
TBD *TBDmmfreels(TBD *w);
TBD *TBDmmaddt(TBDmm *TBDttb,TBD *w);
TBD *TBDmmgetc(TBDmm *TBDctb,int x,TBD *a,TBD *b);
TBD *TBDmmaddc(TBDmm *nc,TBD *w);
TBD *TBDmmnewnode(int x,TBD *a,TBD *b,TBD *c);
void TBDmmresize(TBDmm *cache,TBD *f());
void TBDmmresizels(TBDmm *newcache,TBD *w,TBD *f());
int TBDmmmarkset(TBD *w,int marker);
void TBDmmmarkclear(TBDmm *mmtb,int marker);
void TBDmmmarkclearls(TBD *w,int marker);
int TBDmmnodecnt(TBD *w);
int TBDmmnodecntbg(TBD *w);
void TBDmmnodeclear(TBD *w,int marker);
void TBDkp(TBD *w);
void TBDdr(TBD *w);
void TBDcl();
int TBDmmnodecnt1(TBD *w);
int TBDmmnodecntbg1(TBD *w);
void TBDmmnodeclear1(TBD *w);
int TBDclck(TBD *i2);
void TBDcheck(TBD *w,int min,int max);
void TBDcheckodd(TBD *w,int odd);


void TBDwrite(TBD *w,int level,FILE *g);
void TBDwriteclause(TBD *w,int n,FILE *g);
int p2intlen(int *aa);
void p2intwrite(int *ss,int a,int b,FILE *g);
void p2writelev(TBD *w,int slevel,int elevel,FILE *g);
void p2writeclause(TBD *w,int *clause,FILE *g);