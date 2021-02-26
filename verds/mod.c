#include "mod.h"
#include "mod.h"
/*******************************************************************/
#define MAXINPUT	0x400000		/*2019*/
/*******************************************************************/
#define const1	"TRUE"
#define const2	"FALSE"
/*******************************************************************/
#define checkinv(inv) \
	if (inv!=NULL) { \
		stringcpy(s,inv); \
		for (i=0;i<m->varcnt;i++) {\
			getTXTvar(m->varlist[i],var);\
			s=rmDEFone(s,var,"1");\
		}\
		if (hasvar(s)) { fprintf(stdout,"%s\n",s); error("hasvar"); }\
		free(s); \
	} \
/*******************************************************************/
void checkVAR(MODmodule *m)
{
	int i;
	MODmodule *m1;
	char var[1024];
	char *s;

	for (m1=m;m1!=NULL;m1=m1->next) {
		checkinv(m1->init); 
		checkinv(m1->trans); 
		checkinv(m1->inv); 
		checkinv(m1->spec); 
	}
}
/*******************************************************************/
#define istempop(c) (c=='A'||c=='E'||c=='F'||c=='G'||c=='X'||c=='R'||c=='U')
/*******************************************************************/
int hasvar(char *s)
{
	for (;*s;s++) {
		if (strncmp(s,"TRUE",4)==0) { s+=3; continue; }
		else if (strncmp(s,"FALSE",5)==0) { s+=4; continue; }
		if (isalpha(*s)&&!(istempop(*s))) return 1; 
	}
	return 0;
}
/*******************************************************************/

