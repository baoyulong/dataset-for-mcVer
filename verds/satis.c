#include "satis.h"

/************************************************************************/
int SATapply(model *mm)
{
	CNFmodelclean(mm); return issat(mm);
}
/************************************************************************/

/************************************************************************/
LOCAL typedef struct _bstruct {
	int **binMap;
	int **binMapClause;
	int *binCount;
	int *binCountTmp;
} bstruct;
/**********************************************************************/
LOCAL bstruct *bstr;
/**********************************************************************/
int bin_init(int nn,int *ll);
int bin_close(int nn);
int bin_update(int x,int y,int c);
int bin_new(int nn);
int bin_backtrack(int nn);
int *bin_use1(int c);
int *bin_use2(int c);
void bin_sum_init(int nn,char *gim,int *out);
void bin_sum(int nn,char *gim,int *in,int *out,int k);
/**********************************************************************/
int bin_init(int nn,int *ll)
{
	int i;
        bstr=(bstruct *)malloc(sizeof(bstruct));
        bstr->binMap=(int **)calloc(2*nn+1,sizeof(int))+nn;
        bstr->binMapClause=(int **)calloc(2*nn+1,sizeof(int))+nn;
        bstr->binCount=(int *)calloc(2*nn+1,sizeof(int))+nn;
        bstr->binCountTmp=(int *)calloc(2*nn+1,sizeof(int))+nn;

	for (i=1;i<=nn;i++) { 
		bstr->binMap[-i] =(int *)malloc((ll[-i]+1)*sizeof(int));
		bstr->binMap[ i] =(int *)malloc((ll[ i]+1)*sizeof(int));
		bstr->binMapClause[-i] =(int *)malloc((ll[-i]+1)*sizeof(int));
		bstr->binMapClause[ i] =(int *)malloc((ll[ i]+1)*sizeof(int));
		*bstr->binMap[ i] =*bstr->binMap[-i] =0;
		bstr->binCount[i] =bstr->binCount[-i] =0;
	}
}
int bin_close(int nn)
{
	int i;

	for (i=1;i<=nn;i++) { free(bstr->binMap[-i]); free(bstr->binMap[ i]); }
	free(bstr->binMap-nn); free(bstr->binMapClause-nn); 
	free(bstr->binCount-nn); free(bstr->binCountTmp-nn); 
	free(bstr); 
}
/**********************************************************************/
int bin_new(int nn)
{
	memcpy(bstr->binCountTmp-nn,bstr->binCount-nn,nn*sizeof(int)); 
}
int bin_backtrack(int nn)
{
	int i;
	memcpy(bstr->binCount-nn,bstr->binCountTmp-nn,nn*sizeof(int)); 
	for (i=1;i<=nn;i++) { 
		bstr->binMap[i][bstr->binCount[i]]=0;
		bstr->binMap[-i][bstr->binCount[-i]]=0;
	}
}
/**********************************************************************/
int bin_update(int x,int y,int c)
{
        if (bin_update_local(x,y,c)) return y;
        if (bin_update_local(y,x,c)) return x;
        return 0;
}
/**********************************************************************/
int bin_update_local(int x,int y,int c)
{
	int *l=bstr->binMap[y];

	while (*l) { if (x==*l) return 0; else if (-x==*l++) return 1; }
	*(bstr->binMapClause[y]+bstr->binCount[y])=c; 
	*l++ =x; *l=0; bstr->binCount[y]++; 

	if (l-bstr->binMap[y]!=bstr->binCount[y]) error("bin");
	return 0;
}
/**********************************************************************/
int *bin_use(int c)
{
	return bstr->binMap[c];
}
int *bin_use2(int c)
{
	return bstr->binMapClause[c];
}
/**********************************************************************/
void bin_sum_init(int nn,char *gim,int *out)
{
	int i;
	int *la=out+1,*lb=out-2;
	int *l1,*l2;

        for (i=1;i<=nn;i++,la++,lb--) if (!(gim[i]&1)) {
		*la+=bstr->binCount[-i];
		*lb+=bstr->binCount[ i];
	}
}
/**********************************************************************/
void bin_sum(int nn,char *gim,int *in,int *out,int k)
{
	int i;
	int *la=out+1,*lb=out-2;
	int *l1,*l2;

	if (k>1) {
		bin_sum(nn,gim,in,out,1);
		bin_sum(nn,gim,out,in,k-1); return;
	}

        for (i=1;i<=nn;i++,la++,lb--) if (!(gim[i]&1)) {
                for (l1=bstr->binMap[i],*la=0;*l1;l1++)
                if(!(gim[*l1]&1)) *la+= in[-*l1];
                for (l1=bstr->binMap[-i],*lb=0;*l1;l1++)
                if(!(gim[*l1]&1)) *lb+= in[-*l1];
	}
}
/**********************************************************************/

