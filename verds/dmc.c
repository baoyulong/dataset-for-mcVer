#include "dmc.h"
#include "dmc.h"
#include "main.h"
#include "main.h"
#include "p2bp.h"
#include "tbdbw.h"
#include "tbdop.h"
#include "tlf.h"

/************************************************************************/
#define nextnq(r,q)		TBDcn(TBDfw(r),q)
/************************************************************************/
tree *DMCisef(tree *prop)
{
        tree *t;
        if (prop->node[0]!='E') return NULL; t=prop->right;
        if (t->node[0]!='F') return NULL; t=t->right;
        return t;
}
/************************************************************************/
tree *DMCismixag(TBDprg *lggt,tree *prop)
{
        tree *t;
        if (lggt->fairlist!=NULL) return NULL;
        if (prop->node[0]!='A') return NULL; t=prop->right;
        if (t->node[0]!='G') return NULL; t=t->right;
        return t;
}
/************************************************************************/
tree *DMCismixaf(TBDprg *lggt,tree *prop)
{
	tree *t;
	if (lggt->fairlist!=NULL) return NULL;
	if (prop->node[0]!='A') return NULL; t=prop->right;
	if (t->node[0]!='F') return NULL; t=t->right;
	return t;
}
/************************************************************************/
tree *DMCismixagaf(TBDprg *lggt,tree *prop)
{
	tree *t1,*t=prop;
	if (lggt->fairlist!=NULL) return NULL; 
	if (t->node[0]!='A') return NULL; t=t->right;
	if (t->node[0]!='G') return NULL; t=t->right;
	if (t->node[0]=='|') {
		t1=t; t=t->right;
		if (t->node[0]!='A') return NULL; t=t->right;
		if (t->node[0]!='F') return NULL; return t1;
	} 
	if (t->node[0]=='A') {
		t1=t; t=t->right;
		if (t->node[0]!='F') return NULL; return t1;
	} 
	return NULL;
}
/************************************************************************/
int DMCmixaf(TBD *s0,TBD *nq)
{
	return DMCpAF(s0,nq);
}
/************************************************************************/
int DMCmixagaf(TBD *s0,TBD *p,TBD *nq)
{
	int r;
	TBDkp(p); TBDkp(nq);
	r=DMCpAF(TBDcn(TBDrset(s0,0),p),nq);
	TBDdr(p); TBDdr(nq);
	return r;
}
/************************************************************************/
/*
int DMCpAF(TBD *p,TBD *nq)
{
	TBD *r1=p;
	int i=0;

	while (1) {
		reportprogress(1,i); reportprogress(0,i); i++;
		r1=TBDcn(r1,nq);
		if (TBDrc(r1)) { TBDrc(NULL); return 0; }
		if (TBDmp(r1)) { TBDrc(NULL); return 1; }
		r1=TBDfw(r1);
	}
}
/************************************************************************/
int DMCpAF(TBD *p,TBD *nq)
{
	TBD *r1=TBDcn(p,nq);
	int i=0;

	while (1) {
		reportprogress(1,i); reportprogress(0,i); i++;
		if (TBDrc(r1)) { TBDrc(NULL); return 0; }
		if (TBDmp(r1)) { TBDrc(NULL); return 1; }
		r1=nextnq(r1,nq);
	}
}
/************************************************************************/

/************************************************************************/
int DMCmixag(TBD *s0,TBD *es,int opt)
{
	int r;

	ggt->cex=CEXinit(s0,es);

	TBDkp(es); 
	switch ((opt/1000)%10) {
		case 0: r=TBDmp(TBDcn(TBDrsetckerror(s0,es,opt),es)); break;
		case 1: r=TBDmp(TBDcn(TBDpo(s0,opt),es)); break;
		case 2: r=TBDmp(TBDcn(TBDrsetckerrorpo(s0,es,opt),es)); break;
	}
	TBDdr(es);
	return r;
}
/************************************************************************/
TBD *TBDrsetckerror(TBD *s0,TBD *es,int opt)
{
        TBD *rset=TBDomg;
	int j,k,i=0;
	TBD *s1;
	int y;

	int aemp=(opt%10);
	int aerr=(opt/10)%10;
	int aclean=(opt/100)%10;

	reportprogress(1,i); reportprogress(0,i); 
	if (TBDmp(TBDcn(s0,es))==0) return TBDtau;
        while (1) {
		i++; reportprogress(1,i); reportprogress(0,i); 
                s1=s0; s0=TBDcn(s0,TBDng(rset)); s0=TBDfw(s0);  
/*
 if (i==10)
*/
		rset=TBDds(rset,s1); 

		CEXreg(ggt->cex,s0,1);

		switch (aemp) {
		    case 0: if (TBDrc(s0)) return rset; break;
		    case 1: if (TBDmp(s0)) return rset; break;
		    default:if ((i%aemp==0)) { if (TBDmp(s0)) return rset; }
			    break;
		}
		switch (aerr) {
		    case 0: break;
		    case 1: if (TBDmp(TBDcn(s0,es))==0) return TBDtau; break;
		    case 2: if ((i%aerr==0)) {
		      		if (TBDmp(TBDcn(rset,es))==0) return TBDtau;
					      /*rset,s0*/
			    }
			    break;
		}
		switch (aclean) {
		    case 0: break;
		    case 1: TBDkp(rset); y=TBDclck(s0); TBDdr(rset); break;
		    case 2: if ((i%aclean==0)) {
		    		TBDkp(rset); y=TBDclck(s0); TBDdr(rset); 
			    }
			    break;
		}
 s0=SYMred(s0);
        }
}
/************************************************************************/
int DMCprocedurewait=0;
/************************************************************************/
TBD *TBDrset(TBD *s0,int opt)
{
        TBD *s1,*rs0,*rs1,*rset=TBDomg;

/*
	if (TBDmp(s0)) return rset; 
	while (1) {
               	s1=TBDfw(s0); rset=TBDds(rset,s0); if (TBDmp(s1)) return rset; 
		s0=TBDcn(s1,TBDng(rset)); 
       	}
*/
	s1=s0; rs0=rset=TBDomg;
	while (1) {
		s0=TBDcn(s1,TBDng(rset)); if (TBDmp(s0)) return rset; 
		DMCprocedurewait=1; rs1=TBDomg;
        	while (1) {
                	s1=TBDfw(s0); 
			rset=TBDds(rset,s0); 
			rs1=TBDds(rs1,s0); 
			s0=TBDcn(s1,TBDng(rset)); if (TBDmp(s0)) break; 
        	}
		DMCprocedurewait=0;
               	s1=TBDfw(TBDcn(rs1,TBDng(rs0))); rs0=TBDds(rs0,rs1);
	}
}
/************************************************************************/




/*******************************************************************/
void CEXreg(CEXtype *cex,TBD *s,int fw)
{
	if (cex==NULL) return;
        if (fw) cex->fw[cex->cf++]=s; else cex->bw[cex->cb++]=s; TBDkp(s);
        if (cex->cb+cex->cf==CEXLEN) error("CEXreg");
}
/********************************************************************/
GLOBAL CEXtype *CEXinit(TBD *s0,TBD *es)
{
	CEXtype *cex=(CEXtype *)calloc(1,sizeof(CEXtype));
        CEXreg(cex,s0,1); CEXreg(cex,es,0); return cex;
}
/********************************************************************/




