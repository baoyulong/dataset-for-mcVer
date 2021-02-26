#include "bmc.h"
#include "bmc.h"

/********************************************************************/
LOCAL extern OPT op0;
LOCAL extern int opLIN;
LOCAL extern int opbSTEP;
LOCAL extern int PAR_tm;
LOCAL extern time_t t0,t1;
/********************************************************************/
#define isAG(t)	\
	(t->node[0]=='A'&&t->right->node[0]=='G'&&isProp(t->right->right))
/********************************************************************/
#define mreturn(r)	{op0.opb.bound=k;return r;}
#define prverdstmp()    {sprintf(fn,".verds_%i.%3i",bv,k);}
#define tout            (PAR_tm&&(int)difftime(time(&t1),t0)>PAR_tm)
#define message(s)    	{fprintf(stdout,"INFO:       applying %s\n",s);}
/********************************************************************/
int satbmc0(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln);
/********************************************************************/
int bmc(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln)
{
	int r;
	tree *p=spec;

        if (op0.bs==8) {
	if ((TLFisLTL(p)||TLFisACTLx(p)||TLFisECTLx(p)))
		op0.bs=2; else 
			/* op0.bs=3; some error with this algorithm */
			op0.bs=1; /* uses pure sat-based algorithm */
	}

	switch (op0.bs) {
		case 1: r=checksolver(op0.opb.qbfsolver,"QBFSOLVER");
			if (r==0) op0.opb.qbfsolver=NULL;
		case 3: r=checksolver(op0.opb.satsolver,"SATSOLVER");
			if (r==0) op0.opb.satsolver=NULL;
			r=qbfbmc(spec,prog,fness,varconstr,booln); 
			return r;
		case 2: r=checksolver(op0.opb.satsolver,"SATSOLVER");
			if (r==0) op0.opb.satsolver=NULL;
			if ((TLFisLTL(p)||TLFisACTLx(p))) {
				r=satbmc(spec,prog,fness,varconstr,booln); 
				return r;
			} else if ((TLFisECTLx(p))) {
				r=satbmcECTLx(spec,prog,fness,varconstr,booln); 
				return r;
			} 
			fprintf(stdout,"ERROR:      invalid options\n"); 
			op0.opb.bound=0; return 0;
		case 4: r=checksolver(op0.opb.qbfsolver,"QBFSOLVER");
			if (r==0) op0.opb.qbfsolver=NULL;
			r=qbfdiameter(spec,prog,fness,varconstr,booln); 
			return r;
	}
}
/********************************************************************/
int qbfbmc(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln)
{
	int i,k,r,bv;
	qmdata *qm;
	qinfo *qi;
	char fn[100];
	FILE *g;
	model *mm0;
/*
	int isreach=isAG(spec);
*/

	if (op0.bs==1) {
	  if (op0.opb.qbfsolver==NULL) { message("an internal QBF-solver"); }
	  else if (strcmp(op0.opb.qbfsolver,"_")) { message(op0.opb.qbfsolver);}
	} else /*=3*/ {
	  if (op0.opb.satsolver==NULL) { message("an internal SAT-solver"); }
	  else if (strcmp(op0.opb.satsolver,"_")) { message(op0.opb.satsolver);}
	}

	for (r=0,k=op0.opb.lbound;k<=op0.opb.bound;k+=opbSTEP) {

	    if (k==0) {
		r=satbmc0(spec,prog,fness,varconstr,booln);
		if (r) mreturn(r); continue;
	    }

	    for (bv=1;bv>=0;bv--) if (bv!=op0.opb.xbv) {
/*
		if (isreach&&bv) { reportprogress(bv,k); continue; }
*/
		if (tout) mreturn(r);
		qi=(qinfo *)calloc(1,sizeof(qinfo));
		qm=bp2q(qi,prog,fness,spec,booln,k,bv);
		if (op0.opb.qbfsolver==NULL||strcmp(op0.opb.qbfsolver,"_")) {
                        i=md2treetype(qm);
                        if (i==1&&op0.bs==3) {
                              mm0=qmd2model(mdnegate(qm),qi);
                              if (bv==1) r=SOLVEapply(op0.opb.satsolver,mm0,bv);
                              else r=-SOLVEapply(op0.opb.satsolver,mm0,!bv);
                              free(mm0->mstring); free(mm0);
                        } else if (i==2&&op0.bs==3) {
                              mm0=qmd2model(qm,qi);
                              if (bv==0) r=SOLVEapply(op0.opb.satsolver,mm0,bv);
                              else r=-SOLVEapply(op0.opb.satsolver,mm0,!bv);
                              free(mm0->mstring); free(mm0);
                        } else {
                              r=SOLVEapplyq(op0.opb.qbfsolver,qm,qi,bv);
                        }
		} else {
                	prverdstmp(); stringreplacechar(fn,' ','0');
                	g=fopen(fn,"w"); qmdwrite(qm,qi,g); fclose(g);
			i=md2treetype(qm);
			if (i==1) {
                		prverdstmp(); stringreplacechar(fn,' ','a');
                		g=fopen(fn,"w"); qmdwritecnf(mdnegate(qm),qi,g);
				fclose(g);
			} else if (i==2) {
                		prverdstmp(); stringreplacechar(fn,' ','e');
                		g=fopen(fn,"w"); qmdwritecnf(qm,qi,g);
				fclose(g);
			} 
			freeqmdata(qm); free(qi); 
		}
		reportprogress(bv,k); if (r) mreturn(r);
	    }
/*
	    if (isreach&&qbfdia1(spec,prog,fness,varconstr,booln,k)) mreturn(1);
*/
	}
	mreturn(r);
}
/********************************************************************/
int satbmc(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln)
{
	int i,k,r,bv;
	model *mm0;
	char fn[100];
	FILE *g;

	if (op0.opb.satsolver==NULL) { message("an internal SAT-solver"); }
	else if (strcmp(op0.opb.satsolver,"_")) { message(op0.opb.satsolver); }

	for (r=0,k=op0.opb.lbound;k<=op0.opb.bound;k+=opbSTEP) {
	    for (bv=1;bv>=0;bv--) if (bv!=op0.opb.xbv) {
		if (tout) mreturn(r);
		mm0=bp2s(prog,varconstr,spec,booln,k,bv,opLIN); 
		if (op0.opb.satsolver==NULL||strcmp(op0.opb.satsolver,"_")) {
			r=SOLVEapply(op0.opb.satsolver,mm0,bv); 
		} else {
                	prverdstmp(); stringreplacechar(fn,' ','0');
                	g=fopen(fn,"w"); cnfmodelwrite(mm0,g); fclose(g);
		}
		free(mm0->mstring); free(mm0); 
		reportprogress(bv,k); if (r) mreturn(r);
	    }
	}
	mreturn(r);
}
/********************************************************************/
int satbmcECTLx(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln)
{
}
/********************************************************************/
void qmdwritecnf(qmdata *qm,qinfo *qi,FILE *g)
{
	model *mo;
	tree *t=md2tree(qm);
	t=treedopost(t,TLFnnf);
	t=treedopost(t,TLFnnfsimp);
	mo=CNFspec2model(t,qi->n); cnfmodelwrite(mo,g); 
	treefree(t); free(mo->mstring); free(mo); 
}
/********************************************************************/
model *qmd2model(qmdata *qm,qinfo *qi)
{
	model *mo;
	tree *t=md2tree(qm);
	t=treedopost(t,TLFnnf);
	t=treedopost(t,TLFnnfsimp);
	mo=CNFspec2model(t,qi->n); treefree(t);
	return mo;
}
/********************************************************************/
int checksolver(char *s,char *txt)
{
	if (s==NULL) return 0;
	else if (strcmp(s,"_")==0) return 1;
	else if (fileok(s)==0) return 0;
       	else { fprintf(stdout,"%s:  %s\n",txt,s); return 1; }
}
/********************************************************************/
int satbmc0(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln)
{
	int i,k,r,bv;
	model *mm0;
	char fn[100];
	FILE *g;
	tree *bs1;

	r=0; k=0;
	    for (bv=1;bv>=0;bv--) if (bv!=op0.opb.xbv) {
		bs1=k0simplify2(treecopy(spec),bv);
		mm0=bp2s(prog,varconstr,bs1,booln,k,bv,opLIN); 
		if (op0.opb.satsolver==NULL||strcmp(op0.opb.satsolver,"_")) {
			r=SOLVEapply(op0.opb.satsolver,mm0,bv); 
		} else {
                	prverdstmp(); stringreplacechar(fn,' ','0');
                	g=fopen(fn,"w"); cnfmodelwrite(mm0,g); fclose(g);
		}
		free(mm0->mstring); free(mm0); treefree(bs1);
		reportprogress(bv,k); if (r) mreturn(r);
	    }

	return r;
}
/********************************************************************/
int qbfdiameter(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln)
{
	int i,k,r,bv=1;
	qmdata *qm;
	qinfo *qi;
	char fn[100];
	FILE *g;
	model *mm0;

	for (r=0,k=op0.opb.lbound;k<=op0.opb.bound;k+=opbSTEP) {
	    if (k==0) { continue; }
		if (tout) mreturn(r);
		qi=(qinfo *)calloc(1,sizeof(qinfo));
		qm=bp2dm(qi,prog,fness,spec,booln,k);
		if (op0.opb.qbfsolver==NULL||strcmp(op0.opb.qbfsolver,"_")) {
                        r=SOLVEapplyq(op0.opb.qbfsolver,qm,qi,1);
		} else {
                	prverdstmp(); stringreplacechar(fn,' ','0');
                	g=fopen(fn,"w"); qmdwrite(qm,qi,g); fclose(g);
			freeqmdata(qm); free(qi); 
		}
		reportprogress(1,k); if (r) mreturn(r);
	}
	mreturn(r);
}
/********************************************************************/
int qbfdia1(tree *spec,tree *prog,tree *fness,tree *varconstr,int booln,int k)
{
	int i,r=0,bv=1;
	qmdata *qm;
	qinfo *qi;
	char fn[100];
	FILE *g;
	model *mm0;

		qi=(qinfo *)calloc(1,sizeof(qinfo));
		qm=bp2dm(qi,prog,fness,spec,booln,k);
		if (op0.opb.qbfsolver==NULL||strcmp(op0.opb.qbfsolver,"_")) {
                        r=SOLVEapplyq(op0.opb.qbfsolver,qm,qi,1);
		} else {
                	prverdstmp(); stringreplacechar(fn,' ','0');
                	g=fopen(fn,"w"); qmdwrite(qm,qi,g); fclose(g);
			freeqmdata(qm); free(qi); 
		}
		return r;
}
/********************************************************************/