/************************************************************************/
LOCAL int *io;
LOCAL int cflag=1;
LOCAL int STAT_depth=-1;
LOCAL int STAT_branch=0;
LOCAL int STAT_learned=0;
/************************************************************************/
int issat1(model *mm)
{
	mstruct *cc;
	mdata *wc;
	int *unitcl;
	int *inac=NULL;
	int i=1;
	int j;
	FILE *g;
	

	cc=model2mstruct(mm); wc=create_mdata(mm);
	g=fopen(ISSATLOG,"w"); fprintf(g,"NN\n%i 0\n",mm->n); fclose(g);

/****** not compatible with couter-example generation ******************
	if (modelcleaneq(mm,cc,wc,cc->CL)) {
		freemdata(cc,wc,mm->n);
		cc=model2mstruct(mm); wc=create_mdata(mm);
	}
***********************************************************************/

	unitcl=unitclauses(cc,wc,cc->CL,mm->m,wc->InactiveList);
	if (*unitcl) { 
		i=remove_red2(unitcl);
		if (i==1) { 
			i=unitp1(cc,wc,cc->CL,unitcl);
			writelog("IL",wc->InactiveList);
			j=intlen(wc->InactiveList)+1;
			inac=(int *)malloc(j*sizeof(int));
			for (j=0;wc->InactiveList[j];j++) 
				inac[j]=wc->InactiveList[j];
			inac[j]=0;
			
			modelcompress(mm,cc,wc,cc->CL); 
			freemdata(cc,wc,mm->n);
			cc=model2mstruct(mm); wc=create_mdata(mm);
		}
	}

	if (i==1) {
		imp_init(mm->n);
		STAT_depth=-1; STAT_branch=0; STAT_learned=0;
		i=startbranching(cc,wc,mm->n,mm->m,cc->CL);
		imp_close(mm->n);
	}
	if (i==1) { 
		cleaninactive(wc->InactiveList,mm->n);
		writelog("IL",wc->InactiveList); 
		if (inac!=NULL) for (j=0;inac[j];j++) {
			wc->InactiveMatrix[inac[j]]=1;
			wc->InactiveMatrix[-inac[j]]=3;
		}
		g=fopen("logsat.cex","w"); 
		fprintf(g,"SAT\n"); 
		for (j=1;j<=mm->n;j++) {
			if (wc->InactiveMatrix[j]==1) fprintf(g,"%i ",j); 
			else fprintf(g,"-%i ",j); 
		}
		fprintf(g,"0\n"); 
		fclose(g);
	}

	if (inac!=NULL) free(inac); freemdata(cc,wc,mm->n);
	return i;
}
/************************************************************************/
int startbranching(mstruct *cc,mdata *wc,int n,int m,int *ccCL)
{
	int i,j;
	int *lcl;
	int *cur=wc->Current;

	lcl=(int *)malloc(m*sizeof(int)); memcpy(lcl,ccCL,m*sizeof(int));
	io=initorder(cc,lcl,wc->InactiveMatrix,n);
	i= branching(cc,wc,lcl,n,m); 
	free(lcl);

	return i;
}
/************************************************************************/
int branching(mstruct *cc,mdata *wc,int *cl,int n,int m)
{
	int i,c;
	int *lcl;
	int *cur;

	c=imp_use(cc,wc); if (c==0) return 0;
	if (c!=1) {
		cur=wc->Current; 
		wc->Current[0]=c; wc->Current[1]=0;
	  	i=unitp(cc,wc,cl,wc->Current); 
		cflag=c;
		if (i==1) { i=branching(cc,wc,cl,n,m); if (i==1) return 1; }
		return 0;
	}

	c=select_v_b(cc,cl,wc,n);

	if (c==0) return 1;

 	STAT_depth++; 
	lcl=(int *)malloc(m*sizeof(int)); memcpy(lcl,cl,m*sizeof(int));
	cur=wc->Current; wc->Current[0]=c; wc->Current[1]=0;
	wc->Literals[STAT_depth]=c; wc->Literals[STAT_depth+1]=0;
/*
 if (STAT_depth>=10) { prs("SSSS"); pri(wc->Literals[10]); }
 if (wc->Literals[16]==-86) writeintlist(wc->Literals,0,intlen(wc->Literals),stdout); 
*/

	i=unitp(cc,wc,lcl,cur); 
	cflag=c;
	if (i==1) { i=branching(cc,wc,lcl,n,m); if (i==1) return 1; }
	imp_backtrack(cur); backtrack(wc,lcl,cur); 

	cur=wc->Current; wc->Current[0]=-c; wc->Current[1]=0;
	wc->Literals[STAT_depth]=-c; wc->Literals[STAT_depth+1]=0;

/*
 if (STAT_depth>=10) { prs("SSSS"); pri(wc->Literals[10]); }
 if (wc->Literals[10]==-86) writeintlist(wc->Literals,0,intlen(wc->Literals),stdout); 
*/

	i=unitp(cc,wc,cl,cur); 
	cflag=c;
	if (i==1) { i=branching(cc,wc,cl,n,m); if (i==1) return 1; }

 	STAT_depth--; STAT_branch++; return 0; 
}
/**********************************************************************/
void backtrack(mdata *wc,int *lcl,int *cur)
{
	free(lcl);
        for (wc->Current=cur;*cur;cur++) {
                if (wc->InactiveMatrix[*cur]) {
                        wc->InactiveMatrix[*cur]=wc->InactiveMatrix[-*cur]=0; 
                }
        } 
	*wc->Current=0; 
}
/**********************************************************************/
int select_v_a(mstruct *cc,int *cl,char *gim,int n)
{
	int i;
	for (i=1;i<=n;i++) if (gim[i]==0) return i; return 0;
}
/**********************************************************************/
int select_v_b(mstruct *cc,int *cl,mdata *wc,int n)
{
	int i,j,c;
	char *gim=wc->InactiveMatrix;
	int l1,l2;
	float k,k0;

	for (k0=0.0,c=0,j=1;j<=n;j++) if (gim[io[j]]==0) {
		i=io[j];
		if ((l1=lvalue_b(cc,cl,i))==0) { 
			wc->InactiveMatrix[i]=1; 
			wc->InactiveMatrix[-i]=3; *wc->Current++=i;
			continue; /* gflag0=1; return -i; */
		}
		if ((l2=lvalue_b(cc,cl,-i))==0) { 
			wc->InactiveMatrix[-i]=1; 
			wc->InactiveMatrix[i]=3; *wc->Current++=-i;
			continue; /* gflag0=1; return i; */
		}
  return i;
		k=l1*l2;
		if (k>k0) { k0=k; c=i; }
	}
	return c;
}
/**********************************************************************/
int select_v(mstruct *cc,int *cl,char *gim,int n)
{
        int i;
	for (i=1;i<=n;i++) if (gim[io[i]]==0) return io[i]; return 0;
}
/************************************************************************/
int issat(model *mm)
{
	mstruct *cc;
	mdata *wc;
	int *unitcl;
	int i=1;
	int j,k,k1,x;
	FILE *g;
	int *pcl,*p1;
	model *m1;

	return issat1(mm);

	cc=model2mstruct(mm); wc=create_mdata(mm);
	g=fopen(ISSATLOG,"w"); fprintf(g,"NN\n%i 0\n",mm->n); fclose(g);

/****** not compatible with couter-example generation ******************
	if (modelcleaneq(mm,cc,wc,cc->CL)) {
		freemdata(cc,wc,mm->n);
		cc=model2mstruct(mm); wc=create_mdata(mm);
	}
***********************************************************************/

	unitcl=unitclauses(cc,wc,cc->CL,mm->m,wc->InactiveList);
	if (*unitcl) { 
		i=remove_red2(unitcl);
		if (i==1) { 
			i=unitp1(cc,wc,cc->CL,unitcl);
			writelog("IL",wc->InactiveList);
			modelcompress(mm,cc,wc,cc->CL); 
			freemdata(cc,wc,mm->n);
			cc=model2mstruct(mm); wc=create_mdata(mm);
		}
	}

	if (i==0) { return 0; }

	k=ispart(cc,mm->m,mm->n); if (k==-1) return issat1(mm);

	k1=cc->CL[k];

	pcl=(int *)malloc((k1+1)*sizeof(int));
	for (x=0;x<=k1;x++) pcl[x]=-cc->Clause[k][x];

	freemdata(cc,wc,mm->n);

	for (j=0;j<k1;j++) {
		m1=(model *)malloc(sizeof(model)); 
		m1->m=mm->m+k1; m1->n=mm->n; m1->l=mm->l+k1*2;
		m1->mstring=(int *)malloc((m1->l)*sizeof(int));
		memcpy(m1->mstring,mm->mstring,mm->l*sizeof(int));
		p1=m1->mstring+mm->l;
		for (x=0;x<k1;x++) { 
			if (x==j) *p1++=-pcl[x]; else *p1++=pcl[x]; 
			*p1++=0; 
		}
		i=issat1(m1); 
		free(m1->mstring); free(m1); 
		if (i==1) break; 
	}
	free(pcl); 
	return i;
}
/************************************************************************/
int select_v_c(mstruct *cc,int *cl,mdata *wc,int n)
{
	int i,j,c;
	char *gim=wc->InactiveMatrix;
	int l1,l2;
	float k,k0;

	for (k0=0.0,c=0,j=1;j<=n;j++) if (gim[io[j]]==0) {
		i=io[j];
		if ((l1=lvalue_c(cc,cl,i))==0) { 
			wc->InactiveMatrix[i]=1; 
			wc->InactiveMatrix[-i]=3; *wc->Current++=i;
			continue; /* gflag0=1; return -i; */
		}
		if ((l2=lvalue_c(cc,cl,-i))==0) { 
			wc->InactiveMatrix[-i]=1; 
			wc->InactiveMatrix[i]=3; *wc->Current++=-i;
			continue; /* gflag0=1; return i; */
		}
		k=l1*l2;
		if (k>k0) { k0=k; c=i; }
	}
	return c;
}
int cleaninactive(int *i1,int n)
{
	int i;

	for (i=0;i<n;i++) { if (i1[i]==0||i1[i]>n||i1[i]<-n) break; }
	i1[i]=0;
}