/*******************************************************************/
/*
#define DBDMC
/*******************************************************************/
#define OPDF 1
#define OPTT 2
/*******************************************************************/
extern TBD *TBDomg;
extern int opNOREP;
/*******************************************************************/
int dmcDFTT(int dftt)
{
	TBD *r1;
	TBD *r2;
	int i0;
	
	logs("DMCstart 0"); logi((int)difftime(time(&t1),t0));

	switch (dftt) {
	    case OPTT: 
		 r1=DMCex(TPtbdvar(ggt,"^")); 
		 return TBDmp(TBDng(r1));
	    case OPDF: 
		 r1=TBDrset(ggt->i,0);
		 r2=TBDcn(r1,TBDng(DMCex(r1)));
		 i0=TBDmp(r2);
		 if (i0==0) ggt->cex=CEXinit(r2,r2);
		 return i0;
	     default:
		error("dmcDFTT"); 
		return 0;
	}
}
/*******************************************************************/
int dmc(tree *prop,int opt)
{
	tree *t;
	TBD *r1,*es,*fair=TBDtau;
	TBD *p,*nq;
	
#ifdef DBDMC
 infoi(2000);
 infoi(opt);
 treewrite(prop,stdout);
 infos(" ");
#endif
	logs("DMCstart 0"); logi((int)difftime(time(&t1),t0));

	if (opt>=0) {
		if ((t=DMCismixagaf(ggt,prop))!=NULL) {
#ifdef DBDMC
 if (opNOREP==0) infos("REP: agaf");
 infoi(2005);
 treewrite(t,stdout);
 infos(" ");
#endif
			p=TBDtau;
			if (t->node[0]=='|') {
				p=m2prop(TLFnnf(TLFnegate(t->left))); 
				t=t->right;
			}
			nq=m2prop(TLFnnf(TLFnegate(t->right->right)));
			return DMCmixagaf(ggt->i,p,nq);
		}
		if ((t=DMCismixag(ggt,prop))!=NULL) {
#ifdef DBDMC
 infoi(2006);
 if (opNOREP==0) infos("REP: ag");
#endif
			es=m2prop(TLFnnf(TLFnegate(t))); 
#ifdef DBDMC
 infoi(20061);
#endif
        		return DMCmixag(ggt->i,es,opt);
		}
		if ((t=DMCismixaf(ggt,prop))!=NULL) {
 if (opNOREP==0) infos("REP: af");
			nq=m2prop(TLFnnf(TLFnegate(t))); 
			return DMCmixaf(ggt->i,nq);
		}
		if ((t=DMCisef(prop))!=NULL) {
 if (opNOREP==0) infos("REP: ef");
			if (ggt->fairlist!=NULL) fair=DMCeg(TBDtau);
			es=m2prop(TLFnnf((t))); 
			return optDMCef(TBDcn(fair,ggt->i),TBDcn(es,fair));
		}
	}
	
	r1=m2prop(TLFnnf(TLFnegate(prop))); 
	return TBDmp(TBDcn(ggt->i,r1));
}
/************************************************************************/
TBD *m2prop(tree *prop)
{
        TBD *r;

 
        switch (prop->node[0]) {
            case 'A': /* if (fair==NULL) { r= m2propa(prop->right); break; } */
		      prop=TLFnegate(prop);
		      r= TBDng(m2prop(prop)); 
		      prop=TLFnegate(prop); break;
            case 'E': r= m2prope(prop->right); break;
            case '|': r= TBDds(m2prop(prop->left),m2prop(prop->right)); break;
            case '&': r= TBDcn(m2prop(prop->left),m2prop(prop->right)); break;
            case '!': r= TBDng(m2prop(prop->right)); break;
            default:  r= TPfml(ggt,prop); break;
        }
        TBDkp(r); return r;
}
/************************************************************************/
TBD *m2prope(tree *prop)
{
	TBD *r=NULL;
	switch (prop->node[0]) {
	    case 'X': r= DMCex(m2prop(prop->right)); break;
	    case 'F': r= DMCef(m2prop(prop->right)); break;
	    case 'G': r= DMCeg(m2prop(prop->right)); break;
	    case 'U': r= DMCeu(m2prop(prop->left),m2prop(prop->right)); break;
	    case 'R': r= DMCer(m2prop(prop->left),m2prop(prop->right)); break;
	}
	TBDkp(r); return r;
}
/************************************************************************/
/*
TBD *m2propa(tree *prop)
{
	TBD *r=NULL;
	switch (prop->node[0]) {
	    case 'X': r= DMCexa(m2prop(prop->right)); break;
	    case 'F': r= DMCefa(m2prop(prop->right)); break;
	    case 'G': r= DMCega(m2prop(prop->right)); break;
	    case 'U': r= DMCeua(m2prop(prop->left),m2prop(prop->right)); break;
	    case 'R': r= DMCera(m2prop(prop->left),m2prop(prop->right)); break;
	}
	TBDkp(r); return r;
}
/************************************************************************/
#define addnret2p(p,nret)  if (p!=NULL) { TBDdr(p); p=TBDcn(p,nret); TBDkp(p);}
/************************************************************************/
TBD *dmc2(TBD *s0,int booln,int opt)
{
	int i;
	TBD *r0,*r1;
	TBD *nret;
	
	logs("DMCstart2 0"); logi((int)difftime(time(&t1),t0));

	nret=TBDap(-(2*ggt->booln-1)); r0=TBDcn(ggt->i,nret);

	for (i=0;i<ggt->trscnt;i++) {
		addnret2p(ggt->trs[i]->p,nret);
		addnret2p(ggt->trs[i]->pt,nret);
		addnret2p(ggt->trs[i]->tc,nret);
	}
/*
 TBDwrite(ggt->i,10,stdout);
 infoi(booln);
 infoi(ggt->booln);
*/
	r1=TBDrset(TBDcn(r0,s0),opt); 
	r1=TBDrestrict(r1,2*ggt->booln-1);
	for (i=2*booln+1;i<2*ggt->booln-1;i+=2) r1=TBDxb(i,r1);

	return r1;
}
/************************************************************************/
TBD *dmc3(TBD *s0,tree *ag)
{
	int i;
	TBD *a0,*g0;

	a0=TPfml(ggt,ag->left);
 	if (!TBDmp(TBDcn(s0,TBDng(a0)))) return NULL;

	g0=TPfml(ggt,ag->right);
	return TBDrn(TBDrp(s0,g0,1,1));
}
/************************************************************************/
/*
void TBDgetvarlist(TBD *g0,char *varl)
{
	int x;
	if (g0==TBDomg||g0==TBDtau) return;
	if (g0->x<0) x=-g0->x; else x=g0->x; if (varl[x]==0) varl[x]=1;
	TBDgetvarlist(g0->a,varl);
	TBDgetvarlist(g0->b,varl);
	TBDgetvarlist(g0->c,varl);
}
/************************************************************************/

/************************************************************************/
#define DMCeq(new,new0)	((ff==TBDds&&imp(new,new0))||(ff==TBDcn&&imp(new0,new)))
/*
#define DMCeq(new,new0)	(0)
#define DMCeq(new,new0)	(new==new0) [already covered by rc()]
/************************************************************************/
#define condbreak()	if (TBDrc(new)) break
/*
#define condbreak()	if (i++%10==0) if (DMCeq(new,new0)) {TBDrc(NULL);break;}
/************************************************************************/
TBD *DMCfg(TBD* p0,TBD *ff())
{
        TBD *inc,*new,*new0;

	cl(p0); 
        for (new0=new=p0;1;new0=new) {
            	inc=TBDbw(new); new=ff(p0,inc); condbreak();
        }
	return new;
}
TBD *DMCur(TBD* p0,TBD *q0,TBD *ff(),TBD *fg())
{
        TBD *inc,*new,*new0;

	clcl(p0,q0);
        for (new0=new=q0;1;new0=new) {
            	inc=TBDbw(new); new=ff(q0,fg(p0,inc)); condbreak();
        }
	return new;
}
/************************************************************************/

