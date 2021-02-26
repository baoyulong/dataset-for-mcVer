#include "coi.h"
#include "dmc.h"
#include "main.h"
#include "main.h"
#include "mod.h"

/********************************************************************/
void checktrs(TBDprg *lgt)
{
	int k;
	TBDtrs **t1=lgt->trs;
	for (k=0;t1[k]!=NULL;k++); infos("checktrs"); infoi(k);
}
/********************************************************************/





/********************************************************************/
/*
#define DBMAIN
/********************************************************************/
LOCAL FILE *vdslog;
LOCAL ggbase *ggb;
LOCAL TBDprg *ggt;
/********************************************************************/
extern OPT op0;
extern int Xce;
extern int opDFTT;
extern int PAR_debug;
extern MODoption *MODopt;
/********************************************************************/
#define ggbprogspec ggb->pr->prog->right->right->right->right
/********************************************************************/
#define max(a,b)   	(a>b?a:b)
#define rversion()	printf("VERSION:    verds 1.48 - JAN 2016\n")
#define rfile(fn)	printf("FILE:       %s\n",fn);
/********************************************************************/
LOCAL int main(int argc, char *argv[])
{
	char *fn;
	tree *p,*fness;
	module *mm;
	TBDprg *prgt;
	prg *pr1;
	int r;

	logtmpinit();
	vdslog=fopen("logvds.tmp","w"); 
        rversion(); time(&t0); utiltime(0);

        MODopt=(MODoption *)calloc(1,sizeof(MODoption));

        paramopen(1); fn=mainparameters(argc,argv); paramopen(0);
#ifdef DBMAIN
	infoi(1001);
#endif

	if (op0.opc.filec!=NULL) {
		c2v(op0.opc.filec,op0.opc.filesp,op0.opc.filefsp);
		fn=(char *)malloc(strlen(op0.opc.filec)+5);
		gfilename(op0.opc.filec,fn); strcat(fn,".vvm");
	}
#ifdef DBMAIN
	infoi(1002);
#endif
	if (PAR_debug==6) exit(0);

	rfile(fn); if (!fileok(fn)) exit(0);

#ifdef DBMAIN
	infoi(1003);
#endif
 if (PAR_debug==5) exit(0);

        ggb=f2p0(fn,op0.ck,op0.string,argv,argc); 
	if ((op0.Oalg/1000)%10) invPOinit(ggb);

 if (PAR_debug) ggbwrite(ggb,vdslog); if (PAR_debug==4) exit(0);


/*
	infoi(op0.ck); treew(ggbprogspec);
*/
	if (op0.ck==-32) { specmerge21(ggbprogspec); op0.ck=1; }
/*
	treew(ggbprogspec);
*/
#ifdef DBMAIN 
	infoi(1004);
#endif

	if (op0.ck<0) {
		if (*op0.string==7) {
                        printf("INFO:       PROCEDURE not available\n");
                        reportresult(0,-2); exit(0);
		}
	}


#ifdef DBMAIN 
	infoi(1006);
#endif
       	for (mm=ggb->mm;(mm!=NULL)&&(mm->type!='P');mm=mm->next);
       	if ((mm!=NULL)&&(mm->type=='P')) op0.Oalg=max(11,op0.Oalg);


#ifdef DBMAIN 
	infoi(1007);
#endif
	if (opDFTT==1) fprintf(stdout,"PROPERTY:   deadlock-freeness\n");
	else if (opDFTT==2) fprintf(stdout,"PROPERTY:   tr-totalness\n");
	else if ((*op0.string==8)) {;}
	else if (op0.bs==4) {;}
	else if ((p=mgetp(ggb->pr->prog,op0.ck,1))==NULL) {
		reportresult(0,0);
		return 0;
	} 

#ifdef DBMAIN 
	infoi(1008);
#endif
	f2p1(ggb->pr->prog);
 if (PAR_debug) ggbwrite(ggb,vdslog); if (PAR_debug==3) exit(0);
/*
 ggbwrite(ggb,stdout);
*/
#ifdef DBMAIN 
	infoi(1009);
#endif
        if (vdslog!=NULL) {
                fprintf(vdslog,"VARALL\n"),
                treewrite(ggb->pr->prog->left,vdslog);
                fprintf(vdslog,"TRANSALL\n"),
                treewrite(ggb->pr->prog->right->right->left,vdslog);
        }
/*
 ggbwrite(ggb,stdout);
	if (op0.string!=NULL&&isalpha(*op0.string)) 
*/

	if (isalpha(*op0.string)) 
		var_order(ggb->pr,op0.string);

        if (op0.Ovord) { DEPanalysis(ggb->pr->prog,0); }
        if (op0.Om&&op0.ck>=1) { 
                fprintf(vdslog,"INITALL\n"),
                treewrite(ggb->pr->prog->right->left,vdslog);
		DEPanalysis(ggb->pr->prog,op0.ck); 
                fprintf(vdslog,"INIT\n"),
                treewrite(ggb->pr->prog->right->left,vdslog);
	}
	f2p1INFO(ggb);
#ifdef DBMAIN 
	infoi(1011);
#endif
/*
 ggbwrite(ggb,stdout);
*/
        if (vdslog!=NULL) {
                fprintf(vdslog,"VAR %i\n",ggb->pr->booln),
                treewrite(ggb->pr->prog->left,vdslog);
        }
#ifdef DBMAIN 
	infoi(1012);
#endif

	ggb->prb=p2bp(ggb->pr); 
	CEXoptcexprepare(ggb->prb->prog->left,&op0);
/*
 treewrite(ggb->prb->prog->left,stdout);
 treewrite(ggb->prb->prog->left,stdout);
 t1=treesearchvarlist("nf_idct",0,ggb->prb->prog->left);
 treewrite(t1->left,stdout);
*/

	p=mgetp(ggb->prb->prog,op0.ck,0);

#ifdef DBMAIN 
	infoi(1013);
#endif

 if (PAR_debug) prgwrite(ggb->prb,vdslog); if (PAR_debug==2) exit(0);

#ifdef DBMAIN 
	infoi(1010);
#endif
	if (op0.bs==0) {
		ggt=prg2t(ggb->prb,NULL);
		SYMcreate(op0.symfile,ggb->prb->prog->left);
/*
 checktrs(ggt);
*/
#ifdef DBMAIN 
	infoi(1015);
#endif
		if (opDFTT) r=dmcDFTT(opDFTT); else r=dmc(p,op0.Oalg);
#ifdef DBMAIN 
	infoi(1016);
#endif
		if (Xce==0&&ggt->cex!=NULL&&(PAR_debug==1||(r==0&&op0.ck!=-3)))
		{ 
#ifdef DBMAIN 
	infoi(1017);
#endif
/*
			treew(ggb->prb->prog->left);
*/
 			if (r==1) { ggt->cex->bw[0]=TBDtau; }
/*
			infos("The property is false, preparing files ..."); 
*/
  {
  	char fn0[32];
	if (op0.opc.filec!=NULL) gfilename(op0.opc.filec,fn0); else 
	gfilename(fn,fn0); 
	CEXoptcex(ggt->cex,&op0);
  	fprintf(stdout,"The property is false, preparing %s.cex ...\n",fn0); 
#ifdef DBMAIN 
	infoi(1018);
#endif
  	CEXwrite(ggt->cex,ggb->prb->prog->left,fn); 
#ifdef DBMAIN 
	infoi(1019);
#endif
  }
		}
		reportresult(2*r-1,-2); 
		return 2*r-1;
	} else {
		pr1=ggb->prb; 
		pr1->prog->right->right->left=mgett(pr1);
		fness=pr1->prog->right->right->right->left;
		r=bmc(p,pr1->prog,fness,pr1->varconstr,pr1->booln);
		if (vdslog!=NULL) {
                 fprintf(vdslog,"\n\nSTP %i\n",op0.opb.bound);
		 if (op0.bs==2)
		   fprintf(vdslog,"\nPAT %i\n",SPECnpath(p,0,op0.opb.bound,0));
		}
		reportresult(r,op0.opb.bound); 


		fclose(vdslog);
		if (Xce==0&&r==-1&&(op0.bs&2)==2) {
			char fn0[32];
			if (op0.opc.filec!=NULL) gfilename(op0.opc.filec,fn0); 
			else gfilename(fn,fn0); 
			strcat(fn0,".cex");
			bmcce("logvds.tmp",op0.opb.bound,fn0);
		}
		return r;
	}
}
/********************************************************************/