/*******************************************************************/
MODmodule *clockMOD(MODmodule *m,MODoption *MODopt)
{
	char *s0;
	MODmodule *m1,*m0a;
	int i,n;
	char **lclock=(char **)malloc(1024*sizeof(char *));
	char var[1024];
	int lc=0;
	MODmoduleproc *mp;

	for (m1=m;m1!=NULL;m1=m1->next) if (strcmp(m1->key,"MODULE")==0) {
		mp=(MODmoduleproc *)calloc(1,sizeof(MODmoduleproc));
        	mp->parcnt=getTXTanylist(m1->trans,&(mp->parlist[0]),';',1);
		MODopt->trnumber+=mp->parcnt; freemoduleproc(mp);
	}

	var[0]=0; 
	for (m1=m;m1->next!=NULL;m1=m1->next);
	for (i=0;i<m->varcnt;i++) {
		if ((s0=move2word(m->varlist[i],"clock"))==NULL) continue;
		rMAXD(s0); break;
	}
	for (i=0;i<m->varcnt;i++) {
		if ((s0=move2word(m->varlist[i],"lclock"))==NULL) continue;
		sprintf(s0,"0..%i",MODopt->maxt);
		lc=getLCLOCKnames(m->varlist[i],&(lclock[lc]));
	}
	for (i=0;i<m->varcnt;i++) {
		if ((s0=move2word(m->varlist[i],"clock"))==NULL) continue;
		if (MODopt->clname[0]!=0) error("too many clocks");
		getTXTvar(m->varlist[i],var); strcpy(MODopt->clname,var);
		switch (MODopt->dir) {
			case 4: m->varlist[i][0]=0; 
				break;
			case 3: 
			default:sprintf(s0,"0..%i",MODopt->maxt); 
				break;
		}
		switch (MODopt->dir) {
			case 4: 
			case 3: 
			case 2: fprintf(stdout,"INFO:\tMAXD=%i\n",MODopt->maxt-1);
				break;
			case 1: fprintf(stdout,"INFO:\tMAXT=%i\n",MODopt->maxt); 
				break;
		}
		m->proclist[m->proccnt]=(char *)malloc(1024*sizeof(char));
		n=m->proccnt; 
		sprintf(m->proclist[n],"_process_%i:_mprocess_%i()",n+1,n+1);
		m->proccnt++;
		m0a=createclockmodule(m,var,m->proclist[n],lclock,lc,MODopt);
		m1->next=m0a; m1=m1->next;
	}
	addINVdone(m); 
	if (MODopt->dir==4&&var[0]!=0) addCLOCKdone(m,var,MODopt->maxt); 
	for (i=0;i<lc;i++) free(lclock[i]); free(lclock);
	return m;
}
/***********************************************************************/
MODmodule *createclockmodule(MODmodule *m,char *var,char *mpname,char **lclk,int lc,MODoption *MODopt)
{
	char *s;
	char *invcond,*idlec,*i1,*i2;
	char varass[1024];
	MODmodule *m0=(MODmodule *)calloc(1,sizeof(MODmodule));

	strcpy(m0->key,"MODULE");
	s=move2word(mpname,"_mp"); s=getTXTatom(s,m0->name);

	m0->init=(char *)malloc((strlen(var)+4)*sizeof(char));
	switch (MODopt->dir) {
		case 1: sprintf(m0->init,"%s=%i;",var,0); break;
		case 2:
		case 3: sprintf(m0->init,"%s=%i;",var,1); break;
		case 4: sprintf(m0->init,""); break;
	}
	sprintf(varass,"%s+1",var);
	invcond=addINVcond(m,var,varass);

	sprintf(varass,":%s=%s+1;\n",var,var);
	switch (MODopt->dir) {
		case 1: stringcat(m0->trans,invcond,varass); break;
		case 2: /*na_stateidle*/ 
			idlec=buildDLFnaidlecond(m);
			stringconj(i1,invcond,idlec); free(idlec); 
			stringcat(m0->trans,i1,varass); free(i1); 
			break;
		case 3: /*MODopt->na_clockidle*/
			idlec=buildLCLOCKcond(lclk,lc);
			stringconj(i1,invcond,idlec); free(idlec); 
			stringcat(m0->trans,i1,varass); free(i1); 
			break;
		case 4: 
			stringnew(m0->trans,2); m0->trans[0]=0;
			break;
	}
		
	if (lc&&MODopt->dir!=1) {
		i2=m0->trans;
		switch (MODopt->dir) {
		    case 2: i1=consLCLOCKtrans(var,lclk,lc,MODopt->maxt,NULL); 
			    break;
		    case 3:
		    case 4: i1=consLCLOCKtrans(var,lclk,lc,MODopt->maxt,invcond);
			    break;
		}
		stringcat(m0->trans,i2,i1); free(i2); free(i1);
	}
	free(invcond); return m0;
}
/*******************************************************************/
char *addINVcond(MODmodule *m,char *var,char *varass)
{
	char *s0,*s10;
	char *s1,*s1a,*s2a;
	char *s2;
	MODmodule *m1;
	char *s;
	int j;
        MODmoduleproc *mp;


	stringcpy(s,"1=1");
	for (m1=m;m1!=NULL;m1=m1->next) if (m1->inv!=NULL) {
		stringcpy(s10,m1->inv);
                mp=(MODmoduleproc *)calloc(1,sizeof(MODmoduleproc));
                mp->parcnt=getTXTanylist(s10,&(mp->parlist[0]),';',1);
		for (j=0;j<mp->parcnt;j++) {
		    stringcpy(s1,mp->parlist[j]);
		    if ((s2=rmDEFone(s1,var,varass))==s1) {free(s1);continue;}
		    s1=rmREPanyone(s2,";"," "); 
		    if ((s1a=rmREPsearch(s1,":"))==NULL) error("INV"); 
		    *s1a++=0;
		    stringimp(s2,s1,s1a); free(s1);
		    stringconj(s0,s2,s); s=s0; free(s2);
		}
                freemoduleproc(mp);
		free(s10);
	}
	return s;
}
/*******************************************************************/
void addCLOCKdone(MODmodule *m,char *var,int maxt)
{
	char val[1024];
	char *s1;

	sprintf(val,"%i",maxt);
	for (;m!=NULL;m=m->next) 
	if (m->trans!=NULL) { m->trans=rmDEFone(m->trans,var,val); }
}
/*******************************************************************/
void addINVdone(MODmodule *m)
{
	for (;m!=NULL;m=m->next) if (m->inv!=NULL) { free(m->inv); m->inv=NULL;}
}
/*******************************************************************/
int getLCLOCKnames(char *var,char **lclock)
{
	int i;
	char v1[1024];
	char v2[1024];
	char *v0;
	int low,high;

	getTXTvar(var,v1); getTXTword(var,v2); 
	if (strcmp(v1,v2)==0) { stringcpy(lclock[0],v2); return 1; }

	v0=v2+strlen(v1);
	while (*v0&&!(isdigit(*v0))) v0++; if (*v0==0) error("getLCLOCK");
	low=atoi(v0); 
	while (*v0&&(isdigit(*v0))) v0++; if (*v0==0) error("getLCLOCK");
	while (*v0&&!(isdigit(*v0))) v0++; if (*v0==0) error("getLCLOCK");
	high=atoi(v0); 

	for (i=low;i<=high;i++) {
		stringcpy(lclock[i],v2); 
		sprintf(lclock[i],"%s[%i]",v1,i);
	}
	return (i-low);
}
/*******************************************************************/
char *buildLCLOCKcond(char **lclk,int lc)
{
	int i;
	char *s,*s1,*s2;

	stringnew(s,1024); sprintf(s,"(1=0)",lclk[i]);
	for (i=0;i<lc;i++) {
		stringnew(s1,1024); sprintf(s1,"%s!=0",lclk[i]);
		stringdisj(s2,s,s1); free(s); free(s1); s=s2;
	}
	return s;
}
/*******************************************************************/
void rMAXD(char *s0)
{
	int temp;
	while (*s0&&!(isdigit(*s0))) s0++;

	if (MODopt->maxt==0&&*s0!=0) {
		temp=atoi(s0)+1;
		MODopt->maxt=1; while (MODopt->maxt<=temp) MODopt->maxt*=2;
		MODopt->maxt-=1;
		if (MODopt->dir==0) { MODopt->dir=4; } 
	}
	if (MODopt->maxt==0) { 
		MODopt->maxt=31; if (MODopt->dir==0) { MODopt->dir=4; } 
		fprintf(stdout,"WARN:	MAXD not specified\n");
	}

/*
	fprintf(stdout,"INFO:	MAXD=%i\n",MODopt->maxt-1);
*/
} 
/*******************************************************************/