/************************************************************************/
extern TBD *TBDtau;
extern TBDprg *ggt;
/************************************************************************/
TBD *DMCex(TBD* p0)
{
	TBD *f0=TBDtau;
	if (ggt->fairlist!=NULL) f0=DMCeg(f0); return DMCex0(TBDcn(p0,f0));
}
/************************************************************************/
TBD *DMCeu(TBD* p0,TBD *q0)
{
	TBD *f0=TBDtau;
	if (ggt->fairlist!=NULL) f0=DMCeg(f0); return DMCeu0(p0,TBDcn(q0,f0));
}
/************************************************************************/
TBD *DMCer(TBD* p0,TBD *q0)
{
	TBD *f0=TBDtau;
	if (ggt->fairlist==NULL) return DMCer0(p0,q0);
	return TBDds(DMCeg(q0),DMCeu(q0,TBDcn(q0,p0)));
}
/************************************************************************/
TBD *DMCef(TBD* p0)
{
	TBD *f0=TBDtau;
	if (ggt->fairlist!=NULL) f0=DMCeg(f0); return DMCef0(TBDcn(p0,f0));
}
/************************************************************************/
TBD *DMCeg(TBD* p0)
{
 static TBD *fair=NULL;
	int i,k,m;
	TBD *n0=TBDtau;
	TBD **fn=ggt->fairlist;

	if (p0==TBDtau&&fair!=NULL) return fair;

	if (fn==NULL) return DMCeg0(p0); 
	for (m=0;fn[m]!=NULL;m++); n0=DMCegik(p0,0,m); 
	if (p0==TBDtau) { TBDkp(n0); fair=n0; } return n0;
/*
	n0=d2eg(TBDcn(p0,fair));
	n0=d2eg(p0);
*/
	n0=p0;
	for (k=1;k<=m;k++) 
	for (i=0;i<=m-k;i++) { n0=TBDcn(n0,DMCegik(n0,i,i+k)); }
/*
	if (m>1) for (i=0;i<m;i++) n0=TBDcn(n0,DMCegik(n0,i,i+1));
	if (m>=2) for (i=0;i<m-1;i++) n0=TBDcn(n0,DMCegik(n0,i,i+2));
*/
}
/************************************************************************/
TBD *DMCegik(TBD* r0,int i,int k)
{
        TBD *n0,*n1,*n2;
        TBD **fn=ggt->fairlist;
        int j;

        for (n0=TBDtau;1;n0=n1) {
                TBDkp(n0);
                for (n1=r0,j=i;j<k;j++) {
                        TBDkp(n1);
                        n1=TBDcn(n1,DMCex0(DMCeu0(r0,TBDcn(n0,fn[j]))));
                        TBDdr(n1);
                }
                TBDdr(n0);
                if (TBDmp(TBDcn(n0,TBDngpos(n1)))) break;
        }
        return n0;
}
/************************************************************************/




/*******************************************************************/
void CEXoptcex(CEXtype *cex,OPT *op0)
{
	TBD *t,*s,*s0,*s1,*smin;
	int i,x,n;
	int y=0;
	int low,high;

	if (op0->opcex.varlow==0) return;
	if (cex==NULL||cex->cf==0) return;
	if (cex->cb!=1) return;


        s0=TBDcn(cex->fw[cex->cf-1],cex->bw[0]);
 	low=2*op0->opcex.varlow-1; high=2*op0->opcex.varhigh-1;

       	for (s=s0,x=0,i=low;i<=high;i+=2) {
		x*=2; 
		t=TBDcn(s,TBDng(TBDap(i)));
		if (TBDmp(t)) { s=TBDcn(s,TBDap(i)); x+=1; } else { s=t; }
	}
	smin=s; cex->fw[cex->cf-1]=smin;
       	for (s=s0,y=0,i=low;i<=high;i+=2) {
		y*=2; 
		t=TBDcn(s,TBDap(i));
		if (TBDmp(t)) { s=TBDcn(s,TBDng(TBDap(i)));} else { s=t; y+=1;}
	}
	fprintf(stdout,"(MIN,MAX)(%s) = (%i,%i)\n",op0->opcex.var,x,y);
/*
 infoi(100); TBDwrite(s0,5,stdout);
*/
/*
        s1=s0; n=TBDmaxlabel(s1); 
	for (i=1;i<low;i++) s1=TBDxb(i,s1);
       	for (i=high+1;i<=n;i++) s1=TBDxb(i,s1);

       	for (s=s1,x=0,i=low;i<=high;i+=2) {
		x*=2; 
		t=TBDrestrict(s,i); 
		if (TBDmp(t)) { s=TBDrestrict(s,-i); } else { s=t; x+=1; }
	}
       	for (s=s1,y=0,i=low;i<=high;i+=2) {
		y*=2; 
		t=TBDrestrict(s,-i); 
		if (TBDmp(t)) { s=TBDrestrict(s,i); y+=1; } else { s=t; }
	}
	fprintf(stdout,"(MIN,MAX)(%s) = (%i,%i)\n",op0->opcex.var,x,y);
/*
       	for (s=s1,i=low;i<=high;i+=2) {
		t=TBDrestrict(s,i); 
		if (TBDmp(t)) { s=TBDrestrict(s,-i); s0=TBDrestrict(s0,-i); }
		else { s=t; s0=TBDrestrict(s0,i); }
	}
        cex->fw[cex->cf-1]=s0;
*/
}
/********************************************************************/
void CEXoptcexprepare(tree *vlist,OPT *op0)
{
        tree *t1;

        if (op0->opcex.var[0]==0) return;
	t1=treesearchvarlist(op0->opcex.var,0,vlist);

/*
         treewrite(vlist,stdout);
*/
        if(t1!=NULL) {
                op0->opcex.varlow=atoi(t1->left->left->node);
                op0->opcex.varhigh=atoi(t1->left->right->node);
        }
/*
         infoi(op0->opcex.varlow); infoi(op0->opcex.varhigh);
*/
}
/********************************************************************/