/*******************************************************************/
#define isag(a)   	(a->node[0]=='A'&&a->right->node[0]=='G')
#define getag(a)   	treecopy(a->right->right)
/*******************************************************************/
#define err_msg1        "error: no properties have been specified"
#define err_msg2        "error: the selection is not valid"
#define err_msg3        "error: the selected property does not exist"
/********************************************************************/
tree *mgetp(tree *prog,int k,int start)
{
	int i=0;
	tree *plist=prog->right->right->right->right;
	tree *p;
	char c;


	if (k<-3) return NULL; if (k<0) k=1;
	if (k>0) { 
		p=listget1n(plist,k);
		if (p!=NULL) {
			if (start) {
       				fprintf(stdout,"PROPERTY:   ");
                		treewrite(p,stdout); fprintf(stdout,"\n"); 
				op0.ck=k;
			}
			return p;
		}
		fprintf(stdout,"%s\n",err_msg3); return NULL;

	}
        while ((p=listgetone(plist,i++))!=NULL) {
                fprintf(stdout,"%i: ",i); treewrite(p,stdout);
                fprintf(stdout,"\n");
        }
        if (i==1) { fprintf(stdout,"%s\n",err_msg1); return NULL; }
	while (1) {
		k=0;
        	fprintf(stdout,"select a property (1-%i): ",i-1);
        	while ((c=getc(stdin))&&(isdigit(c)||(c!='\n'))) k=k*10+c-'0';
        	if ((k>=1&&k<i)) break; else fprintf(stdout,"%s\n",err_msg2); 
	}
	return mgetp(prog,k,start);
}
/*******************************************************************/
tree *mgett(prg *pr1)
{
	int i,j;
	tree *tr=NULL;

	for (i=0;i<pr1->prscnt;i++) 
	for (j=0;j<pr1->prs[i]->n;j++) {
		if (pr1->prs[i]->tslist[j]->left->node[0]=='_') continue;
		if (pr1->prs[i]->tslist[j]->right->node[0]=='_') continue;
		if (pr1->prs[i]->tslist[j]->left->node[0]=='^') {
			free(pr1->prs[i]->tslist[j]->left);
			pr1->prs[i]->tslist[j]->left=treeconstab(1,1);
		}
		pr1->prs[i]->tslist[j]->node[0]='&';
		if (tr==NULL) tr=pr1->prs[i]->tslist[j];
		else tr=treecompose("|",tr,pr1->prs[i]->tslist[j]);
	}
	return tr;
}
/*******************************************************************/
void specmerge21(tree *spec)
{
	tree *ns;

	if (spec->node[0]==';') {
		specmerge21(spec->right);
		if (isag(spec->left)&&isag(spec->right)) {
			ns=treecompose("A",NULL, 
			   treecompose("G",NULL,
			   treecompose("&",getag(spec->left),getag(spec->right))));
			treereplace(spec,ns);
		} else {
			spec->node[0]='&';
		}
		return;
	}
}
/*******************************************************************/