/*******************************************************************/
char *buildDLFcond2(MODmodule *m)
{
        int i;
        MODmodule *m1;
        char *s2,*s0;
        MODmoduleproc *mp;
	char *s1=(char *)malloc((6)*sizeof(char));
	char *t1=(char *)malloc((1024*10)*sizeof(char));
	char *t2,*t0,*tp;
	int n;

	strcpy(s1,"1=0"); strcpy(t1,"1=0"); 
        for (m1=m;m1!=NULL;m1=m1->next) {
                if (m1->trans==NULL) continue;
                mp=(MODmoduleproc *)calloc(1,sizeof(MODmoduleproc));
	   	stringnew(s2,strlen(m1->trans)); stringnew(t2,strlen(s2)); 
                mp->parcnt=getTXTanylist(m1->trans,&(mp->parlist[0]),';',1);
		for (i=0;i<mp->parcnt;i++) {
		   getTXTuntilchar(mp->parlist[i],s2,':');
		   if ((tp=rmREPsearch(s2,"state=0"))==NULL){continue;}
		   getTXTatomicp(s2,tp,t2); /*s2=rmREPanyone(s2,t2,"1=1");*/
		   stringdisj(s0,s1,s2); free(s1); s1=s0;
		   stringdisj(t0,t1,t2); free(t1); t1=t0;
		}
	        free(s2); free(t2);
		freemoduleproc(mp);
        }
        stringimp(t0,t1,s1); free(s1); free(t1);
	stringpar(t1,t0); free(t0); 
	stringcat(t0,"AG",t1); free(t1); return t0;
}
/*******************************************************************/
char *buildDLFnaidlecond(MODmodule *m)
{
        int i;
        MODmodule *m1;
        char *s2,*s0;
        MODmoduleproc *mp;
	char *s1=(char *)malloc((6)*sizeof(char));
	char *t1=(char *)malloc((1024*10)*sizeof(char));
	char *t2,*t0,*tp;
	int n;

	strcpy(s1,"1=0"); strcpy(t1,"1=1"); 
        for (m1=m;m1!=NULL;m1=m1->next) {
                if (m1->trans==NULL) continue;
                mp=(MODmoduleproc *)calloc(1,sizeof(MODmoduleproc));
	   	stringnew(s2,strlen(m1->trans)); stringnew(t2,strlen(s2)); 
                mp->parcnt=getTXTanylist(m1->trans,&(mp->parlist[0]),';',1);
		for (i=0;i<mp->parcnt;i++) {
		   getTXTuntilchar(mp->parlist[i],s2,':');
		   if ((tp=rmREPsearch(s2,"state=0"))==NULL){continue;}
		   getTXTatomicp(s2,tp,t2); /*s2=rmREPanyone(s2,t2,"1=1");*/
		   stringdisj(s0,s1,s2); free(s1); s1=s0;
		   stringconj(t0,t1,t2); free(t1); t1=t0;
		}
	        free(s2); free(t2);
		freemoduleproc(mp);
        }
        free(s1); s1=t1; stringneg(t1,s1); 
	free(s1); return t1;
}
/*******************************************************************/