/*******************************************************************/
LOCAL TBD **SYMleft;
LOCAL int *SYMileft;
LOCAL int *SYMiright;
LOCAL int SYMscnt=0;
LOCAL TBD **SYMcut;
LOCAL int *SYMicut;
LOCAL int SYMccnt=0;
LOCAL TBD **SYMres;
LOCAL int *SYMires;
LOCAL int SYMrcnt=0;
LOCAL TBD **SYMcon;
LOCAL int *SYMicon;
LOCAL int SYMconcnt=0;
/*******************************************************************/
#define equiv(a,b) 	(TBDds(TBDcn(a,b),TBDcn(TBDng(a),TBDng(b))))
/*******************************************************************/
TBD *SYMred(TBD *t0)
{
	int i;
	TBD *s0,*s1,*t1;
	TBD *a0,*a1;
	static lev=0;

	if (SYMscnt==0&&SYMccnt==0&&SYMrcnt==0) return t0;
	for (i=0;i<SYMrcnt;i++) { t0=TBDcn(t0,SYMres[i]); }
	for (i=0;i<SYMscnt;i++) { t0=TBDcn(t0,SYMleft[i]); }
	for (i=lev;i<SYMccnt;i++) {
		if (SYMicut[i]) { t1=TBDrestrict(t0,SYMicut[i]); } else 
			        { t1=TBDcn(t0,SYMcut[i]) ; }
		if (TBDmp(t1)==0) {t0=TBDcn(t0,SYMcut[i]); if(lev<=i)lev=i+1; }
	}
	for (i=0;i<SYMconcnt;i++) {
		if (SYMicon[i]) { t1=TBDrestrict(t0,SYMicon[i]); } else 
			        { t1=TBDcn(t0,SYMcon[i]) ; }
		if (TBDmp(t1)==0) {t0=TBDcn(t0,SYMcon[i]); }
	}
/*
/*
	for (i=0;i<SYMscnt;i++) { 
		t1=TBDrestrict(TBDrestrict(t0,-SYMileft[i]),SYMiright[i]);
		t1=TBDcn(t1,TBDcn(TBDap(SYMileft[i]),TBDap(-SYMiright[i])));
		t0=TBDcn(t0,SYMleft[i]); 
		t0=TBDds(t0,t1);
	}
*/
	return t0;
}
/*******************************************************************/
void SYMcreate(char *fn,tree *vlist)
{
	int i=0;
	char l0[1024];
	char *l1,*l2;
	FILE *ff;
	TBD *e0,*t0,*t1;
	TBD *t0a,*t1a;
	tree *s0,*s1;
	int i01,i02,i11,i12;
	char a00[1024];
	char b00[1024];
	char c0[1024];
	char c1[1024];
	char c2[1024];
	char c3[1024];
	char *x;
	int cut=0;
    	tree *x0;

	if (fn==NULL) return;

	SYMscnt=0; SYMccnt=0; SYMrcnt=0; SYMconcnt=0; 

	ff=fopen(fn,"r");
	while (1) { fgets(l0,1024,ff); if (feof(ff)) break; i++; } fclose(ff);
/*
	SYMeq=(TBD**)malloc(i*sizeof(TBD *));
*/
	SYMres=(TBD**)malloc(i*sizeof(TBD *));
	SYMires=(int*)malloc(i*sizeof(int));
	SYMcut=(TBD**)malloc(i*sizeof(TBD *));
	SYMicut=(int*)malloc(i*sizeof(int));
	SYMleft=(TBD**)malloc(i*sizeof(TBD *));
	SYMileft=(int*)malloc(i*sizeof(int));
	SYMiright=(int*)malloc(i*sizeof(int));
	SYMcon=(TBD**)malloc(i*sizeof(TBD *));
	SYMicon=(int*)malloc(i*sizeof(int));

/*
 treewrite(vlist,stdout);
*/

	ff=fopen(fn,"r");
	while (1) { 
		fgets(l0,1024,ff); if (feof(ff)) break; l1=l0; 
		if (cut==8) continue;
		if (strncmp(l0,"SYM",3)==0) { cut=0; continue; }
		if (strncmp(l0,"CUT",3)==0) { cut=1; continue; }
		if (strncmp(l0,"RES",3)==0) { cut=2; continue; }
		if (strncmp(l0,"CON",3)==0) { cut=3; continue; }
		if (strncmp(l0,"===",3)==0) { cut=8; continue; }
		while (*l1&&*l1!=' '&&*l1!='='&&*l1!='<') l1++; 
		c3[0]=*l1; c3[1]=0;
		*l1++=0; l2=l1;
		while (*l2&&*l2!=' '&&*l2!='\n') l2++; *l2++=0;
		if (*l0==0||*l1==0) continue;
		strcpy(a00,l0); strcpy(b00,l1);
                for (x=a00;*x;x++)
                if (*x=='[') { strcpy(c0,x+1); strcpy(x,"_x_"); strcpy(x+3,c0);}
                else  if (*x==']') { strcpy(c0,x+1); strcpy(x,c0); }  
                for (x=b00;*x;x++)
                if (*x=='[') { strcpy(c0,x+1); strcpy(x,"_x_"); strcpy(x+3,c0);}
                else  if (*x==']') { strcpy(c0,x+1); strcpy(x,c0); }  
        	s0=treesearchvarlist(a00,0,vlist);
		if (s0==NULL) continue;
		i01=atoi(s0->left->left->node);
		i02=atoi(s0->left->right->node);
		t0=TBDap(2*i01-1);
		SYMileft[SYMscnt]=2*i01-1;
		switch (cut) {
			case 3:
			    if (i01==i02) {
			      if (*l1=='1') {
				SYMicon[SYMconcnt]=2*i01-1;
				SYMcon[SYMconcnt]=t0; 
			      } else {
				SYMicon[SYMconcnt]=-(2*i01-1);
				SYMcon[SYMconcnt]=TBDng(t0);
			      }
			      TBDkp(SYMcon[SYMconcnt]); SYMconcnt++;
			      continue;
			    }
			case 1:
			    if (i01==i02) {
			      if (*l1=='1') {
				SYMicut[SYMccnt]=2*i01-1;
				SYMcut[SYMccnt]=t0; 
			      } else {
				SYMicut[SYMccnt]=-(2*i01-1);
				SYMcut[SYMccnt]=TBDng(t0);
			      }
			      TBDkp(SYMcut[SYMccnt]); SYMccnt++;
			      continue;
			    }
			case 2:
			    sprintf(c1,"V%s",a00);
			    sprintf(c2,"%s",b00);
			    x0=treecompose(c3,treecompose(c1,NULL,NULL),
					       treecompose(c2,NULL,NULL));
			    BOOLreplace(x0,vlist,ggt->booln);
 			    x0=treedopost(x0,ARITapply); 
        		    x0=treedopost(x0,TLFnnf); 
			    x0=treedopost(x0,TLFnnfsimp); 
			    t0a=TPfml(ggt,x0); treefree(x0); 
/*
	infoi(100); treewrite(x0,stdout);
	infoi(100); TBDwrite(t0,5,stdout);
*/
			    if (cut==1) { 
					SYMicut[SYMccnt]=0;
					SYMcut[SYMccnt]=t0a; 
			    		TBDkp(SYMcut[SYMccnt]); SYMccnt++;
			    } else if (cut==3) { 
					SYMicon[SYMconcnt]=0;
					SYMcon[SYMconcnt]=t0a; 
			    		TBDkp(SYMcon[SYMconcnt]); SYMconcnt++;
			    }
			    else { SYMres[SYMrcnt]=t0a; SYMrcnt++; }
			    continue;
		}
		if (i01!=i02) continue;
        	s1=treesearchvarlist(b00,0,vlist);
		if (s1==NULL) continue;
		i11=atoi(s1->left->left->node);
		i12=atoi(s1->left->right->node);
		if (i11!=i12) continue;
		t1=TBDap(2*i11-1);
		SYMiright[SYMscnt]=2*i11-1;
		SYMleft[SYMscnt]=TBDds(equiv(t0,t1),TBDcn(t0,TBDng(t1))); 
/*
		e0=TBDtau;
		t0a=TBDap(2*i01); t1a=TBDap(2*i11);
		for (i=1;i<ggt->booln;i+=2) {
			if (i!=i01&&i!=i11)
			e0=TBDcn(e0,equiv(TBDap(i),TBDap(i+1)));
		}
		SYMeq[SYMscnt]=TBDcn(e0,TBDcn(equiv(t0,t1a),equiv(t1,t0a)));
*/
		SYMscnt++;
/*
 infoi(i01);
 infoi(i11);
 TBDwrite(t0a,5,stdout);
*/
	} 
	fclose(ff);
	t0=TBDtau; for (i=0;i<SYMrcnt;i++) { t0=TBDcn(t0,SYMres[i]); }
	SYMres[0]=t0; TBDkp(SYMres[0]); SYMrcnt=1;
	t0=TBDtau; for (i=0;i<SYMscnt;i++) { t0=TBDcn(t0,SYMleft[i]); }
	SYMleft[0]=t0; TBDkp(SYMleft[0]); SYMscnt=1;
}
/********************************************************************/
/*
LOCAL TBD **SYMeq;
TBD *SYMredcheck(TBD *t0)
{
	int i;
	TBD *s0,*s1,*t1;
	TBD *a0,*a1;

	if(SYMscnt==0) return t0;
	for (i=0;i<SYMscnt;i++) {
/*
		s0=TBDcn(t0,SYMleft[i]); s1=TBDbwone(s0,SYMeq[i],ggt->booln);
		if (TBDmp(TBDcn(s0,TBDng(s1)))) t0=s1; 
		else if (TBDmp(TBDcn(s1,TBDng(s0)))) t0=s0; 
**
		s0=TBDrestrict(t0,SYMileft[i]);
		s0=TBDrestrict(s0,-SYMiright[i]);
		s1=TBDrestrict(t0,-SYMileft[i]);
		s1=TBDrestrict(s1,SYMiright[i]);

		if (TBDmp(s0)||TBDmp(s1)) continue;

		a0=TBDap(SYMileft[i]); a1=TBDap(SYMiright[i]);

		if (TBDmp(TBDcn(s0,TBDng(s1)))) {
			t1=TBDds(equiv(a0,a1),TBDcn(a1,TBDng(a0)));
			t0=TBDcn(t0,t1);
		} else if (TBDmp(TBDcn(s1,TBDng(s0)))) {
			t1=TBDds(equiv(a0,a1),TBDcn(a0,TBDng(a1)));
			t0=TBDcn(t0,t1);
		}
	}
	return t0;
}
/********************************************************************/