/********************************************************************/
LOCAL OPT op0;
LOCAL int opOD=0;
LOCAL int opDFTT=0;
LOCAL int opNOREP=1;
LOCAL int Xce=0;
LOCAL int opLIN=0;
LOCAL int opbSTEP=1;
LOCAL time_t t0,t1;
/********************************************************************/
LOCAL int PAR_tm=0;
LOCAL int PAR_debug=0;
/*******************************************************************/
#define ignore1(i) printf("warning: invalid option, ignored: %s\n",argv[i])
/*******************************************************************/
char *mainparameters(int argc, char *argv[])
{
	int i,j,temp;

	if (argc==1) return NULL;
  	for(i=1;i<=argc-2;i++)
	if (argv[i][0]=='-') {
      		temp=atoi(argv[i]+2);
      		if      (argv[i][1]=='T') 		PAR_tm=temp*60;
        	else if (!strcmp(argv[i]+1,"d")) 	PAR_debug++;
		else if ((j=MODparam(argv,i))>=0) i+=j;
		else i+=param0(argv,i);
	}
	return argv[i];
}
/********************************************************************/
int param0(char **argv, int i)
{
	char ctemp[32];

	if (!strcmp(argv[i]+1,"?")) 	{ help(); exit(0); }
	if (!strcmp(argv[i]+1,"h")) 	{ help(); exit(0); }
	if (!strcmp(argv[i]+1,"help")) 	{ help(); exit(0); }
	if (!strcmp(argv[i]+1,"rep"))   { opNOREP=1-opNOREP; return 0; } 
	if (!strcmp(argv[i]+1,"ag"))    { op0.ag=-1; return 0; } 
	if (!strcmp(argv[i]+1,"Oalg"))  { 
	   if (isdigit(*argv[i+1])) { op0.Oalg=atoi(argv[i+1]); return 1; }
	   op0.Oalg=0; return 0;
	}
	if (!strcmp(argv[i]+1,"b"))     { op0.Oalg=-1; return 0; }
	if (!strcmp(argv[i]+1,"f"))     { op0.Oalg=11; return 0; }
	if (!strcmp(argv[i]+1,"OD"))    { opOD=1; return 0; }
	if (!strcmp(argv[i]+1,"po"))    { op0.Oalg=1011; return 0; }
	if (!strcmp(argv[i]+1,"po2"))   { op0.Oalg=2011; return 0; }
	if (!strcmp(argv[i]+1,"dcx"))   { Xce=1; return 0; } 
	if (*(argv[i]+1)=='D')     	{ repadddef(argv[i]+2); return 0; } 
	if (!strcmp(argv[i]+1,"c"))     { 
		op0.opc.filec=argv[i+1]; op0.Oalg=11; return 1; 
	} 
	if (!strcmp(argv[i]+1,"sp"))    { op0.opc.filesp=argv[i+1]; return 1; } 
	if (!strcmp(argv[i]+1,"fsp"))   { op0.opc.filefsp=argv[i+1]; return 1; }
	if (!strcmp(argv[i]+1,"ck")) { 
	   if (isdigit(*argv[i+1])) 	{ op0.ck=atoi(argv[i+1]);  return 1;}
	   if (!strcmp(argv[i+1],"df")) { opDFTT=1; return 1; } 
	   if (!strcmp(argv[i+1],"tt")) { opDFTT=2; return 1; } 
	   if (!strcmp(argv[i+1],"dl")) { opDFTT=3; return 1; } 
	   if (!strcmp(argv[i+1],"sx")) { op0.ck=-1; return 1; }
	   if (!strcmp(argv[i+1],"all")) { op0.ck=-32; return 1; }
	   if (argv[i+1][0]=='i'&&isdigit(argv[i+1][1])) { 
		op0.ck=-3; 
		sprintf(ctemp,"int=%s",argv[i+1]);
		repadddef(ctemp);
		return 1; 
	   }
	   if (isalpha(*argv[i+1]))  {
		op0.ck=-2;
		strcpy(op0.string,argv[i+1]);
		return 1;
 	   }
	   ignore1(i); return 0;
	} 
	if (!strcmp(argv[i]+1,"v")) { 
	   if (isalpha(*argv[i+1])) { strcpy(op0.string,argv[i+1]); return 1; }
	   ignore1(i); return 0;
	} 
	return param1(argv,i);
}
int param1(char **argv, int i)
{
	if (!strcmp(argv[i]+1,"Om"))    { op0.Om=1; return 0; }
	if (!strcmp(argv[i]+1,"Ovord")) { op0.Ovord=1; return 0; }
	if (!strcmp(argv[i]+1,"SYM")) { op0.symfile=argv[i+1]; return 1; }
	return param2(argv,i);
}
/*************************************************************************/
#define bmcret(i) op0.opb.xbv=1; op0.bs=2; return i
/*************************************************************************/
int param2(char **argv, int i)
{
	if (!strcmp(argv[i]+1,"bcc")) 	 {op0.bs=8; return 0; }
	if (!strcmp(argv[i]+1,"QBF")) 	 {op0.bs=1; return 0; }
	if (!strcmp(argv[i]+1,"SAT")) 	 {op0.bs=2; return 0; }
	if (!strcmp(argv[i]+1,"bmc"))    {opLIN=0; bmcret(0); }
	if (!strcmp(argv[i]+1,"fsbmc"))  {opLIN=3; bmcret(0); }
	if (!strncmp(argv[i]+1,"fsbmc=",6)) {opLIN=atoi(argv[i]+7); bmcret(0); }
	if (!strcmp(argv[i]+1,"QBFS")) 	 {op0.bs=3; return 0; }
	if (!strcmp(argv[i]+1,"QBFD")) 	 {op0.bs=4; return 0; }
	if (!strcmp(argv[i]+1,"org")) 	 {opLIN=0; return 0; }
	if (!strcmp(argv[i]+1,"lin")) 	 {opLIN=64; return 0; }
	if (!strcmp(argv[i]+1,"lin2")) 	 {opLIN=128; return 0; }
	if (!strcmp(argv[i]+1,"bv")) 	 {op0.opb.xbv=0; return 0; }
	if (!strcmp(argv[i]+1,"bf")) 	 {op0.opb.xbv=1; return 0; }
	if (!strcmp(argv[i]+1,"lbound")) {
		op0.opb.lbound=atoi(argv[i+1]); return 1;
	}
	if (!strcmp(argv[i]+1,"bound"))  {
		op0.opb.bound=atoi(argv[i+1]); return 1;
	}
	if (!strcmp(argv[i]+1,"step"))  {
		opbSTEP=atoi(argv[i+1]); return 1;
	}
	if (!strcmp(argv[i]+1,"qbfsolver")){
		op0.opb.qbfsolver=argv[i+1]; return 1;
	}
	if (!strcmp(argv[i]+1,"satsolver")){
		op0.opb.satsolver=argv[i+1]; return 1;
	}
	if (!strcmp(argv[i]+1,"optcex"))     { 
		strcpy(op0.opcex.var,argv[i+1]); return 1;
	} 
	ignore1(i); return 0;
}
/********************************************************************/
int paramopen(int yes)
{
	switch (yes) {
        case 1: op0.ck=-32;
		op0.Oalg=11;
		op0.bs=0; /*op0bs=0*/
		op0.opb.lbound=0; op0.opb.bound=1000; op0.opb.xbv=-2;
		/*some problme with op0.opb.lbound=0 for -QBF and -QBFS*/
		op0.opc.filec=NULL; op0.opc.filesp=NULL; op0.opc.filefsp=NULL;
		repinit();
		break;
	case 0: 
		break;
	}
}
/********************************************************************/