/************************************************************************/
mstruct *model2mstruct(model *mm)
{
	int i,k;
        int *p0,*p1;
	int *pos;
	mstruct *ss=(mstruct *)malloc(sizeof(mstruct));

	ss->Clause          =(int **)malloc((mm->m+1)*sizeof(int*));
        ss->CL              =(int *)malloc((mm->m)*sizeof(int)); 
        ss->LiteralMatrix   =(int *)malloc((mm->l)*sizeof(int)); 
        ss->LS              =(int *)malloc((mm->n+1+mm->n)*sizeof(int))+mm->n; 
        ss->LL              =(int *)malloc((mm->n+1+mm->n)*sizeof(int))+mm->n; 

	for (i=0;i<mm->m;i++) ss->CL[i]=0; 
	for (p0=mm->mstring,p1=p0+mm->l,i=0;p0<p1;p0++) 
	if (*p0) ss->CL[i]++; else i++;

	for (p0=mm->mstring,ss->Clause[0]=p0,i=0;i<mm->m;i++) 
	ss->Clause[i+1]=ss->Clause[i]+ss->CL[i]+1;

	for (i=1;i<=mm->n;i++) ss->LL[i]=ss->LL[-i]=0; 
	for (p0=mm->mstring,p1=p0+mm->l;p0<p1;p0++) ss->LL[*p0]++;

	for (k=0,i=1;i<=mm->n;i++) {
		ss->LS[i]=k; k+=ss->LL[i]; ss->LS[-i]=k; k+=ss->LL[-i]; 
		ss->LL[i]=ss->LL[-i]=0;
	}
        for (p0=mm->mstring,i=0;i<mm->m;i++) for (k=0;k<=ss->CL[i];k++,p0++) 
	if (*p0) { pos=&ss->LiteralMatrix[ss->LS[*p0]+ss->LL[*p0]++]; *pos=i; }

	return ss;
}
/****************************************************************************/
mdata *create_mdata(model *mm)
{
	mdata *ss=(mdata *)malloc(sizeof(mdata));

        ss->InactiveMatrix=((char *)calloc((mm->n+1+mm->n),sizeof(char)))+mm->n;
	ss->InactiveMatrix[0]=1;
        ss->InactiveList=(int *)malloc((mm->m+mm->n+1)*sizeof(int));
        ss->Literals=(int *)malloc((mm->n+1)*sizeof(int));
	ss->InactiveList[0]=0;
        ss->Current=ss->InactiveList; 
	return ss;
}