/************************************************************************/
#define condbreak()	if (TBDrc(new)) break
/************************************************************************/
int optDMCef(TBD *init,TBD* p0)
{
        TBD *inc,*new,*new0;

	clcl(p0,init); 
	init=TBDcn(init,TBDng(p0)); 
        for (inc=new=p0;1;) {
            	inc=TBDbw(inc); 
		init=TBDcn(init,TBDng(inc)); if (TBDmp(init)) return 1;
		new=TBDds(new,inc); 
		condbreak();
        }
	return 0;
}
/************************************************************************/


/************************************************************************/
TBD *TBDfwpo(TBD *frontier,TBD *stack,char **POdependent,char **POenable,char *atomT,char *POvisible, char **POpre)
{
	TBD *image,*cycle;
	TBDtrspo **ta,**te;
	int i;

		ta=ampleset(frontier,POdependent,POenable,atomT,POvisible,POpre);
               	image=TBDpost(frontier,ta); 
		cycle=TBDcn(image,stack);
		if (!TBDmp(cycle)) { 
			te=enableset(cycle,NULL);
               		image=TBDds(image,TBDpost(cycle,te)); 
			freetrspo(te);
		}
		freetrspo(ta);
		return image;
}

extern char *atomicTrs;
extern char *POvisibleTrs;
extern char **POdependent;
extern char **POenable;
extern char **POpredecessor;
/************************************************************************/
TBD *TBDrsetckerrorpo(TBD *s0,TBD *es,int opt)
{
        TBD *rset=TBDomg;
	int j,k,i;
	TBD *s1;
	int y;
/*
	char **POdependent,**POenable;
    char *atomicTrs;
    char *POvisibleTrs;
    char **POpredecessor;
*/
	int xi=0;
	int aemp=(opt%10);
	int aerr=(opt/10)%10;
	int aclean=(opt/100)%10;
/*
	POinit(); 
*/
	reportprogress(1,xi); reportprogress(0,xi); 
	if (TBDmp(TBDcn(s0,es))==0) return TBDtau;
        while (1) {
		xi++; reportprogress(1,xi); reportprogress(0,xi); 
                s1=s0; s0=TBDcn(s0,TBDng(rset)); 
		s0=TBDfwpo(s0,rset,POdependent,POenable,atomicTrs,POvisibleTrs,POpredecessor);  
		rset=TBDds(rset,s1); 

		CEXreg(ggt->cex,s0,1);

		switch (aemp) {
		    case 0: if (TBDrc(s0)) return rset; break;
		    case 1: if (TBDmp(s0)) return rset; break;
		    default:if ((i%aemp==0)) { if (TBDmp(s0)) return rset; }
			    break;
		}
		switch (aerr) {
		    case 0: break;
		    case 1: if (TBDmp(TBDcn(s0,es))==0) return TBDtau; break;
		    case 2: if ((i%aerr==0)) {
		      		if (TBDmp(TBDcn(rset,es))==0) return TBDtau;
					      /*rset,s0*/
			    }
			    break;
		}
		switch (aclean) {
		    case 0: break;
		    case 1: TBDkp(rset); y=TBDclck(s0); TBDdr(rset); break;
		    case 2: if ((i%aclean==0)) {
		    		TBDkp(rset); y=TBDclck(s0); TBDdr(rset); 
			    }
			    break;
		}
        }
}
/************************************************************************/

/************************************************************************/
#define myret(s) \
	for (i=0;i<ggt->trscnt;i++) {free(POdependent[i]);free(POenable[i]);}\
	free(POdependent); free(POenable); return s;\
/************************************************************************/
#define TBDtrspoinit(te) \
	te=(TBDtrspo **)malloc(ggt->trscnt*sizeof(TBDtrspo *)); \
        for (k=0;t1[k]!=NULL;k++) te[k]=(TBDtrspo *)calloc(1,sizeof(TBDtrspo));\
/************************************************************************/
#define POunion(j,en) \
	if (ta[j]->trok==0) { ta[j]->trok=1; ta[j]->p=TBDcn(en,t1[j]->p); } \
	else { p1=TBDcn(en,t1[j]->p); ta[j]->p=TBDds(ta[j]->p,p1); } \
/************************************************************************/

/************************************************************************/
void getStr(tree *t)
{
	if (t==NULL) return;
    
    getStr(t->left); infos(t->node);getStr(t->right); 

}
/*******************************************************************/
#define repsimp(a,b,c)	\
	BOOLreplace(a,b,c); a=treedopost(a,ARITapply); \
	a=treedopost(a,TLFnnf); a=treedopost(a,TLFnnfsimp); \
/************************************************************************/
TBD *getPCTBD(int i)
{
    tree *trs=ggb->pr->prog->right->right->left;
    tree *pcNode;
    char pcStr[64];
    TBD *pcTBD;
    
    getPCWrite(listget1n(trs,i+1)->left,pcStr);
    pcNode=string2tree(pcStr);
    repsimp(pcNode,ggb->pr->prog->left,ggb->pr->booln);
    pcTBD=TPfml(ggt,pcNode);
    treefree(pcNode);
    return pcTBD;
}