/*******************************************************************/
extern ggbase *ggb;
/*******************************************************************/
TBD *pc2tbd(TBDprg *pp0,tree *pname,TBD *s0)
{
	tree *p;
	prg *pr1,*pr1b;
	TBD *r0,*r1;
	TBDprg *lgt;
 static TBDprg *ogt;
	int rr;
	int k=0;

	if (pname==NULL) {
		lgt=ggt; ggt=ogt; 
		r1=dmc2(s0,pp0->booln,0); ggt=lgt; return r1;
	}

 logvds("pc2tbd");
/*
 infos("pc2tbd");
 treew(pname);
*/
	p=procedurecreate(pname,ggb->mm,pp0->vlist,op0.ag+*op0.string);
	pr1=prog2prg(p,ggb->mm);

/*
 infos("p0");
 treew(pr1->prog);
 infos("p1");
*/

	if (pr1->prog->right->right->right->right!=NULL) {
		AGrewritevo(pr1->prog->right->right->right->right);
		pr1->prog->right->right->right->right
		=AGaddeq(pr1->prog->right->right->right->right,pp0->vlist);
	}

	pr1b=p2bp(pr1);
/*
 infos("p0");
 prgwrite(pr1b,stdout);
 infos("p1");
*/
	if (*op0.string==-1) *op0.string=0;
	if (*op0.string==8) {
		*op0.string=(-1)*(op0.ag+1);
		for (;1;op0.ag=-1,*op0.string=0) {
 /*no ag in the first call of the first round*/
 /*no ag in the second round*/
			logvds("CHECK Procedure");
			r0=dmc3(s0,pr1b->prog->right->right->right);
			r1=pc2tbd(pp0,pname,s0);
			rr=TBDmp(TBDcn(r1,TBDng(r0))); 
			if (rr==1||op0.ag==-1) break;
		}
		printf("INFO:       A/G=%i\n",2*(op0.ag)+1);
		reportresult(2*rr-1,-2); exit(0);
	}
	if (pr1b->prog->right->right->right->right==NULL) {
		lgt=ggt; ggt=prg2t(pr1b,NULL); ogt=ggt;
		r1=dmc2(s0,pp0->booln,0);
		/*TBDprgfree(ggt);*/ ggt=lgt; 
	} else {
		r1=dmc3(s0,pr1b->prog->right->right->right);
		if (r1==NULL) {
			printf("INFO:       ASSUMPTION violation\n");
			reportresult(0,-2); exit(1);
		}
	}
	/*prgfree(pr1); prgfree(pr1b);*/ 
	return r1;
}
/*******************************************************************/
tree *AGaddeq(tree *t,tree *vl)
{
	tree *l0;
	int lc=0;
	char node[MAXVARLEN];
	tree *t0;
	tree *t1=treeconst(1);
	tree *t1a,*t1b;
	int i,j,k;

	while ((l0=listgetone(vl,lc++))!=NULL) 
 {
	if (k=arrayindex(l0->left->node)) {
		i=ran(l0->left->node+k,0); j=ran(l0->left->node+k,1);
		for (;i<=j;i++) {
			sprintf(node,"O");
			strncpy(node+1,l0->left->node,k);
			sprintf(node+k,"_x_%i",i);
			t0=treesearch(node,0,t);
			if (t0==NULL) {
				t1a=treecompose(node,NULL,NULL);
				node[0]='V'; t1b=treecompose(node,NULL,NULL);
				t1=treecompose("&",t1,treecompose("=",t1a,t1b));
			}

		}
	} else {
		sprintf(node,"O%s",l0->left->node);
		t0=treesearch(node,0,t);
		if (t0==NULL) {
			t1a=treecompose(node,NULL,NULL);
			node[0]='V'; t1b=treecompose(node,NULL,NULL);
			t1b=treecompose(node,NULL,NULL);
			t1=treecompose("&",t1,treecompose("=",t1a,t1b));
		}
	}
 }
	return treecompose("&",t,t1);
}
/*******************************************************************/

/********************************************************************/
extern int opNOREP;
extern time_t t0,t1;
/********************************************************************/
void reportprogress(int bv,int k) 
{ 
	int i=k;

/*
	if (opNOREP) { if (i>0) { while (i%2==0) i=i/2; if (i!=1) return; }}
*/
	if (opDFTT) return;
 	switch (bv) {
  		case 0: printf("----------  "); break;
  		case 1: printf("check: %3i  ",k); break;
 	}
 	if (opNOREP==0) printf("time = %i",(int)difftime(time(&t1),t0));
	printf("\n"); fflush(stdout);
}
/********************************************************************/
int reportresult(int r,int k)
{
	int bv;

	bv=(int)difftime(time(&t1),t0);
       	if (r==1) printf("CONCLUSION: TRUE");
       	if (r==-1) printf("CONCLUSION: FALSE");
       	if (r==0) { printf("CONCLUSION: UNKNOWN"); k--; }
	if (opNOREP==0) {
		if (k>=-1) printf(" (time=%i bound=%i)",bv,k); 
		else printf(" (time=%i sec)",bv);
	}
	printf("\n"); fflush(stdout);
	return r;
}
/********************************************************************/
