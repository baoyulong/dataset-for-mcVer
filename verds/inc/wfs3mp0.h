int imp_init(int nn);;
int imp_close(int nn);;
int imp_update(int x,int i);;
int imp_backtrack(int *cur);;
int *imp_res(int x,int m,mstruct *cc,mdata *wc);;
int imp_init(int nn);
int imp_close(int nn);
int imp_update(int x,int i);
int imp_backtrack(int *cur);
int *imp_res(int x,int m,mstruct *cc,mdata *wc);
int imp_res_local(int *p1,int c1,int *p2,int c2,int p,int x);
int imp_getvar(int *aa,int aac);
int imp_unit(mstruct *cc,mdata *wc,int *cl,float *cd,int *cur);