TBD *TBDpo(TBD *s0,int opt)
{
        TBD *frontier=s0,*stack=TBDomg;
	TBD *image=frontier,*cycle;
	TBDtrspo **ta,**te;
/*
	char **POdependent,**POenable;
    char *atomicTrs;
    char *POvisibleTrs;
    char **POpredecessor;
*/    
	int i;
	int xi=0;
/*
	POinit(); 
*/
        reportprogress(1,xi); reportprogress(0,xi);
	while (1) {
		stack=TBDds(stack,image); 
		if (TBDmp(frontier)) { myret(stack); }
                xi++; reportprogress(1,xi); reportprogress(0,xi);
		ta=ampleset(frontier,POdependent,POenable,atomicTrs,POvisibleTrs,POpredecessor);
               	image=TBDpost(frontier,ta); 
		cycle=TBDcn(image,stack);
		if (!TBDmp(cycle)) { 
			te=enableset(cycle,NULL);
               		image=TBDds(image,TBDpost(cycle,te)); 
			freetrspo(te);
		}
		frontier=TBDcn(image,TBDng(stack)); 
		freetrspo(ta);
	}
}
/************************************************************************/
TBDtrspo **enableset(TBD *frontier,TBDtrspo **te0)
{
        TBDtrs **t1=ggt->trs; 
	TBDtrspo **te;
	int k;
    TBD *intersection;

	TBDtrspoinit(te);
    
	if (te0==NULL) {
           	for (k=0;t1[k]!=NULL;k++){ intersection=TBDcn(t1[k]->p,frontier);
		if (!TBDmp(intersection)) {te[k]->trok=1;te[k]->p=intersection;}}
	} else {
        	for (k=0;t1[k]!=NULL;k++) if (te0[k]->trok) { intersection=TBDcn(t1[k]->p,frontier);
		if (!TBDmp(intersection)) {te[k]->trok=1;te[k]->p=intersection;}}
	}
	return te;
}
/************************************************************************/
TBDtrspo **alreadyinc(TBD *frontier,TBDtrspo **ta0)
{
        TBDtrs **t1=ggt->trs; 
	TBDtrspo **ts;
	int k;
	TBD *left;
	TBDtrspoinit(ts);
	
        for (k=0;t1[k]!=NULL;k++) if (ta0[k]->trok) {
		left=TBDcn(TBDcn(t1[k]->p,frontier),TBDng(ta0[k]->p));
		if (TBDmp(left))
			ts[k]->trok=1;
		else ts[k]->p=left;    
		/*the left states of transition k to be checdked*/ 
	}
	else ts[k]->p=TBDcn(t1[k]->p,frontier);     
	/*all states in frontier of transition k to be checdked*/ 

	return ts;
}
/***********************************************************************/
TBDtrspo **ampleset(TBD *frontier,char **POdep,char **POen, char *atT,char *POvis,char **POpre)
{
        TBDtrs **t1=ggt->trs; 
	/*TBDtrspo **te=enableset(frontier,NULL);*/
	TBDtrspo **ta;
	/*TBDtrspo **te1;*/
	TBD *p1,*p2;
	int k,j,i;
    int mpSame, mpDiff;
/********************************/
	struct taDL
	{
	  int no;
	  TBD *p;
	  struct taDL *next;
	};
	typedef struct taDL TADL;
	
	 struct tceDL
	{
	  int no;
	  struct tceDL *next;
	};
	typedef struct tceDL TCEDL;
	TADL *tadlHead, *tadlCur, *tadlTail;   /*dynamic list of ample set*/
	TCEDL *tcedlHead, *tcedlCur, *tcedlTail;  /*dynamic list of dependent disable transitions*/
	int *tceSL;   /*static list used to check whether a transition was put in the dynamic list or not*/
	tadlHead=(TADL *)malloc(sizeof(TADL));
	tadlHead->next=NULL;
	tadlTail=tadlHead;
	tceSL=(int *)malloc(ggt->trscnt*sizeof(int));
/********************************/
    
	TBDtrspoinit(ta);

    for (k=0;t1[k]!=NULL;k++){ 
        p1=TBDcn(t1[k]->p,frontier);
        if (!TBDmp(p1)) {
		  ta[k]->trok=1;
		  ta[k]->p=p1; 
		  tadlCur=tadlHead->next;
        	for (;tadlCur!=NULL;tadlCur=tadlCur->next){
			     ta[k]->p=TBDcn(ta[k]->p,TBDng(tadlCur->p));
			     if (TBDmp(ta[k]->p)) { ta[k]->trok=0; break; }
		    }
		  if(ta[k]->trok==1){			/*add (t_k, ta[k]->p) to TADL*/
			tadlTail->next=(TADL *)malloc(sizeof(TADL));
			tadlTail->next->no=k;
			tadlTail->next->p=ta[k]->p;
			tadlTail->next->next=NULL;
			tadlTail=tadlTail->next;
		  }
	   }
    }

	tadlCur=tadlHead->next;
    for (;tadlCur!=NULL;tadlCur=tadlCur->next){
		/*te1=enableset(tadlCur->p,te);
		if (tesize(te1)<=1) { freetrspo(te1); continue; }*/
		k=tadlCur->no;

        if(POvis[k]||!atT[k]){for(j=0;j<ggt->trscnt;j++){
            if(ta[j]->trok)
			    p1=TBDcn(TBDcn(t1[j]->p,tadlCur->p),TBDng(ta[j]->p));
	        else 
			    p1=TBDcn(t1[j]->p,tadlCur->p);
	        if(!TBDmp(p1)){
                POunion(j,p1);
                tadlTail->next=(TADL *)malloc(sizeof(TADL));
				tadlTail->next->no=j;
				tadlTail->next->p=p1;
				tadlTail->next->next=NULL;
				tadlTail=tadlTail->next;
            }
        }continue;}

		tcedlHead=(TCEDL *)malloc(sizeof(TCEDL));
		tcedlHead->next=NULL;
		tcedlTail=tcedlHead;
		for(j=0;j<ggt->trscnt;j++)tceSL[j]=0;

        for (j=0;t1[j]!=NULL;j++) if(POdep[k][j]){
            tcedlTail->next=(TCEDL *)malloc(sizeof(TCEDL));
            tcedlTail->next->no=j;
            tcedlTail->next->next=NULL;
	        tcedlTail=tcedlTail->next;
		    tceSL[j]=1;
        }

        tcedlCur=tcedlHead->next;
        for (;tcedlCur!=NULL;tcedlCur=tcedlCur->next){
            j=tcedlCur->no;
	     if(ta[j]->trok)
			p1=TBDcn(TBDcn(t1[j]->p,tadlCur->p),TBDng(ta[j]->p));
	     else 
			p1=TBDcn(t1[j]->p,tadlCur->p);
	if(!TBDmp(p1)){
                POunion(j,p1);
                tadlTail->next=(TADL *)malloc(sizeof(TADL));
				tadlTail->next->no=j;
				tadlTail->next->p=p1;
				tadlTail->next->next=NULL;
				tadlTail=tadlTail->next;
            }
			p1=TBDcn(tadlCur->p,TBDng(t1[j]->p));
			if(!TBDmp(p1)){  
                p2=getPCTBD(j);
                mpSame=TBDmp(TBDcn(p1,p2));
                mpDiff=TBDmp(TBDcn(p1,TBDng(p2)));
                
                for (i=0;t1[i]!=NULL;i++) if(tceSL[i]==0){   /*add (t_i) to TCEDL*/
                    if((!mpSame&&POen[i][j])){/*||(!mpDiff&&POpredecessor[i][j])){*/
				        tcedlTail->next=(TCEDL *)malloc(sizeof(TCEDL));
				        tcedlTail->next->no=i;
				        tcedlTail->next->next=NULL;
				        tcedlTail=tcedlTail->next;
				        tceSL[i]=1;
                    }
	            }
			}	
            free(tcedlHead);  /*free the nodes that have been checked*/
			tcedlHead=tcedlCur;	
		}
		free(tcedlHead);  /*free the last node*/
		free(tadlHead);  /*free the nodes that have been checked*/
		tadlHead=tadlCur;
		/*freetrspo(te1);*/ 
	}
	free(tadlHead);  /*free the last node*/
	free(tceSL);   /*free the static list*/
	/*freetrspo(te);*/ return ta;
}
/************************************************************************/
TBD *TBDpost(TBD *p0,TBDtrspo **ta)
{                
        int k;
        int n=ggt->booln;
        TBDtrs **t1=ggt->trs; 
        TBD *p1;        
        TBD *r=TBDomg;
        
        for (k=0;t1[k]!=NULL;k++) if (t1[k]->p!=TBDomg&&ta[k]->trok) {
        	p1=TBDcn(p0,t1[k]->p);
		if (ta[k]->p!=NULL) p1=TBDcn(p1,ta[k]->p); 
		if (t1[k]->pc!=NULL&&!TBDmp(p1)) p1=pc2tbd(ggt,t1[k]->pc,p1); 
/*
                r=TBDds(r,TBDsp(p1,t1[k]->tc,t1[k]->uclist));
*/
                r=TBDds(r,TBDrn(TBDrp(p1,t1[k]->t,1,1)));
        }
	return r;
}
/************************************************************************/
void freetrspo(TBDtrspo **a)
{
	int k=ggt->trscnt-1;
	for (;k;k--) free(a[k]); free(a[0]); free(a);
}
/************************************************************************/
void POread(char **pod,char *fn,int n)
{
	int i,j;
	FILE *ff;
	char *ll,*line;
	int size=(ggt->trscnt)*6;

	for (i=0;i<ggt->trscnt;i++) for (j=0;j<ggt->trscnt;j++) pod[i][j]=0;
	if ((ff=fopen(fn,"r"))==NULL) return;

	line=(char *)malloc(size*sizeof(int));
	while (1) {
		ll=line; fgets(ll,size,ff); 
		i=atoi(ll)-1; while (isdigit(*ll)) ll++; 
		for (;*ll;ll++) 
		if (isdigit(*ll)) { 
			j=atoi(ll)-1; pod[i][j]=1;
			while (isdigit(*ll)) ll++; 
		}
		if (feof(ff)) break;
	}
	fclose(ff); free(line); return;
}
/************************************************************************/
int tesize(TBDtrspo **te)
{
	int k,i;
        TBDtrs **t1=ggt->trs; 

        for (i=0,k=0;t1[k]!=NULL;k++) if (te[k]->trok) i++;
	return i;
}
/************************************************************************/