/*******************************************************************/
MODmodule *repSPECwithDLFcond(MODmodule *m0,int opt)
{
        char *dlfcond;

	switch (opt) {
		case 0: return m0;
		case 1: dlfcond=buildDLFcond(m0); break;
		case 2: dlfcond=buildDLFcond2(m0); break;
	}
        if (m0->spec!=NULL) free(m0->spec); m0->spec=dlfcond; return m0;
}
/*******************************************************************/
char *buildDLFcond(MODmodule *m)
{
        int i;
        MODmodule *m1;
        char *s2,*s0;
        MODmoduleproc *mp;
	char *s1=(char *)malloc((6)*sizeof(char));

	strcpy(s1,"1=0"); 
        for (m1=m;m1!=NULL;m1=m1->next) {
                if (m1->trans==NULL) continue;
		stringcpy(s2,m1->trans);
                mp=(MODmoduleproc *)calloc(1,sizeof(MODmoduleproc));
                mp->parcnt=getTXTanylist(m1->trans,&(mp->parlist[0]),';',1);
		for (i=0;i<mp->parcnt;i++) {
		   getTXTuntilchar(mp->parlist[i],s2,':');
                   stringdisj(s0,s1,s2); free(s1); s1=s0; 
		}
		freemoduleproc(mp); free(s2);
        }
        s2=(char *)malloc((strlen(s1)+5)*sizeof(char));
        sprintf(s2,"AG(%s)",s1); free(s1); return s2;
}
/*******************************************************************/