void freemdata(mstruct *cc,mdata *wc,int n)
{
	free(cc->Clause); free(cc->CL); 
        free(cc->LiteralMatrix); free(cc->LS-n); free(cc->LL-n); free(cc);
	if (wc==NULL) return;
	free(wc->InactiveList); free(wc->Literals); 
	free(wc->InactiveMatrix-n); free(wc);
}



/************************************************************************/
int modelcleaneq(model *mm,mstruct *cc,mdata *mc,int *cl)
{
	int i;
	int l1,l2;
	int *p;
        int *nFnd1 =(int *)malloc((mm->n+2)*sizeof(int));
        int *nFnd2 =(int *)malloc((mm->n+2)*sizeof(int));

	nFnd1[0]=0; nFnd2[0]=0;
        for (i=0;i<mm->m;i++) if (cl[i]==2) {
          	p =cc->Clause[i];
          	l1=*p++; l2=*p;
          	if (fnd2b2(i,-l1,-l2,cc,cl)) {
			if (l1<0) { l1=-l1; l2=-l2; }
			nFnd1[++*nFnd1]=l1; nFnd2[++*nFnd2]=-l2;
	  	}
	}

	i=eqcycle(nFnd1,nFnd2);
	if (i) if (i=*nFnd1) {
		modelcompresseq(mm,cc,mc,cl,nFnd1,nFnd2);
		nFnd1[*nFnd1+1]=0; writelog("EQ",nFnd1+1); 
		nFnd1[*nFnd2+1]=0; writelog("EQ",nFnd2+1); 
	}
	free(nFnd1); free(nFnd2);
	return i;
}
int *modelsimplifybin(model *mm,mstruct *cc,mdata *mc,int *cl)
{
	int i,j,k;
	int *p;
	int l1,l2;
        int *nFnd =(int *)malloc((mm->m+1)*sizeof(int));

        for (i=0;i<mm->m;i++) if (cl[i]==2) {
          	p =cc->Clause[i];
          	l1 =*p++; l2=*p;
          	if ((k=fnd2b(i,l1,-l2,cc,cl,nFnd))) 
		for (j=1;j<=k;j++) replaceclause(nFnd[j],-l2,cc);
          	if ((k=fnd2b(i,-l1,l2,cc,cl,nFnd))) 
		for (j=1;j<=k;j++) replaceclause(nFnd[j],-l1,cc);
	}
	free(nFnd);
}
/****************************************************************************/
int fnd2b2(int i,int a,int b,mstruct *cc,int *cl)
{
        int *p1,*p2,*p0; 
        int *q1,*q2;
        
        p1=&cc->LiteralMatrix[cc->LS[a]]; 
        p0=&cc->LiteralMatrix[cc->LS[b]]; q2=p0+cc->LL[b];
                
        for (q1=p1+cc->LL[a];p1<q1;p1++) if (cl[*p1]==2&&*p1>i)
        for (p2=p0;p2<q2;p2++) if (cl[*p2]==2&&*p1==*p2) { return 1; }
        return 0; 
}
int fnd2b(int i,int a,int b,mstruct *cc,int *cl,int *nFnd)
{
        int *p1,*p2,*p0; 
        int *q1,*q2;
        
        p1=&cc->LiteralMatrix[cc->LS[a]]; 
        p0=&cc->LiteralMatrix[cc->LS[b]]; q2=p0+cc->LL[b];
                
        *nFnd=0;
        for (q1=p1+cc->LL[a];p1<q1;p1++) if (cl[*p1] && *p1>i)
        for (p2=p0;p2<q2;p2++) 
                if (cl[*p2] && *p1==*p2) { nFnd[++*nFnd]=*p2; break; }
        return *nFnd;
}
/****************************************************************************/
void replaceclause(int i,int c,mstruct *cc)
{
	int j;
	int *p1,*q1;
	int *p=cc->Clause[i];

	for (j=0;j<=cc->CL[i];j++)
	if (*p==c) { 
		*p=*(cc->Clause[i]+cc->CL[i]-1); 
		*(cc->Clause[i]+cc->CL[i]-1)=0; 

        	p1=&cc->LiteralMatrix[cc->LS[c]]; 
        	for (q1=p1+cc->LL[c];p1<q1;p1++) 
		if (*p1==i) { *p1=*(q1-1); cc->LL[c]--; break; }
	}
}

/************************************************************************/
void writelog(char *s,int *y)
{
	int i=0;
	int *x=y;
	FILE *g=fopen(ISSATLOG,"a");

	fprintf(g,"%s\n",s);
	while (*y) { fprintf(g,"%i ",*y); if (++i%10==0) fprintf(g,"\n"); y++; }
	fprintf(g,"0\n");
	fclose(g);
}
/************************************************************************/
int intinneg(int a,int *bb)
{
	int i=0; while (*bb) if (*bb==-a) return 1; else bb++; return 0;
}
int intsetincludeneg(int *aa,int *bb)
{
	int i=0; 
	while (*aa) if (intinneg(*aa,bb)==0) return 0; else aa++; return 1;
}
int intinnegremove(int a,int *bb)
{
	int i=0; 
	int k;

	for (;*bb;bb++) 
	if (*bb==-a) { 
		k=intlen(bb);
		bb[0]=bb[k-1]; bb[k-1]=0; 
		return 1; 
	} 
	return 0;
}
/************************************************************************/
int intinpos(int a,int *bb)
{
	int i=0; while (*bb) if (*bb==a) return 1; else bb++; return 0;
}
int imp_res_simple(int *aa,int *bb)
{
	int i=0; 
	int *a1;

/*
	int *pp2=aa;
 prs("YY");
 writeintlist(aa,-1,intlen(aa),stdout);
 writeintlist(bb,-1,intlen(bb),stdout);
*/
	while (*aa) if (intinpos(*aa,bb)==0) break; else aa++;
	if (*aa==0) return 0;
	if (intinneg(*aa,bb)==0) return 0;
	a1=aa; aa++;
	while (*aa) if (intinpos(*aa,bb)==0) return 0; else aa++;
	*a1=*(aa-1); *(aa-1)=0; 

/*
 prs("SS");
 writeintlist(pp2,-1,intlen(pp2),stdout);
*/
	return 1;
}