/***********************************************************************/
#define MAXLINE         0x00100000
/***********************************************************************/
#define movenext(ll)    while (isdigit(*ll)||*ll=='-') ll++;
#define move2n(ll)      while (!isdigit(*ll)&&!(*ll=='-')) ll++;
#define move2nn(ll)     movenext(ll); move2n(ll);
#define move2s(ll)      while (!isalpha(*ll)) ll++;
/********************************************************************/
#define readSAT(f,s,k,f1) \
        while (1) { \
        	fgets(line,MAXLINE,f); \
		if (feof(f)) { \
			fprintf(f1,"Counterexample not available yet.\n"); \
			fclose(f1); /*error(s);*/  exit(0); \
		} \
        	if (strncmp(line,s,k)==0) break; \
        } \
/********************************************************************/
#define read(f,s,k) \
        while (1) { \
        	fgets(line,MAXLINE,f); \
		if (feof(f)) { error(s); } \
        	if (strncmp(line,s,k)==0) break; \
        } \
/********************************************************************/
int bmcce(char *fn0,int b,char *fn1)
{
	int i,j,k,x,y,p;
	int m,n;
        vinfo *vv;
        FILE *f;
        FILE *f1=fopen(fn1,"w"); 
        char line[MAXLINE];
	char *ll;
	int v0,v1;

        f=fopen(tmp2file,"r"); if (f==NULL) return 0;
	p=getpathinfo(fn0); vv=getvarinfo(fn0); 
        readSAT(f,"SAT",3,f1); fgets(line,MAXLINE,f); ll=line;
	/*n=numbers(ll);*/ n=0; m=n/(p*(b+1)); /*m=n=0*/
	if (p==0||vv==NULL||m*(b+1)*p!=n) {
		fprintf(f1,"Counterexample not available for this case.\n");
		fclose(f1);
		return 0;
	}

	k=vv[0].max;
	for (y=1;y<=p;y++) {
	    for (i=0;i<=b;i++) {
		fprintf(f1,"\nSTATE %i-%i\n",y,i);
        	for (j=0;j<k;j++) {
            		if (vv[j].low==0) break;
			for (v1=0,x=vv[j].low;x<=vv[j].high;x++) {
				move2n(ll); v0=atoi(ll); movenext(ll);
				if (v0<0) v0=0 ; else v0=1;
				v1=v1*2+v0;
			}
			fprintf(f1,"	%s =	%i\n",(vv[j].name),v1);
        	}
            }
	}
}
/********************************************************************/
vinfo *getvarinfo(char *fn)
{
        FILE *f=fopen(fn,"r");
        char line[MAXLINE];
        char *ll,*cc;
        int i,n;
        vinfo *vv;
        
        read(f,"VAR ",4);
        if ((n=atoi(line+4))==0) { return NULL; }
        vv=(vinfo *)calloc(n+1,sizeof(vinfo));
        
        vv[0].max=n; 
        for (i=0;i<n;i++) {
                fgets(line,MAXLINE,f); if (feof(f)) break;
                ll=line; if (*ll!='(') break; while (!isdigit(*ll)) ll++;
                vv[i].low=atoi(ll); move2s(ll);
                cc=vv[i].name; while (*ll!=' ') *cc++=*ll++;
        }

        for (i=0;i<n;i++) {
            if (vv[i+1].low>0) vv[i].high=vv[i+1].low-1; else vv[i].high=n;
        }

 	for (i=0;i<n;i++) { if (vv[i].low>0) { rename1(vv[i].name); } }

/*
 for (i=0;i<n;i++)
 { 
        if (vv[i].low>0) { infoi(i); vinfowrite(&(vv[i]),stdout); }
 }
*/

	fclose(f);
        return vv;
}
/********************************************************************/
void vinfowrite(vinfo *vv,FILE *ff)
{
        fprintf(ff,"%s:%i-%i; m=%i; v=%i\n",
                vv->name,vv->low,vv->high,vv->max,vv->value);
}
/********************************************************************/
int numbers(char *ll)
{
	int v0;
	int i=0;

	while (*ll) {
		move2n(ll); v0=atoi(ll); if (v0==0) return i; 
		movenext(ll); i++; 
	}
}
/********************************************************************/
int rename1(char *ll)
{
	int k;
 rr:
	while (*ll&&strncmp(ll,"_x_",3)) ll++; if (*ll==0) return;
	if (!isdigit(*(ll+3))) goto rr;
	k=atoi(ll+3); sprintf(ll,"[%i]",k);
}
/********************************************************************/
int getpathinfo(char *fn)
{
        FILE *f=fopen(fn,"r");
        char line[MAXLINE];
        int n=0;
        
        while (1) {
        	fgets(line,MAXLINE,f); if (feof(f)) break;
        	if (strncmp(line,"PAT ",4)==0) n=atoi(line+4);
        } 

	fclose(f); return n;
}
/********************************************************************/