/************************************************************************/
#define ggbspec ggb->pr->prog->right->right->right->right
#define ggbtrs  ggb->pr->prog->right->right->left
/************************************************************************/
GLOBAL char *atomicTrs;
GLOBAL char *POvisibleTrs;
GLOBAL char **POdependent;
GLOBAL char **POenable;
GLOBAL char **POpredecessor;
/************************************************************************/
char *getAtomicTrs(ggbase *ggb);
char *getRelatedTrs(ggbase *ggb,char *atomicTrs,int a);
char *getVisibleTrs(ggbase *ggb,char *atomicTrs,char **relatedTrs,tree *sp);
/************************************************************************/
void invPOinit(ggbase *ggb)
{
	int i,j;
	int n=getTrscount(ggbtrs);
        char **relatedTrs=(char **)malloc(getTrscount(ggbtrs)*sizeof(char *));

	atomicTrs=getAtomicTrs(ggb);
/*
	for (i=0;i<n;i++) { infos("A"); infoi(i); infoi(atomicTrs[i]); }
*/
	for (i=0;i<n;i++) relatedTrs[i]=getRelatedTrs(ggb,atomicTrs,i);
        POvisibleTrs=getVisibleTrs(ggb,atomicTrs,relatedTrs,ggbspec);
/*
	for (i=0;i<n;i++) { infos("V"); infoi(i); infoi(POvisibleTrs[i]); }
*/
        POdependent=(char **)malloc(getTrscount(ggbtrs)*sizeof(char *));
	for (i=0;i<n;i++) {
		POdependent[i]=(char *)calloc(n,sizeof(char));
		for (j=0;j<n;j++) {
		    POdependent[i][j]=dependent(ggb,atomicTrs,relatedTrs,i,j);
/*
		    if (POdependent[i][j]) { infos("D"); infoi(i+1); infoi(j+1); }
*/
		}
	}

        POenable=(char **)malloc(getTrscount(ggbtrs)*sizeof(char *));
	for (i=0;i<n;i++) {
		POenable[i]=(char *)calloc(n,sizeof(char));
		for (j=0;j<n;j++) {
			POenable[i][j]=enable(ggb,atomicTrs,relatedTrs,i,j);
/*
		    	if (POenable[i][j]) { infos("E"); infoi(i+1); infoi(j+1); }
*/
		}
	}

    POpredecessor=(char **)malloc(getTrscount(ggbtrs)*sizeof(char *));
	for (i=0;i<n;i++) {
		POpredecessor[i]=(char *)calloc(n,sizeof(char));
		for (j=0;j<n;j++) {
			POpredecessor[i][j]=predecessor(atomicTrs,relatedTrs,i,j);
/*
		    	if (POpredecessor[i][j]) { infos("P"); infoi(i+1); infoi(j+1); }
*/
		}
	}

	for (i=0;i<n;i++) free(relatedTrs[i]); free(relatedTrs); 
/*
	treewrite(ggbspec,stdout);
	treewrite(ggbtrs,stdout);
*/
}
/************************************************************************/
char *getAtomicTrs(ggbase *ggb)
{
	tree *l0;
	int lc=0;
        char *vv=(char *)calloc(getTrscount(ggbtrs),sizeof(char));
	tree *trs=ggbtrs;

        while ((l0=listgetone(trs,lc++))!=NULL) vv[lc-1]=POatomic(l0->left);
	return vv;
}
/************************************************************************/
char *getRelatedTrs(ggbase *ggb,char *atomicTrs,int a)
{
	tree *l0;
	int lc=0;
	int n=getTrscount(ggbtrs);
        char *vv=(char *)calloc(getTrscount(ggbtrs),sizeof(char));
	tree *trs=ggbtrs;
	char pc[64],pc1[64];
	int i,new;

	if (atomicTrs[a]==0) return vv; 
	vv[a]=1; new=1;
	while (new) { 
	      new=0;
	      for (i=0;i<=n;i++) if (vv[i]) {
			*pc=0; getPCWrite(listget1n(trs,i+1)->right,pc); lc=0;
       			while ((l0=listgetone(trs,lc++))!=NULL) {
				if (atomicTrs[lc-1]) continue;
				if (vv[lc-1]) continue;
				*pc1=0; getPCWrite(l0->left,pc1);
				if (strcmp(pc,pc1)==0) { vv[lc-1]=1; new=1; }
			}
	      }
	}
	return vv;
}
/***********************************************************************
int strIsPC(char *str)
{
    int l=strlen(str);
    
    if (l==2&&strcmp(str,"pc")==0) return 1;
	else if (l>2&&strcmp(str+l-3,".pc")==0) return 1;
    return 0;
}
************************************************************************/
void getPC(tree *t,char *pc)
{
	/*int l;*/
	if (t==NULL) return;
	/*l=strlen(t->node);*/
  
	/*if (l==2&&strcmp(t->node+l-2,"pc")==0) strcpy(pc,t->node); 
	else if (l>2&&strcmp(t->node+l-3,".pc")==0) strcpy(pc,t->node); */
    if (strIsPC(t->node))strcpy(pc,t->node); 
	else { getPC(t->left,pc); getPC(t->right,pc); }
}
/************************************************************************
void getPCWrite(tree *t,char *pc)
{

	if (t==NULL) return;
    
    if (t->node[0]=='=') {
		if(strIsPC(t->left->node)){strcpy(pc,t->left->node);strcat(pc,"=");strcat(pc,t->right->node);return;} 
	}
	else { getPCWrite(t->left,pc); getPCWrite(t->right,pc); }
}
************************************************************************/
int POatomic(tree *t)
{
	if (t==NULL) return 0;
	if (t->node[0]=='=') {
		if (strcmp(t->left->node,"vvmtoken")==0
		   &&strcmp(t->right->node,"0")==0) return 1; 
		return 0;
	}
	return POatomic(t->left)||POatomic(t->right); 
}
/************************************************************************/
int getTrscount(tree *trs)
{
	tree *l0;
	int lc=0;
        while ((l0=listgetone(trs,lc++))!=NULL); return lc-1;
}
/************************************************************************/
int dependentvar(tree *t,char **vv,int k)
{
	if (t==NULL) return k;
	if (t->left==NULL&&t->right==NULL) {
		if (strin(t->node,vv,k)) return k;
        if (strcmp(t->node,"vvmtoken")==0) return k;
        if (47<t->node[0]&&t->node[0]<58) return k;
        if (strIsPC(t->node)) return k;
		strcpy(vv[k],t->node); 
        return k+1;
	}
	k=dependentvar(t->left,vv,k);
	return dependentvar(t->right,vv,k);
}
/************************************************************************/
int dependentassign(tree *t,char **vv,int k)
{
	if (t==NULL) return k;
	if (t->node[0]=='=') return dependentvar(t,vv,k);
	k=dependentassign(t->left,vv,k);
	return dependentassign(t->right,vv,k);
}
/************************************************************************/
int writevarPO(tree *t,char **vv,int k)
{
	if (t==NULL) return k;
	if (t->node[0]=='=') {
		if (strin(t->left->node,vv,k)) return k;
        if (strcmp(t->left->node,"vvmtoken")==0) return k;
        if (47<t->node[0]&&t->left->node[0]<58) return k;
        if (strIsPC(t->left->node)) return k;
		strcpy(vv[k],t->left->node); 
        return k+1;
	}
	k=writevarPO(t->left,vv,k);
	return writevarPO(t->right,vv,k);
}
/************************************************************************/
int strin(char *v0,char **vv,int k)
{
	int i;
	for (i=0;i<k;i++) if (strcmp(v0,vv[i])==0) return 1;
	return 0;
}
/************************************************************************/
int emptyintersection(char **v0,int k0,char **v1,int k1)
{
	int i,j;
	for (i=0;i<k0;i++) 
	for (j=0;j<k1;j++) if (strcmp(v0[i],v1[j])==0) return 0;
	return 1;
}
/************************************************************************/
char *getVisibleTrs(ggbase *ggb,char *atomicTrs,char **relatedTrs,tree *sp)
{
	int m=getTrscount(ggb->pr->prog->left);
        char **dv=(char **)calloc(m,sizeof(char*));
        char **wv=(char **)calloc(m,sizeof(char*));
	int n=getTrscount(ggbtrs);
        char *visibleTrs=(char *)calloc(getTrscount(ggbtrs),sizeof(char));
	tree *trs=ggbtrs;
	int i,j;
	int ndv,nwv;

	for (i=0;i<m;i++) dv[i]=(char *)malloc(64*sizeof(char));
	for (i=0;i<m;i++) wv[i]=(char *)malloc(64*sizeof(char));

	ndv=dependentvar(sp,dv,0);
	for (i=0;i<n;i++) if (atomicTrs[i]) {
		for (j=0;j<n;j++) if (relatedTrs[i][j]) {
			nwv=writevarPO(listget1n(trs,j+1)->right,wv,0);
			if (emptyintersection(dv,ndv,wv,nwv)==0) {
				visibleTrs[i]=1;
				break;
			}
		}
	}
	for (i=0;i<m;i++) { free(dv[i]); free(wv[i]); } free(dv); free(wv);
	return visibleTrs;
}
/************************************************************************/
int sameProcess(tree *trs, int a, int b)
{
    char pca[64], pcb[64];
    tree *trsa=listget1n(trs,a+1);
    tree *trsb=listget1n(trs,b+1);

    
    getPC(trsa,pca);
    getPC(trsb,pcb);
    
    if (strcmp(pca,pcb)==0) return 1;
    return 0;
}