/*******************************************************************/
LOCAL typedef struct _istruct {
	int *imp;
	int *impOrder;
	int **impClause;
	int impOrderCount;
	int impNewClausePointer;
	int impClauseMax;
} istruct;
/*******************************************************************/
int imp_init(int nn);
int imp_close(int nn);
int imp_update(int x,int i);
int imp_backtrack(int *cur);
int imp_use(mstruct *cc,mdata *wc);
/*******************************************************************/
LOCAL istruct *istr;
/*******************************************************************/
extern int STAT_branch;
extern int STAT_depth;
extern int STAT_learned;
/*******************************************************************/
int imp_init(int nn)
{
	
	istr=(istruct *)malloc(sizeof(istruct)); 
        istr->imp=(int *)calloc(2*nn+1,sizeof(int))+nn;
        istr->impOrder=(int *)calloc(2*nn+1,sizeof(int))+nn;
	istr->impClause=(int **)calloc(MAXIMPCLA,sizeof(int *));
	istr->impOrderCount=0;
	istr->impClauseMax=nn+2;
	istr->impClause[0]=(int *)calloc(2,sizeof(int));
	istr->impClause[0][0]=-1;
	istr->impNewClausePointer=1;
}
int imp_close(int nn)
{
	int i;
	for (i=0;i<istr->impNewClausePointer;i++) { free(istr->impClause[i]); }
        free(istr->imp-nn); free(istr->impOrder-nn); 
	free(istr->impClause);
	free(istr);
/*
 pprti(STAT_branch);
 pprti(STAT_learned);
 pprti(istr->impNewClausePointer);
*/
}
/*******************************************************************/
int imp_update(int x,int i)
{
        istr->imp[x]=i; istr->impOrder[x]=++istr->impOrderCount; /*zwh*/
}

int imp_backtrack(int *cur)
{
	int i;

	for (i=0;i<istr->impNewClausePointer;i++) 
	if (*istr->impClause[i]>STAT_depth) {
		free(istr->impClause[i]);
		istr->impClause[i]= 
		istr->impClause[--istr->impNewClausePointer]; i--;
	}

        while (*cur) { istr->impOrder[*cur]=0; cur++; }
}
int imp_res_local(int *p1,int c1,int *p2,int c2,int p,int x)
{
	int i,j;

	p1[p]=p1[--c1]; 
	for (i=0;i<c2;i++) if (p2[i]!=x) { 
		for (j=0;j<c1;j++) if (p1[j]==p2[i]) break;
		if (j==c1) { p1[c1++]=p2[i]; }
	}
	p1[c1]=0;
	return c1;
}
int imp_res(int x,int m,mstruct *cc,mdata *wc)
{
	int y;
	int aac=cc->CL[m];
	int *aa,*aa1;

/*
    if (STAT_branch>256&&istr->impNewClausePointer>8&&STAT_learned==0) return;
*/

	if (istr->impNewClausePointer==MAXIMPCLA) return;
	aa=(int *)malloc(istr->impClauseMax*sizeof(int)); aa1=aa+1;

	memcpy(aa1,cc->Clause[m],(cc->CL[m]+1)*sizeof(int));
        while ((x=imp_getvar(aa1,aac))!=-1) {
                y=-aa1[x]; 
                if (aac+cc->CL[istr->imp[y]]>istr->impClauseMax) error("imp");
		aac=imp_res_local(aa1,aac,
			cc->Clause[istr->imp[y]],cc->CL[istr->imp[y]],x,y);
        }

	if (intlen(aa1)==0) error("aa1");

/*
 prs("RESy");
 writeintlist(aa1,0,aac,stdout);
 writeintlist(cc->Clause[istr->imp[y]],0,cc->CL[istr->imp[y]],stdout);
 writeintlist(aa1,0,aac,stdout);
 prs("RESx");
 writeintlist(aa+1,0,intlen(aa+1),stdout);
 prs("RESx");
 writeintlist(aa+1,0,intlen(aa+1),stdout);
 writeintlist(wc->Literals,0,intlen(wc->Literals),stdout);
 writeintlist(wc->InactiveList,0,intlen(wc->InactiveList),stdout);
*/

	if ((*aa=intlevel(aa+1,wc->Literals,STAT_depth))==-1
/*
		|| intsetincludeneg(aa+1,wc->Literals)==0
*/
	) {
		free(aa); return;
	}

/*
	if (intsetincludeneg(aa+1,wc->Literals)==0) {
 prs("RESx");
 writeintlist(aa+1,0,intlen(aa+1),stdout);
 writeintlist(wc->Literals,0,intlen(wc->Literals),stdout);
 writeintlist(wc->InactiveList,0,intlen(wc->InactiveList),stdout);
		error("res");
	}
*/

	istr->impClause[istr->impNewClausePointer]=
		(int *)realloc(aa,(intlen(aa1)+2)*sizeof(int));

	while (
		*istr->impClause[istr->impNewClausePointer-1]==
		*istr->impClause[istr->impNewClausePointer]&&
		intlen(istr->impClause[istr->impNewClausePointer-1]+1)==
		intlen(istr->impClause[istr->impNewClausePointer]+1)&&
		imp_res_simple(istr->impClause[istr->impNewClausePointer-1]+1,
			istr->impClause[istr->impNewClausePointer]+1))
	{
		free(istr->impClause[istr->impNewClausePointer--]);
	}

/*
 prs("RESy");
 writeintlist(aa,0,intlen(aa+1)+1,stdout);
 writeintlist(wc->Literals,0,STAT_depth+1,stdout);
*/

	istr->impNewClausePointer++; 
}
int imp_getvar(int *aa,int aac)
{
        int i,k,x;

        for (k=-1,i=x=0;i<aac;i++)
        if (istr->impOrder[-aa[i]]>x) { x=istr->impOrder[-aa[i]]; k=i; }

        return k;
}
int imp_use(mstruct *cc,mdata *wc)
{
        int i,c,k=1;
        int *p1;
	char *InactiveMatrix=wc->InactiveMatrix;

        for (i=istr->impNewClausePointer-1;i>=1;i--) {
                p1 =istr->impClause[i]+1; 
                while (*p1) { if (InactiveMatrix[-*p1]!=1) break; p1++; }
		if ((*p1)==0) { STAT_learned++; return (0); }
		c=*p1++; if (InactiveMatrix[c]) continue; 
                while (*p1) { if (InactiveMatrix[-*p1]!=1) break; p1++; }
		if ((*p1)==0) {
			STAT_learned++;
			return c;
/*
	prs("SS");
        writeintlist(wc->InactiveList,0,intlen(wc->InactiveList),stdout);nl;
			addCurrent(c); if (unitPropagation()==0) return 0;
*/
		}
	}
	return 1;
}
/******************************/
int intlevel(int *aa,int *literals,int dep)
{
	int i;
	for (i=0;i<=dep;i++) if (intinnegremove(literals[i],aa)==0) {
/*
	for (i=0;i<=dep;i++) if (intinneg(literals[i],aa)==0) {
 prs("INTx");
 writeintlist(aa,0,intlen(aa),stdout);
 writeintlist(literals,0,dep,stdout);
 prs("INTy");
 writeintlist(aa,0,intlen(aa),stdout);
 pri(i);
 if (dep>5) exit(0);
*/
		return i;
	}
	return -1;
}