/*******************************************************************/
MODmodule *flattMOD(MODmodule *m,MODdefinition *def)
{
	int i;
	MODmodule *m0,*m1,*m0a;
	MODmoduleproc *mp;
	char *s;
	char s1[1024];

	if (m->proc==NULL) return m; 
	m0=m; m1=m->next; 
        m0->proccnt=getTXTanylist(m0->proc,&(m0->proclist[0]),';',1);
	for (i=0;i<m0->proccnt;i++) {
		s=m0->proclist[i];
/*
 infoi(100);
 infos(s);
 infoi(200);
*/
		mp=(MODmoduleproc *)calloc(1,sizeof(MODmoduleproc));
		s=getTXTatom(s,mp->pname);
		skipws(s); 
		if (*s++!=':') errors(s,"missing : in process definition");
		s=getTXTatom(s,mp->mname);
		skipws(s); 
		if (*s=='(') {
			getTXTuntilchar(s+1,s1,')');
			if (s1[strlen(s1)-1]==')') s1[strlen(s1)-1]=',';
/*
			mp->parcnt=getTXTwordlist(s1,&(mp->parlist[0]),',',1);
*/
			mp->parcnt=getTXTanylist(s1,&(mp->parlist[0]),',',1);
		}
		sprintf(m0->proclist[i],"%s:%s_%s()",mp->pname,mp->pname,mp->mname);
		m0->varcnt+=updatevarlist(mp,m1,&(m0->varlist[m0->varcnt]));
		m0a=createmodule(mp,m1,def); 
		m->next=m0a; m=m->next;
		freemoduleproc(mp);
	}
	freemodule(m1); 
	return m0; 
}
/*******************************************************************/
int updatevarlist(MODmoduleproc *mp,MODmodule *m1,char **varlist)
{
	int i;
	MODmodule *m0;
	int a=strlen(mp->pname)+2;

	m0=searchmodule(m1,mp->mname); if (m0==NULL) return 0;
	for (i=0;i<m0->varcnt;i++) {
	    varlist[i]=(char *)malloc((strlen(m0->varlist[i])+a)*sizeof(char));
	    sprintf(varlist[i],"%s.%s",mp->pname,m0->varlist[i]);
	}
	return i;
}
/*******************************************************************/
MODmodule *createmodule(MODmoduleproc *mp,MODmodule *m1,MODdefinition *def)
{
	int i;
	MODmodule *m0;
	MODmodule *new;
	char a0[1024],b0[1024];

	m0=searchmodule(m1,mp->mname); if (m0==NULL) return NULL;
	copymodulecontents(new,m0);
	sprintf(new->name,"%s_%s",mp->pname,mp->mname);
	for (i=0;i<m0->varcnt;i++) {
		getTXTatom(m0->varlist[i],a0);
	    	sprintf(b0,"%s.%s",mp->pname,a0);
		if (new->init!=NULL) new->init=rmDEFone(new->init,a0,b0);
		if (new->proc!=NULL) new->proc=rmDEFone(new->proc,a0,b0);
		if (new->trans!=NULL) new->trans=rmDEFone(new->trans,a0,b0);
		if (new->fair!=NULL) new->fair=rmDEFone(new->fair,a0,b0);
		if (new->inv!=NULL) new->inv=rmDEFone(new->inv,a0,b0);
		if (new->spec!=NULL) new->spec=rmDEFone(new->spec,a0,b0);
	}
	for (i=0;i<mp->parcnt;i++) {
		if (isArray(m0->parlist[i])) {
			getTXTatom(m0->parlist[i],a0);
			getTXTatom(mp->parlist[i],b0);
		} else {
			strcpy(a0,m0->parlist[i]);
			strcpy(b0,mp->parlist[i]);
		}
		if (new->init!=NULL) new->init=rmDEFone(new->init,a0,b0);
		if (new->proc!=NULL) new->proc=rmDEFone(new->proc,a0,b0);
		if (new->trans!=NULL) new->trans=rmDEFone(new->trans,a0,b0);
		if (new->fair!=NULL) new->fair=rmDEFone(new->fair,a0,b0);
		if (new->inv!=NULL) new->inv=rmDEFone(new->inv,a0,b0);
		if (new->spec!=NULL) new->spec=rmDEFone(new->spec,a0,b0);
	}
	if (new->init!=NULL) new->init=rmDEF(new->init,def);
	if (new->proc!=NULL) new->proc=rmDEF(new->proc,def);
	if (new->trans!=NULL) new->trans=rmDEF(new->trans,def);
	if (new->fair!=NULL) new->fair=rmDEF(new->fair,def);
	if (new->inv!=NULL) new->inv=rmDEF(new->inv,def);
	if (new->spec!=NULL) new->spec=rmDEF(new->spec,def);
	return new;
}
/*******************************************************************/
MODmodule *searchmodule(MODmodule *m1,char *name)
{
	while (m1!=NULL) { if (strcmp(m1->name,name)==0) break; m1=m1->next; }
	return m1;
}
/*******************************************************************/