/************************************************************************/
int dependent(ggbase *ggb,char *atomicTrs,char **relatedTrs,int a,int b)
{
	int m=getTrscount(ggb->pr->prog->left);
        char **dv=(char **)calloc(m,sizeof(char*));
        char **wv=(char **)calloc(m,sizeof(char*));
	int n=getTrscount(ggbtrs);
        char *visibleTrs=(char *)calloc(getTrscount(ggbtrs),sizeof(char));
	tree *trs=ggbtrs;
	int i,j;
	int ndv=0,nwv=0;
    int k;

	for (i=0;i<m;i++) dv[i]=(char *)malloc(64*sizeof(char));
	for (i=0;i<m;i++) wv[i]=(char *)malloc(64*sizeof(char));

	if (!(atomicTrs[a]&&atomicTrs[b])) return 0;
    if (sameProcess(trs,a,b)) return 0;
	for (j=0;j<n;j++) if (relatedTrs[a][j]) {
		nwv=writevarPO(listget1n(trs,j+1)->right,wv,0);
		for (i=0;i<n;i++) if (relatedTrs[b][i]) {
            if(b!=i)
            ndv=dependentvar(listget1n(trs,i+1)->left,dv,0);
			ndv=dependentvar(listget1n(trs,i+1)->right,dv,ndv);
			if (emptyintersection(dv,ndv,wv,nwv)==0) return 1;
		}
	}
	for (i=0;i<m;i++) { free(dv[i]); free(wv[i]); } free(dv); free(wv);
	return 0;
}
/************************************************************************/
int enable(ggbase *ggb,char *atomicTrs,char **relatedTrs,int a,int b)
{
	int m=getTrscount(ggb->pr->prog->left);
        char **dv=(char **)calloc(m,sizeof(char*));
        char **wv=(char **)calloc(m,sizeof(char*));
	int n=getTrscount(ggbtrs);
        char *visibleTrs=(char *)calloc(getTrscount(ggbtrs),sizeof(char));
	tree *trs=ggbtrs;
	int i,j;
	int ndv,nwv;

	for (i=0;i<m;i++) dv[i]=(char *)malloc(64*sizeof(char));
	for (i=0;i<m;i++) wv[i]=(char *)malloc(64*sizeof(char));

	if (!(atomicTrs[a]&&atomicTrs[b])) return 0;
    if (sameProcess(trs,a,b)) return 0;
    ndv=dependentvar(listget1n(trs,b+1)->left,dv,0);
    
	for (j=0;j<n;j++) if (relatedTrs[a][j]) {
		nwv=writevarPO(listget1n(trs,j+1)->right,wv,0);
       
		if (emptyintersection(dv,ndv,wv,nwv)==0) return 1;
	}
	for (i=0;i<m;i++) { free(dv[i]); free(wv[i]); } free(dv); free(wv);
	return 0;
}
/************************************************************************/
int predecessor(char *atomicTrs,char **relatedTrs,int a,int b)
{
	tree *trs=ggbtrs;
	int n=getTrscount(ggbtrs);
	char pca[64],pcb[64];
	int i;

    if (!(atomicTrs[a]&&atomicTrs[b])) return 0;
    if (!sameProcess(trs,a,b)) return 0;
    getPCWrite(listget1n(trs,b+1)->left,pcb); 
	for (i=0;i<=n;i++) if (relatedTrs[a][i]) {
	   getPCWrite(listget1n(trs,i+1)->right,pca); 
	   if (strcmp(pca,pcb)==0)return 1;
	}

	return 0;
}
/********************************************************************/
int strIsPC(char *str)
{
    int l=strlen(str);

    if (l==2&&strcmp(str,"pc")==0) return 1;
        else if (l>2&&strcmp(str+l-3,".pc")==0) return 1;
    return 0;
}
/***********************************************************************/
void getPCWrite(tree *t,char *pc)
{

        if (t==NULL) return;

        if (t->node[0]=='=') {
                if(strIsPC(t->left->node)) {
                        strcpy(pc,t->left->node);
                        strcat(pc,"=");
                        strcat(pc,t->right->node); return;
                }
        }
        else { getPCWrite(t->left,pc); getPCWrite(t->right,pc); }
}
/************************************************************************/