/*******************************************************************/
#define isLiteral(c)     (isdigit(c)||(c=='-'))
#define isNumbers(ll)	 ((strncmp(ll,"p cnf",5)==0))
#define moveLiteral(ll)  while (isLiteral(*ll)) ll++; 
#define moveNLiteral(ll) while (*ll&&!isLiteral(*ll)&&*ll!='\n') ll++;
/*******************************************************************/
model *file2model(char *fn)
{
	char *ll; 
        model *mm=malloc(sizeof(model)); 

	ll=file2string(fn); 
	CNFread_numbers(ll,mm); 
	CNFread_clauses(ll,mm); free(ll);
	return mm;
}
void CNFread_numbers(char *ll,model *mm)
{
	int n,m,l;

	m=n=l=0;
	while (*ll) {
		if (*ll=='c') { while (*ll&&*ll!='\n') ll++; }
		else if (isNumbers(ll)) { break; }
		else ll++;
	}
	if (*ll) {
		moveNLiteral(ll); n=atoi(ll); moveLiteral(ll); 
		moveNLiteral(ll); m=atoi(ll); moveLiteral(ll); 
		moveNLiteral(ll); if (isLiteral(*ll)) l=atoi(ll); 
	}
	if (!(n>0&&m>0)) error("numbers required.");

	if (l==0) l=2*strlen(ll); 
	mm->n=n; mm->m=m; mm->l=l;
        mm->mstring=malloc(l*sizeof(int)); 
}
void CNFread_clauses(char *ll,model *mm)
{
        int i=0,j=0,k=0,len,n;
        int clen,c;
	int modm=mm->m;
	int modl=mm->l;
	int *p0=mm->mstring;
	
	while (*ll) {
		if (*ll=='c') { while (*ll&&*ll!='\n') ll++; }
		else if (isNumbers(ll)) { while (*ll&&*ll!='\n') ll++; break; }
		else ll++;
	}
	for (;;) {
		while (*ll&&(!isLiteral(*ll)||*ll=='0')) ll++;
		if (*ll==0) break;
                if (j==modm) error("too many clauses");
                for (clen=0,i=0;i<modl;i++) {
			c=atoi(ll); moveLiteral(ll); moveNLiteral(ll);
			if (!c) {
				n=remove_red(p0-clen,clen); 
				p0-=n; k-=n;
				if (clen>n) *p0++=c; else j--; break;
			}
        		if (k==modl) error("too many literals"); 
			k++; *p0++=c; clen++;
                }
                j++;
        }
	mm->m=j; mm->l=k+j; 
	mm->mstring=realloc(mm->mstring,mm->l*sizeof(int)); 
}
/************************************************************************/