/********************************************************************/
#define mreturn(r)	{ op0.bound=k; return r; }
#define prverdstmp()    { sprintf(fn,".verds_%i.%3i",bv,k); }
/********************************************************************/

/*******************************************************************/
int SOLVEapply(char *solver,model *mo,int bv)
{
	FILE *g;
	int r;

        if (solver==NULL||*solver=='_') r=SATapply(mo); 
	else {
       		g=fopen(tmp0file,"w"); cnfmodelwrite(mo,g); fclose(g);
		r=SOLVEapply_0(solver); if (r==-1) return 0;
	}
	if (bv&&r==0) return 1;
	if (!bv&&!r==0) return -1;
	return 0;
}
/*******************************************************************/
int SOLVEapply_0(char *solver)
{
	char *ll,*l0;
	char cmd[256];
	int r;
	int i;

	i=strlen(solver);

	if (i>=7&&(strcmp(&(solver[i-7]),"minisat")==0)) {
	   sprintf(cmd,"%s %s %s > %s",solver,tmp0file,tmp2file,tmp1file);
	} else {
	   sprintf(cmd,"%s %s > %s",solver,tmp0file,tmp1file);
	}
	i=system(cmd); i/=256;
	if (i==10) r=1; else if (i==20) r=0; else r=-1;
/*
	ll=file2string(tmp1file);
	l0=stringsearch(ll,"UNSAT","SATISFIABLE"); 
	switch (*l0) {
		case 'S': r=1; break;
		case 'U': r=0; break;
		case  0:  r=-1; break;
	}
	free(ll); 
*/
	return r;
}
/*******************************************************************/

/*******************************************************************/
int SOLVEapplyq(char *solver,qmdata *qm,qinfo *qi,int bv)
{
        FILE *g;
        int r;

        if (solver==NULL||*solver=='_') r=QBFapply(qi,qm); 
	else {
                g=fopen(tmp0file,"w"); 
		qmdwrite(qm,qi,g); fclose(g); freeqmdata(qm); free(qi);
                r=SOLVEapply_0(solver); 
		if (r==-1) return 0;
        }

	if (bv&r==1) return 1;
	if (!bv&r==1) return -1;
        return 0;
}
/*******************************************************************/
int qm2file(qmdata *qm,qinfo *qi,int bv,int k)
{
        FILE *f;
	char fn[100];
	char *f1;

	sprintf(fn,"qbf.%i%2i",bv,k); 
	f1=fn; for (;*f1;f1++) if (*f1==' ') *f1='0'; 
	f=fopen(fn,"w"); qmdwrite(qm,qi,f); fprintf(f,"\n"); fclose(f);
}
