#include "cex.h"
#include "mod.h"
#include "tbdbw.h"



/********************************************************************/
LOCAL extern MODoption *MODopt;
/********************************************************************/
void CEXwrite(CEXtype *ce,tree *vlist,char *fn)
{
	int i;
	int *xx;
	char fn1[128];
	FILE *cexlog;

	if (ce==NULL) return;

	gfilename(fn,fn1); strcat(fn1,".cex"); cexlog=fopen(fn1,"w");

	xx=(int *)malloc((ce->cf+ce->cb)*sizeof(int));
	CEXselect(ce,xx);
	if (MODopt->clname[0]!=0) fprintf(cexlog,"%s\t=%i\n",MODopt->clname,0);
	CEXwriteState(ce->fw,0,cexlog,vlist); 
	for (i=1;i<ce->cf;i++) { 
		CEXwriteTr(xx,i-1,cexlog); 
		CEXwriteState(ce->fw,i,cexlog,vlist); 
	}
	free(xx);
	fclose(cexlog);
}
/********************************************************************/
void CEXselect(CEXtype *ce,int *xx)
{
	int i,j,k,x;
	int cf=ce->cf;
	int cb=ce->cb;
	TBD **fw1=ce->fw;
	TBD **bw1=ce->bw;

	if (cb==0) return;
	for (i=cf-1;i>=0;i--) { TBDdr(fw1[i]); }
	for (i=cb-1;i>=0;i--) { TBDdr(bw1[i]); }

	for (i=cf-1;i>=0;i--) { if (!TBDmp(TBDcn(fw1[i],bw1[cb-1]))) break; }
	if (i<0) { ce->cf=ce->cb=0; return; }

/*
 infos("JDSceselect");
 infoi(cf);
 infoi(cb);
 TBDwrite(fw1[0],10,stdout);
 TBDwrite(bw1[0],10,stdout);
*/

	j=cb-1;
	fw1[i]=TBDone(TBDcn(fw1[i],bw1[j]));
	for (k=i-1;k>=0;k--) { 
		fw1[k]=TBDbwSelectOne(fw1[k+1],fw1[k],&x); xx[k]=x+1;
	}

	bw1[j]=fw1[i];
	for (k=j-1;k>=0;k--) { 
		bw1[k]=TBDfwSelectOne(bw1[k+1],bw1[k],&x); xx[i+(j-1-k)]=x+1;
	} 
	
	for (k=0;k<=i;k++) { TBDkp(fw1[k]); }
	for (x=i+1,k=j-1;k>=0;k--,x++) { fw1[x]=bw1[k]; TBDkp(fw1[x]); }
	ce->cf=x; ce->cb=0;
}
/********************************************************************/
void CEXwriteTr(int *xx,int i,FILE *f)
{
	static clocktr=0;
        fprintf(f,"--- TRANS %i ---\n",xx[i]);
	if (MODopt->clname[0]!=0) {
		if (xx[i]>MODopt->trnumber) clocktr++;
		fprintf(f,"%s	=%i\n",MODopt->clname,clocktr);
	}
}
/********************************************************************/
void CEXwriteState(TBD **fw1,int k,FILE *f,tree *vlist)
{
	int *xs;
	tree *t0;
	int i,j,x,lc=0;
	char node[MAXVARLEN];

        fprintf(f,"--- STATE %i ---\n",k);
	xs=TBDwriteOne(fw1[k],ggt->booln,k,f); 
	while ((t0=listgetone(vlist,lc++))!=NULL) {
		t0=t0->left;
		i=atoi(t0->left->node); 
		j=atoi(t0->right->node);
		for (x=0;i<=j;i++) x=2*x+(xs[i-1]==1);
		fprintf(f,"%s	=%i\n",CEXrename(t0->node,node),x);
	}
	free(xs);
}
/********************************************************************/
char *CEXrename(char *s0,char *s1)
{
	int a;
	char *ll=s0;
	char *l2=s1;

	while (*ll) {
		if (strncmp(ll,"_x_",3)!=0) { *l2++=*ll++; continue; }
		ll+=3; a=atoi(ll); while (isdigit(*ll)) ll++;
		sprintf(l2,"[%i]",a); l2=s1+strlen(s1);
	}
	*l2=0; return s1; 
}
/********************************************************************/
