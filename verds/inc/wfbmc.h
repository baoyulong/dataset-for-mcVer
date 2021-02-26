int satbmc0(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln);;
int bmc(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln);
int qbfbmc(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln);
int satbmc(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln);
int satbmcECTLx(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln);
void qmdwritecnf(qmdata *qm,qinfo *qi,FILE *g);
model *qmd2model(qmdata *qm,qinfo *qi);
int checksolver(char *s,char *txt);
int satbmc0(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln);
int qbfdiameter(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln);
int qbfdia1(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln,int k);
int bmcce(char *fn0,int b,char *fn1);
vinfo *getvarinfo(char *fn);
void vinfowrite(vinfo *vv,FILE *ff);
int numbers(char *ll);
int rename1(char *ll);
int getpathinfo(char *fn);
int SOLVEapply(char *solver,model *mo,int bv);
int SOLVEapply_0(char *solver);
int SOLVEapplyq(char *solver,qmdata *qm,qinfo *qi,int bv);
int qm2file(qmdata *qm,qinfo *qi,int bv,int k);
extern char *PAR_satsolver;;