/*******************************************************************/
char *consLCLOCKtrans(char *cname,char **lclock,int lcnt,int maxt,char *invcond)
{
	char *s0,*s1,*s2;
	char *s;
	int i,j,k,m;
	int *a=(int *)malloc(lcnt*sizeof(int));

	stringnew(s,1024); *s=0;
        for (k=1;k<=lcnt;k++) { /*was k=0, removed this all non-active case*/
                m=bin(lcnt,k); 
                for (i=1;i<=m;i++) {
                        for (j=0;j<lcnt;j++) a[j]=j+1;
                        binmarkpos(a,lcnt,k,i);
                        s0=bin0(lclock,a,lcnt,cname,maxt,invcond); 
                        s1=bin1(lclock,a,lcnt,cname,invcond);
			stringcat(s2,s0,": "); free(s0); s0=s2;
			stringcat(s2,s0,s1); free(s0); free(s1); s0=s2;
			stringcat(s2,s0,";\n"); free(s0); s0=s2;
			stringcat(s2,s,s0); free(s); free(s0); s=s2;
                }       
        }               
	return s;
}
/*******************************************************************/

/********************************************************************/
LOCAL MODoption *MODopt;
/********************************************************************/
#define myret()	{ \
	ss=writeMOD(m0); freemodule(m0); \
	if (fn1!=NULL) { ff=fopen(fn1,"w"); fprintf(ff,"%s",ss); fclose(ff); } \
	return ss; \
	} \
/********************************************************************/
char *MODapply(char *fn,char *fn1)
{
	int i;
	FILE *ff;
	char *string;
	MODmodule *m0;
	char *ss;
	MODdefinition *MODdef=(MODdefinition *)malloc(sizeof(MODdefinition));
	int MODoptSdlf=0;

	MODopt->trnumber=0; MODopt->clname[0]=0;

        string=MODfile2string(fn);
        string=rmCOM(string);
	MODdef->n=-1; string=rmDEF(string,MODdef);
	string=rmFOR(string);
	string=rmDEF(string,MODdef);
        string=rmPDEF(string);
        string=rmENUM(string);

	if ((m0=getMOD(string))==NULL) { 
		if (fn1!=NULL) { 
			ff=fopen(fn1,"w"); 
			fprintf(ff,"%s",string); 
			fclose(ff); 
		} 
		return string;
	}

	free(string);

	if (MODopt->test==Tmod) { myret(); } /*modules*/

	m0=flattMOD(m0,MODdef); rmDEFfree(MODdef);
	if (MODopt->test==Tflat) { myret(); } /*flattening*/

	m0=clockMOD(m0,MODopt); 
	if (MODopt->test==Tclock) {myret();} /*clock&inv*/

	if (MODopt->test==Tdlf1) MODoptSdlf=1; 
	if (MODopt->test==Tdlf2) MODoptSdlf=2; 
	if (MODoptSdlf>0) m0=repSPECwithDLFcond(m0,MODoptSdlf); /*repl spec */

	checkVAR(m0); myret();
}
/********************************************************************/
int MODparam(char **argv, int i)
{
	int temp;

	if (!strcmp(argv[i]+1,"deadlockfree")) { MODopt->test=Tdlf1; return 0; }
	if (!strcmp(argv[i]+1,"dir")) { MODopt->dir=atoi(argv[i+1]); return 1; }
	if (!strcmp(argv[i]+1,"MAXD"))  { 
		MODopt->dir=4;
		temp=atoi(argv[i+1])+1;
		MODopt->maxt=1; while (MODopt->maxt<=temp) MODopt->maxt*=2;
		MODopt->maxt-=1;
		fprintf(stdout,"INFO:	MAXD=%i\n",MODopt->maxt-1);
		return 1;
	} 
	if (!strcmp(argv[i]+1,"MAXT")) { /*no good*/
		MODopt->dir=1;
		temp=atoi(argv[i+1]);
		MODopt->maxt=1; while (MODopt->maxt<=temp) MODopt->maxt*=2;
		MODopt->maxt-=1;
		return 1;
	} 
	return -1;
}
/********************************************************************/