/************************************************************************/
int *initorder(mstruct *cc,int *cl,char *gim,int n)
{
	int k;
	int i,c;
	float v0,v1;
	float *fp=(float *)malloc((n+1)*sizeof(float));
	float *fn=(float *)malloc((n+1)*sizeof(float));
	float *ff=(float *)malloc((n+1)*sizeof(float));
	int *io=(int *)malloc((n+1)*sizeof(int));
	for (i=1;i<=n;i++) { io[i]=i; }
	for (i=1;i<=n;i++) {
		if (gim[i]==0) {
			fp[i]=lvalue(cc,cl,i); fn[i]=lvalue(cc,cl,-i);
			ff[i]=fp[i]*fn[i];
			if (fp[i]==0.0&&fn[i]==0.0) { gim[i]=1; gim[-i]=3; }
		} 
		else { ff[i]=0.0; }
	}
/*
	reorder(ff,io,1,n); 
*/
	return io;
}
/**********************************************************************/
float lvalue(mstruct *cc,int *cl,int literal)
{
	int *p0,*p1;
	float val=0.0;
	int i=0;

        p0=&(cc->LiteralMatrix[cc->LS[literal]]);
        for (p1=p0+cc->LL[literal];p0<p1;p0++) 
        switch (cl[*p0]) {
        	case 0: break;
        	default: 
			if (cl[*p0]>10) val+=1.0/(1<<10); 
			else val+=1.0/(1<<(cl[*p0]-1)); break;
			i++;
        }

	if (i==1) return 0.0; return val;
}
/************************************************************************/
int b1search(int v,int *b,int n)
{
        int l=0,r=n-1,x;
        while (r>=l) {
                x=(l+r)/2;
                if (v<b[x]) r=x-1; else l=x+1; if (v==b[x]) return v;
        }
        return 0;
}
/************************************************************************/
int b2search(char *gim,int *b,int n)
{
        int l=0,r=n-1,x;

        while (r>=l) {
                x=(l+r)/2;
                if (gim[b[x]]==0) { r=x-1; 
			if (r<l) return b[x]; 		/*r<0*/
			if (gim[b[r]]!=0) return b[x];
		} else l=x+1; 
        }
	return 0;
}
/************************************************************************/
int lvalue_b(mstruct *cc,int *cl,int literal)
{
	int *p0,*p1;
	int k=0;

        p0=&(cc->LiteralMatrix[cc->LS[literal]]);
        for (p1=p0+cc->LL[literal];p0<p1;p0++) if (cl[*p0]) {
 return 1;
/*
*/
		switch (cl[*p0]) {
			case 2: k+=32; break; 
			case 3: k+=8; break; 
			default: k++; break;
		}
	}

/*
	if (k) return k;
*/
        p0=&(cc->LiteralMatrix[cc->LS[-literal]]);
        for (p1=p0+cc->LL[-literal];p0<p1;p0++) if (cl[*p0]) cl[*p0]=0;

	return 0;
}
/************************************************************************/
int ispart(mstruct *cc,int m,int n)
{
	int *p1;
	int pp,px;
	int i,j,k;

	for (px=-1,j=0,i=-n;i<=n;i++) if (cc->LL[i]==1&&i!=0) {
        	pp=(cc->LiteralMatrix[cc->LS[i]]);
		p1=cc->Clause[pp];
		while (*p1) if (cc->LL[*p1]!=1) break; else p1++;
		if (*p1) continue;
		p1=cc->Clause[pp];
		k=0; while (*p1) k+=cc->LL[-*p1++];
		if (k>j) {j=k; px=pp; }
        }

/*
	if (k<m/16) return -1; 
*/
	return px;
}
/************************************************************************/
int reorder(float *ff,int *io,int m,int n)
{
	int i,x;
	for (i=n;i>=m;i--) if (ff[i]!=0.0) break;
	while (i>m) { x=io[i]; io[i]=io[m]; io[m]=x; i--; m++; }
} 

/************************************************************************/
int lvalue_c(mstruct *cc,int *cl,int literal)
{
	int *p0,*p1;
	int k=0;

        p0=&(cc->LiteralMatrix[cc->LS[literal]]);
        for (p1=p0+cc->LL[literal];p0<p1;p0++) if (cl[*p0]) {
		switch (cl[*p0]) {
			case 2: k+=32; break; 
			case 3: k+=8; break; 
			default: k++; break;
		}
	}
	if (k) return k;

        p0=&(cc->LiteralMatrix[cc->LS[-literal]]);
        for (p1=p0+cc->LL[-literal];p0<p1;p0++) if (cl[*p0]) cl[*p0]=0;

	return 0;
}

/************************************************************************/
int eqcycle(int *a,int *b)
{
        int i,j;
        for (i=1;i<=*b;i++) {
                if (j=intin(b[i],a,1,i-1)) b[i]=b[j];
                if (j=intin(-b[i],a,1,i-1)) b[i]=-b[j];
                if (a[i]==-b[i]) { return 0; }
                if (a[i]==b[i]) {
                        for (j=i+1;j<=*b;j++) {
                                a[j-1]=a[j]; b[j-1]=b[j];
                        }
                        a[0]--; b[0]--; i--; continue;
                }
                j=1;
                while (j) {
                        if (j=intin(a[i],b,1,i-1)) b[j]=b[i];
                        else if (j=intin(-a[i],b,1,i-1)) b[j]=-b[i];
                }
        }
        return 1;
}
/*******************************************************************/
void modelcompresseq(model *mm,mstruct *cc,mdata *wc,int *cl,int *n1,int *n2)
{
        int i,j,k;
        char *gim=wc->InactiveMatrix;
        int *p0=mm->mstring;
        int *p2=mm->mstring;

        for (k=i=0;i<mm->m;i++) if (cl[i]) {
                p2=cc->Clause[i];
                while (*p2) if (gim[*p2]&1) p2++; else {
                        if (j=intin(*p2,n1,1,*n1)) *p0++=n2[j];
                        else if (j=intin(-*p2,n1,1,*n1)) *p0++=-n2[j];
                        else *p0++=*p2;
                        p2++;
                }
                *p0++=*p2++; k++;
        }
        mm->m=k;
        mm->l=p0-mm->mstring;
        mm->mstring=realloc(mm->mstring,mm->l*sizeof(int));
        CNFmodelclean(mm);
}
/************************************************************************/
void modelcompress(model *mm,mstruct *cc,mdata *wc,int *cl)
{
        int i,k;
        char *gim=wc->InactiveMatrix;
        int *p0=mm->mstring;
        int *p2=mm->mstring;

        for (k=i=0;i<mm->m;i++) if (cl[i]) {
                p2=cc->Clause[i];
                while (*p2) if (gim[*p2]&1) p2++; else *p0++=*p2++;
                *p0++=*p2++; k++;
        }
        mm->m=k;
        mm->l=p0-mm->mstring;
        mm->mstring=realloc(mm->mstring,mm->l*sizeof(int));
}
/************************************************************************/
void CNFmodelclean(model *mm)
{
        int i;
        int *p0=mm->mstring;
        int *p2=mm->mstring;
        int m=0;
        int n,k;

        for (i=0;i<mm->m;i++) {
                k=0;
                while (*p0) { *p2++=*p0++; k++; }
                n=remove_red(p2-k,k);
                p2-=n; k-=n; if (k>0) { *p2++=0; m++; }
                p0++;   /*zwh*/
        }
        mm->m=m;
        mm->l=p2-mm->mstring;
        mm->mstring=realloc(mm->mstring,mm->l*sizeof(int));
}
/************************************************************************/

