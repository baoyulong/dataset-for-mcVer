qinfo *q1read(char *fn,qinfo *qi);
qinfo *md2qifull(qmdata *md,qinfo *qi);
void freesdata(sdata *sd,qinfo *qi);
void freeqwdata(qwdata *wd,qinfo *qi);
void freeqinfo(qinfo *qi);
qmdata *file2md(char *fn,qinfo *qi);;
void md2qi(qmdata *md,qinfo *qi);;
int mdwrite(qinfo *qi,qmdata *md,FILE *g);;
int qiwrite(qinfo *qi,FILE *g);;
qmdata *file2md(char *fn,qinfo *qi);
int *linecreate(char *ll);
void linenegate(char *l1,char *l2);
qmdata *file2qmdataq(qinfo *qi,FILE *ff);
qmdata *file2qmdatacd(qinfo *qi,FILE *ff,char cd);
int mdwrite(qinfo *qi,qmdata *md,FILE *g);
int qmdatawrite1(qmdata *md,FILE *g,int mode);
void md2qi(qmdata *md,qinfo *qi);
int qmdatawrite2string(qinfo *qi,qmdata *md,int *clausem,qcdata *info,int p,int n);
int qiwrite(qinfo *qi,FILE *g);
void intwrite2string(int *ss,int a,int b,int *g);
char *file2string(char *fn); ;
qmodel *file2cnfmodel(char *fn,qmodel *mm);
qmodel *file2qmodel(char *fn,qmodel *mm);
sdata *qi2sd(qinfo *qi);;
qwdata *sd2wd(sdata *sd,qinfo *qi);;
int sdwrite(qinfo *qi,sdata *sd,FILE *g);;
int wdwrite(qinfo *qi,qwdata *wd,int level,FILE *g);;
int *wdbranch0(int qim,qwdata *wd,int c);;
int *wdbranch1(int qim,qwdata *wd,int c);;
sdata *qi2sd(qinfo *qi);
int sdwrite(qinfo *qi,sdata *sd,FILE *g);
qwdata *sd2wd(sdata *sd,qinfo *qi);
int wdwrite(qinfo *qi,qwdata *wd,int level,FILE *g);
int *wdbranch0(int qim,qwdata *wd,int c);
void wdbranch0back(qwdata *wd);
int *wdbranch0restore(qwdata *wd);
int *wdbranch1(int qim,qwdata *wd,int c);
void wdbranch1back(qwdata *wd);