/*******************************************************************/
#define filltxt(cc,s) \
	cc=(char *)malloc((strlen(s)+1)*sizeof(char)); \
	s=getTXTuntilkeys(s,cc,10); \
/***********************************************************************/
MODmodule *getMOD(char *s)
{
	MODmodule *m,*m0,*m1;

	m=m0=(MODmodule *)calloc(1,sizeof(MODmodule));
	if ((s=getMODmain(s,m0))==NULL) { free(m0); return NULL; }
	while (1) {
		skipws(s);
		if (*s==0) break;
		m1=(MODmodule *)calloc(1,sizeof(MODmodule));
		if (strncmp(s,"MODULE",6)==0) s=getMODmodule(s,m1);
		if (strncmp(s,"PROC",4)==0) s=getMODproc(s,m1);
		m0->next=m1; m0=m0->next;
	}
	return m;
}
/*******************************************************************/
char *getMODmain(char *s,MODmodule *m)
{
	char *s0;
        char *alist=(char *)malloc((strlen(s)+1)*sizeof(char));
	char *a1;

	strcpy(m->key,"VVM");
	m->txt=(char *)malloc((strlen(s)+1)*sizeof(char));
	s0=getTXTuntilkeys(s+3,m->txt,1);
	s=m->txt; 
	s=getTXTline(s,m->name); 
	skipws(s);
	if (strncmp(s,"VAR",3)==0) {
		s=getTXTuntilkeys(s+3,alist,10);
		a1=alist; skipws(a1);
		if (*a1) m->varcnt=getTXTanyliststrict(alist,&(m->varlist[0]),';',1);
	}
	skipws(s); if (strncmp(s,"INIT",4)==0) { s+=4; filltxt(m->init,s); }
	skipws(s); if (strncmp(s,"PROC",4)==0) { s+=4; filltxt(m->proc,s); }
		   else { return NULL; }
	skipws(s); if (strncmp(s,"TRANS",5)==0) { s+=5; filltxt(m->trans,s); }
	skipws(s); if (strncmp(s,"FAIRNESS",8)==0) { s+=8; filltxt(m->fair,s); }
	skipws(s); if (strncmp(s,"INV",3)==0) { s+=3; filltxt(m->inv,s); }
	skipws(s); if (strncmp(s,"SPEC",4)==0) { s+=4; filltxt(m->spec,s); }
	free(alist); return s0;
}
/***********************************************************************/
char *getMODmodule(char *s,MODmodule *m)
{
	char *s0;
        char *alist=(char *)malloc((strlen(s)+1)*sizeof(char));
	char *a1;

	strcpy(m->key,"MODULE");
	m->txt=(char *)malloc((strlen(s)+1)*sizeof(char));
	s0=getTXTuntilkeys(s+6,m->txt,2);
	s=m->txt; 
	s=getTXTatom(s,m->name); 
	skipws(s);
	if (*s=='(') { 
		s=getTXTuntilchar(s+1,alist,')');
                m->parcnt=getTXTwordlist(alist,&(m->parlist[0]),',',1);
	}
	skipws(s);
	if (strncmp(s,"VAR",3)==0) {
		s=getTXTuntilkeys(s+3,alist,10);
		a1=alist; skipws(a1);
		if (*a1) m->varcnt=getTXTanyliststrict(alist,&(m->varlist[0]),';',1);
	}
	skipws(s); if (strncmp(s,"INIT",4)==0) { s+=4; filltxt(m->init,s); }
	skipws(s); if (strncmp(s,"TRANS",5)==0) { s+=5; filltxt(m->trans,s); }
	skipws(s); if (strncmp(s,"FAIRNESS",8)==0) { s+=8; filltxt(m->fair,s); }
	skipws(s); if (strncmp(s,"INV",3)==0) { s+=3; filltxt(m->inv,s); }
	skipws(s); if (strncmp(s,"SPEC",4)==0) { s+=4; filltxt(m->spec,s); }
	free(alist); return s0;
}
/*******************************************************************/
char *getMODproc(char *s,MODmodule *m)
{
	char *s0;

	strcpy(m->key,"PROC");
	m->txt=(char *)malloc((strlen(s)+1)*sizeof(char));
	s0=getTXTuntilkeys(s+4,m->txt,2);
	return s0;
}
/*******************************************************************/
#define mprintf(ff,a,b,c) 	{ sprintf(ff,a,b,c); ff+=strlen(ff); }
/*******************************************************************/
char *writeMOD(MODmodule *m)
{
	int i;
	char *ss=(char *)malloc(MAXINPUT*sizeof(char));
	char *ff=ss;

	for (;m!=NULL;m=m->next) 
	switch (m->key[0]) {
	case 'V':
		mprintf(ff,"VVM %s\n",m->name,0); 
		mprintf(ff,"VAR\n",0,0);
		for (i=0;i<m->varcnt;i++) {
		 if (m->varlist[i][0]!=0) mprintf(ff,"\t%s;\n",m->varlist[i],0);
		}
		if (m->init!=NULL) { mprintf(ff,"%s\n%s\n","INIT",m->init); }

		if (m->proc!=NULL) {
			if (m->proccnt>0) {
				mprintf(ff,"PROC\n",0,0);
				for (i=0;i<m->proccnt;i++) 
					mprintf(ff,"\t%s;\n",m->proclist[i],0);
			} else mprintf(ff,"%s\n%s\n","PROC",m->proc);
		}

		if (m->trans!=NULL) {mprintf(ff,"%s\n%s\n","TRANS",m->trans);}
		if (m->fair!=NULL) {mprintf(ff,"%s\n%s\n","FAIRNESS",m->fair);}
		if (m->inv!=NULL) {mprintf(ff,"%s\n%s\n","INV",m->inv);}
		if (m->spec!=NULL) {mprintf(ff,"%s\n%s\n","SPEC",m->spec);}
		mprintf(ff,"\n",0,0);
		break;
	case 'M':
		mprintf(ff,"MODULE %s(",m->name,0);
		for (i=0;i<m->parcnt-1;i++) mprintf(ff,"%s,",m->parlist[i],0);
		if (m->parcnt>0) mprintf(ff,"%s",m->parlist[m->parcnt-1],0); 
		mprintf(ff,")\nVAR\n",0,0);
		for (i=0;i<m->varcnt;i++) mprintf(ff,"\t%s;\n",m->varlist[i],0);
		if (m->init!=NULL) { mprintf(ff,"%s\n%s\n","INIT",m->init); }
		if (m->trans!=NULL) { mprintf(ff,"%s\n%s\n","TRANS",m->trans); }
		if (m->fair!=NULL) { mprintf(ff,"%s\n%s\n","FAIRNESS",m->fair);}
		if (m->inv!=NULL) { mprintf(ff,"%s\n%s\n","INV",m->inv); }
		if (m->spec!=NULL) { mprintf(ff,"%s\n%s\n","SPEC",m->spec); }
		mprintf(ff,"\n",0,0);
		break;
	case 'P':
		mprintf(ff,"PROC %s\n",m->txt,0); break;
	}

	if ((ff-ss-1)>=MAXINPUT) error("(ff-ss+1)>=MAXINPUT");
	return realloc(ss,(ff-ss+1)*sizeof(char));
}
/*******************************************************************/
#define myfree(x)	{if(x!=NULL)free(x);}
/*******************************************************************/
void freemodule(MODmodule *m)
{
	int i;
	if (m->next!=NULL) freemodule(m->next);
	for (i=0;m->parlist[i]!=NULL;i++) free(m->parlist[i]);
	for (i=0;m->varlist[i]!=NULL;i++) free(m->varlist[i]);
	for (i=0;m->proclist[i]!=NULL;i++) free(m->proclist[i]);
	myfree(m->txt); myfree(m->init); myfree(m->proc);
        myfree(m->trans); myfree(m->fair); myfree(m->inv);
        myfree(m->spec); myfree(m);
}
/*******************************************************************/

/*******************************************************************/