/************************************************************************/
int *unitclauses(mstruct *cc,mdata *wc,int *cl,int m,int *unit)
{
        int i,k;
        int *p2;
        char *gim;

        gim=wc->InactiveMatrix;
        for (k=i=0;i<m;i++) 
        if (cl[i]==1) {
                p2=cc->Clause[i]; while (gim[*p2]&1) p2++;
                unit[k++]=*p2; cl[i]=0;
        }
        unit[k]=0; return unit;
}
/************************************************************************/
int unitp(mstruct *cc,mdata *wc,int *cl,int *cur)
{
	char *gim=wc->InactiveMatrix;
	int *p0,*p1;
	int *cure,*p2;

        for (cure=cur;*cure;cure++) { gim[*cure] =4; gim[-*cure] =8; }
	for (;*cur;cur++) {
                gim[*cur] =1; gim[-*cur] =3; 
                p0=&(cc->LiteralMatrix[cc->LS[-*cur]]);
                for (p1=p0+cc->LL[-*cur];p0<p1;p0++)
                switch (cl[*p0]) {
                    case 0: break;
                    case 2: p2 =cc->Clause[*p0];
                            while (gim[*p2]&1) p2++; 
			    if (gim[*p2]&8) { imp_res(*p2,*p0,cc,wc); return 0;}
        		    if (!(gim[*p2]&4)) { 
                		*cure++=*p2; *cure=0; 
				gim[*p2] =4; gim[-*p2] =8; imp_update(*p2,*p0);
        		    }
                            cl[*p0] =0; break;
                    default:
			    cl[*p0]--; break;
                }
                p0=&(cc->LiteralMatrix[cc->LS[*cur]]);
                for (p1=p0+cc->LL[*cur];p0<p1;p0++) if (cl[*p0]) cl[*p0] =0;
        }
	wc->Current=cur;
	return 1;
}
/************************************************************************/
int unitp1(mstruct *cc,mdata *wc,int *cl,int *cur)
{
	char *gim=wc->InactiveMatrix;
	int *p0,*p1;
	int *cure,*p2;


        for (cure=cur;*cure;cure++) { gim[*cure] =4; gim[-*cure] =8; }
	for (;*cur;cur++) {
                gim[*cur] =1; gim[-*cur] =3; 
                p0=&(cc->LiteralMatrix[cc->LS[-*cur]]);
                for (p1=p0+cc->LL[-*cur];p0<p1;p0++)
                switch (cl[*p0]) {
                    case 0: break;
                    case 2: p2 =cc->Clause[*p0];
                            while (gim[*p2]&1) p2++; 
			    if (gim[*p2]&8) { return 0;}
        		    if (!(gim[*p2]&4)) { 
                		*cure++=*p2; *cure=0; 
				gim[*p2] =4; gim[-*p2] =8; 
        		    }
                            cl[*p0] =0; break;
                    default:
			    cl[*p0]--; break;
                }
                p0=&(cc->LiteralMatrix[cc->LS[*cur]]);
                for (p1=p0+cc->LL[*cur];p0<p1;p0++) if (cl[*p0]) cl[*p0] =0;
        }
	wc->Current=cur;
	return 1;
}
/************************************************************************/

/*******************************************************************/
/*
void cnfstringwrite(int *mstring,int mstringc,FILE *g)
{
        int i;
        for (i=0;i<mstringc;i++)
        if (mstring[i]) fprintf(g," %i ",mstring[i]);
        else fprintf(g," %i\n",mstring[i]);
}
/*******************************************************************/
void cnfmodelwrite(model *mo,FILE *g)
{
        int i;
        int *mstring=mo->mstring;
        int mstringc=mo->l;

        fprintf(g,"p cnf %i %i\n",mo->n,mo->m);
        for (i=0;i<mstringc;i++)
        if (mstring[i]) fprintf(g," %i ",mstring[i]);
        else fprintf(g," %i\n",mstring[i]);
}
/*******************************************************************/
void cnfmstructwrite(model *mo,mstruct *cc,char *lcl,int k,FILE *g)
{
        int i;
	int *p;

        fprintf(g,"p cnf %i %i\n",mo->n,k);
        for (i=0;i<mo->m;i++) if (lcl[i]) {
		p=cc->Clause[i];
		while (*p) fprintf(g," %i ",*p++); fprintf(g," %i\n",*p);
	}
}
/*******************************************************************/
void cnfmodelappendwrite(model *mo,int *literals,FILE *g)
{
        int i;
        int *mstring=mo->mstring;
        int mstringc=mo->l;

        fprintf(g,"p cnf %i %i\n",mo->n,mo->m+mo->n);
	while (*literals) { fprintf(g," %i 0\n",*literals); literals++; }
        for (i=0;i<mstringc;i++)
        if (mstring[i]) fprintf(g," %i ",mstring[i]);
        else fprintf(g," %i\n",mstring[i]);
}
/*******************************************************************/
