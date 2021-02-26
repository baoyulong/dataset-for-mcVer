#include "q1rd.h"
#include "q1rd.h"

/********************************************************************/
qinfo *q1read(char *fn,qinfo *qi)
{
	qmdata *md;


	md=file2md(fn,qi); 
	qmdsimp(md,qi); 
	if (md!=NULL) { md2qi(md,qi); freeqmdata(md); } else 
	qi=file2cnfmodel(fn,qi);
	qi->sd=qi2sd(qi); 
	qi->wd=sd2wd(qi->sd,qi);

	
	return qi;
}
/********************************************************************/
qinfo *md2qifull(qmdata *md,qinfo *qi)
{

	md2qi(md,qi); 
	qi->sd=qi2sd(qi); 
	qi->wd=sd2wd(qi->sd,qi);
	
	return qi;
}
/********************************************************************/
void freesdata(sdata *sd,qinfo *qi)
{
	int i;
	int nn=qi->n+qi->m;
	free(sd->LiteralMatrix);
	free(sd->LL-nn);
	free(sd->LS-nn);
	free(sd);
}
void freeqwdata(qwdata *wd,qinfo *qi)
{
	int i;
	int nn=qi->n+qi->m;
	free(wd->InactiveMatrix-nn);
	free(wd->InactiveList);
	free(wd->dLiterals);
	for (i=0;i<nn;i++) if (wd->cLength[i]!=NULL) free(wd->cLength[i]);
	free(wd);
}
void freeqinfo(qinfo *qi)
{
	free(qi->e-qi->n);
	free(qi->mstring);
	free(qi->clauseinfo);
	freesdata(qi->sd,qi);
	freeqwdata(qi->wd,qi);
	free(qi);
}


/************************************************************************/
qmdata *file2md(char *fn,qinfo *qi);
void md2qi(qmdata *md,qinfo *qi);
/************************************************************************/
int mdwrite(qinfo *qi,qmdata *md,FILE *g);
int qiwrite(qinfo *qi,FILE *g);
/************************************************************************/
#define space(c) (c==' '||c==10||c==13)
/************************************************************************/
qmdata *file2md(char *fn,qinfo *qi)
{
	char line[MAXLINELEN];
	FILE *ff=fopen(fn,"r");
	qmdata *md=NULL;


	fgets(line,MAXLINELEN,ff); 
	if (strncmp(line,"QBF",3)!=0) { return NULL; }
	fgets(line,MAXLINELEN,ff); qi->n=atoi(line); qi->l=qi->m=0;
	fgets(line,MAXLINELEN,ff); 
	switch (*line) {
		case 'q': md= file2qmdataq(qi,ff);  
			  break;
		case 'c': 
		case 'd': qi->m++;
			  md= file2qmdatacd(qi,ff,*line);  break;
	}
	qi->l+=qi->m-1; 
	fclose(ff); return md;
}
int *linecreate(char *ll)
{
	int *cl=(int *)malloc(MAXLINELEN*sizeof(int));
	int i,n;

	i=0; cl[0]=ll[0]; ll++; while (*ll&&space(*ll)) ll++;
	while (*ll) {
		i++; cl[i]=atoi(ll);
		while (*ll&&*ll!=' ') ll++; while (*ll&&space(*ll)) ll++; 
	}
	cl[i+1]=0;
/*
	return realloc(cl,(n+2)*sizeof(int));
*/
	return realloc(cl,(i+2)*sizeof(int));
}
void linenegate(char *l1,char *l2)
{
	*l2++=' ';
	while (*l1) {
		while (*l1&&!isdigit(*l1)) *l2++=*l1++;
		if (isdigit(*l1)) { 
			*l2++='-'; while (isdigit(*l1)) *l2++=*l1++;
		}
	}
	*l2=0;
}
qmdata *file2qmdataq(qinfo *qi,FILE *ff)
{
	char line[MAXLINELEN];
	qmdata *md;

	fgets(line,MAXLINELEN,ff); 
	if (feof(ff)) error("file2model");
	switch (*line) {
		case 'a': 
		case 'e': md=(qmdata *)calloc(1,sizeof(qmdata));
			  md->Clause=linecreate(line); 
			  md->q=file2qmdataq(qi,ff); 
			  qi->m++; qi->l+=intlen(md->Clause);
			  break;
		case 'c':
		case 'd': qi->m++; 
			  md=file2qmdatacd(qi,ff,*line); 
			  fgets(line,MAXLINELEN,ff); 
			  if (*line!='/') error("file2model"); /*q*/
			  break;
	}
	return md;
}

qmdata *file2qmdatacd(qinfo *qi,FILE *ff,char cd)
{
	char line1[MAXLINELENCD];
	char line2[MAXLINELENCD];
	char line3[MAXLINELENCD];
	qmdata *md=(qmdata *)calloc(1,sizeof(qmdata));
	qmdata *q0=md;
	qmdata *md1=NULL;

	fgets(line1,MAXLINELENCD,ff); 
	fgets(line2,MAXLINELENCD,ff); 
	if (feof(ff)) error("file2model");

	linenegate(line2,line3);
	line2[0]=cd; line2[1]=' '; line2[2]=0; 
	strcat(line2,line1); strcat(line2,line3); md->Clause=linecreate(line2);
	qi->l+=intlen(md->Clause);
	while (1) {
		fgets(line1,MAXLINELENCD,ff); 
		switch (*line1) {
			case 'q': md->q=file2qmdataq(qi,ff); 
				  break;
			case 'c': 
			case 'd': qi->m++; 
				  md->q=file2qmdatacd(qi,ff,*line1); break;
			case '/': 
			if (md1!=NULL) { free(md1->next); md1->next=NULL; }
				  return q0; /*cd*/
		}
		md1=md; 
		md->next=(qmdata *)calloc(1,sizeof(qmdata)); md=md->next;
	}
}
/************************************************************************/
int mdwrite(qinfo *qi,qmdata *md,FILE *g)
{
	fprintf(g,"QBF\n%i\n",qi->n); qmdatawrite1(md,g,0); fprintf(g,"QBF\n"); 
}
int qmdatawrite1(qmdata *md,FILE *g,int mode)
{
	int t0;
	if (md==NULL) return; t0=md->Clause[0];
	switch (t0) {
		case 'a': 
		case 'e': 
			if (mode!='q') fprintf(g,"q\n"); 
			fprintf(g,"%c ",t0); 
			intwrite(md->Clause+1,0,intlen(md->Clause+1)-1,g);
			qmdatawrite1(md->q,g,'q');
			if (mode!='q') fprintf(g,"/q\n"); 
			break;
		case 'c': 
		case 'd': 
			fprintf(g,"%c\n",t0); 
			pnintwrite(md->Clause+1,g);
			qmdatawrite1(md->q,g,0);
			for (;md->next!=NULL;md=md->next)
				qmdatawrite1(md->next->q,g,0);
			fprintf(g,"/%c\n",t0); 
			break;
	}
}
/************************************************************************/
void md2qi(qmdata *md,qinfo *qi)
{
	int i;
	int n=qi->n;
	int *q0;

	qi->mstring =(int *)malloc((qi->l)*sizeof(int));
	qi->clauseinfo =(qcdata *)malloc((qi->m)*sizeof(qcdata));

	qi->m=0;
	qmdatawrite2string(qi,md,qi->mstring,qi->clauseinfo,-1,n); 

	qi->e =(char *)calloc((2*qi->n+1),sizeof(char))+qi->n;

	for (i=0;i<qi->m;i++) if (qi->clauseinfo[i].t=='e') 
	for (q0=qi->clauseinfo[i].Clause;*q0&&*q0<=n;q0++) 
	qi->e[*q0]=qi->e[-*q0]=1;
}
int qmdatawrite2string(qinfo *qi,qmdata *md,int *clausem,qcdata *info,int p,int n)
{
	int t0;	
	int *c0;
	int k;
	qmdata *q0;
	int m=qi->m;

	if (md==NULL) return; t0=md->Clause[0];
	switch (t0) {
		case 'a': 
		case 'e': 
			info[m].Clause=clausem;
			info[m].p=p; info[m].t=t0;
			info[m].cl=intlen(md->Clause+1);
			intwrite2string(md->Clause+1,0,info[m].cl-1,clausem);
			clausem+=info[m].cl; 
			qi->m++;
			*clausem++=n+qi->m; *clausem++=0; info[m].cl++; 
			k=qmdatawrite2string(qi,md->q,clausem,info,m,n);
			return k+info[m].cl+1; 
		case 'c': 
		case 'd': 
			info[m].Clause=clausem;
			info[m].p=p; info[m].t=t0;
			info[m].cl=intlen(md->Clause+1);
			intwrite2string(md->Clause+1,0,info[m].cl-1,clausem);
			for (q0=md,k=0;q0->next!=NULL;q0=q0->next) k++;
			clausem+=info[m].cl; c0=clausem; 
			clausem+=k+1; info[m].cl+=k;
			qi->m++;
			k=0;
			if (md->q!=NULL) {
			  *c0++=n+qi->m; info[m].cl++; clausem++;
			  k+=qmdatawrite2string(qi,md->q,clausem+k,info,m,n);
			}
			for (;md->next!=NULL;md=md->next) {
			  *c0++=n+qi->m; 
			  k+=qmdatawrite2string(qi,md->next->q,clausem+k,info,m,n);
			}
			*c0=0;
			return k+info[m].cl+1; 
	}
}
/************************************************************************/
int qiwrite(qinfo *qi,FILE *g)
{
	int i;

	infoi(qi->m);
	infoi(qi->n);
	infoi(qi->l);
	cnfstringwrite(qi->mstring,qi->l,g);
	for (i=0;i<qi->m;i++) {
		infos("");
		infoi(qi->clauseinfo[i].p)
		infoi(qi->clauseinfo[i].t)
		infoi(qi->clauseinfo[i].cl)
	}
}
/************************************************************************/
void intwrite2string(int *ss,int a,int b,int *g)
{
        int i;
        for (i=a;i<=b;i++) g[i]=ss[i];
}
/*******************************************************************/

/************************************************************************/
char *file2string(char *fn); 
/************************************************************************/
qmodel *file2cnfmodel(char *fn,qmodel *mm)
{
	int *p0,*p1;
	int i;
	int *ms;
	int exlength;

	mm=file2qmodel(fn,mm);
	exlength=mm->n+2+mm->m+1;
	mm->l+=exlength; p0=ms=(int *)malloc((mm->l)*sizeof(int));


	mm->e =(char *)calloc((2*mm->n+1),sizeof(char))+mm->n;
	for (i=1;i<=mm->n;i++) mm->e[i]=mm->e[-i]=1;

	mm->m+=2;
	mm->clauseinfo =(qcdata *)malloc((mm->m)*sizeof(qcdata));

	mm->clauseinfo[0].t='e'; mm->clauseinfo[0].p=-1;
	mm->clauseinfo[0].cl=mm->n+1;
	mm->clauseinfo[0].Clause=p0;
	for (i=1;i<=mm->n;i++) { *p0++=i; } *p0++=i; *p0++=0;

	mm->clauseinfo[1].t='c'; mm->clauseinfo[1].p=0;
	mm->clauseinfo[1].cl=mm->m-2;
	mm->clauseinfo[1].Clause=p0;
	for (i=2;i<mm->m;i++) { *p0++=mm->n+i; } *p0++=0;

	for (p1=mm->mstring,i=2;i<mm->m;i++) { 
		mm->clauseinfo[i].t='d'; mm->clauseinfo[i].p=1;
		mm->clauseinfo[i].Clause=p0; 
		mm->clauseinfo[i].cl=0;
		while (*p1) { *p0++=*p1++; mm->clauseinfo[i].cl++; } 
		*p0++=*p1++;
	}
	free(mm->mstring); mm->mstring=ms; 

	return mm;
}
qmodel *file2qmodel(char *fn,qmodel *mm)
{
	char *ll; 

	ll=file2string(fn); 
	read_numbers(ll,mm); 
	read_clauses(ll,mm); free(ll);
	return mm;
}
/*******************************************************************/

/************************************************************************/
sdata *qi2sd(qinfo *qi);
qwdata *sd2wd(sdata *sd,qinfo *qi);
/************************************************************************/
int sdwrite(qinfo *qi,sdata *sd,FILE *g);
int wdwrite(qinfo *qi,qwdata *wd,int level,FILE *g);
/************************************************************************/
int *wdbranch0(int qim,qwdata *wd,int c);
int *wdbranch1(int qim,qwdata *wd,int c);
/************************************************************************/
sdata *qi2sd(qinfo *qi)
{
	int i,k;
        int *p0,*p1;
	int *pos;
	int nn=qi->n+qi->m;
	sdata *sd=(sdata *)malloc(sizeof(sdata));

        sd->LiteralMatrix   =(int *)malloc((qi->l)*sizeof(int)); 
        sd->LS              =(int *)malloc((nn+1+nn)*sizeof(int))+nn; 
        sd->LL              =(int *)malloc((nn+1+nn)*sizeof(int))+nn; 

	for (i=1;i<=nn;i++) sd->LL[i]=sd->LL[-i]=0; 
	for (p0=qi->mstring,p1=p0+qi->l;p0<p1;p0++) sd->LL[*p0]++;


	for (k=0,i=1;i<=nn;i++) {
		sd->LS[i]=k; k+=sd->LL[i]; sd->LS[-i]=k; k+=sd->LL[-i]; 
		sd->LL[i]=sd->LL[-i]=0;
	}
        for (p0=qi->mstring,i=0;i<qi->m;i++) {
		if (qi->clauseinfo[i].t=='a'||qi->clauseinfo[i].t=='e') {
			p0+=qi->clauseinfo[i].cl+1; continue;
		}
		for (k=0;k<qi->clauseinfo[i].cl;k++,p0++) 
		if (*p0) { 
			pos=&sd->LiteralMatrix[sd->LS[*p0]+sd->LL[*p0]++]; 
			*pos=i; 
		}
		p0++;
	}

	return sd;
}
/****************************************************************************/
int sdwrite(qinfo *qi,sdata *sd,FILE *g)
{
	int i;
	prettyintwrite(sd->LiteralMatrix,0,qi->l-1,g);
}
/****************************************************************************/
qwdata *sd2wd(sdata *sd,qinfo *qi)
{
	int i;
	int nn=qi->n+qi->m;
	qwdata *wd=(qwdata *)malloc(sizeof(qwdata));

        wd->InactiveMatrix   =(char *)calloc((nn+1+nn),sizeof(char))+nn; 
        wd->InactiveList     =(int *)calloc((nn+1),sizeof(int)); 
        wd->Current          =wd->InactiveList;
        wd->dLiterals        =(int *)calloc((nn+1),sizeof(int)); 
        wd->cur        	     =(int **)calloc((nn+1),sizeof(int *)); 
        wd->cLength           =(int **)calloc((nn),sizeof(int *)); 
	wd->depth=0;
	wd->cLength[0]=(int *)malloc((qi->m)*sizeof(int)); 
	for (i=0;i<qi->m;i++) wd->cLength[0][i]=qi->clauseinfo[i].cl;
	wd->cur[0]=wd->Current;

	return wd;
}
/****************************************************************************/
int wdwrite(qinfo *qi,qwdata *wd,int level,FILE *g)
{
	int i;
	prettyintwrite(wd->cLength[level],0,qi->m-1,g);
}
/****************************************************************************/
int *wdbranch0(int qim,qwdata *wd,int c)
{
	int i;

	wd->depth++; i=wd->depth;
	wd->dLiterals[i-1]=c; 
	wd->cur[i]=wd->Current; *wd->Current++=c; *wd->Current=0;
	if (wd->cLength[i]==NULL) 
	wd->cLength[i]=(int *)malloc((qim)*sizeof(int)); 
	memcpy(wd->cLength[i],wd->cLength[i-1],(qim)*sizeof(int)); 
	return wd->cLength[i];
}
void wdbranch0back(qwdata *wd)
{
	wd->depth--; 
}
int *wdbranch0restore(qwdata *wd)
{
	int i=wd->depth;

	int *cur=wd->cur[i];
	for (;*cur;cur++) wd->InactiveMatrix[*cur]=wd->InactiveMatrix[-*cur]=0;
	wd->Current=wd->cur[i]; *wd->Current=0; 
	wd->depth--;
}
int *wdbranch1(int qim,qwdata *wd,int c)
{
	int i=wd->depth;

	int *cur=wd->cur[i];
	for (;*cur;cur++) wd->InactiveMatrix[*cur]=wd->InactiveMatrix[-*cur]=0;
	wd->dLiterals[i-1]=c; 
	wd->Current=wd->cur[i]; *wd->Current++=c; *wd->Current=0;
	memcpy(wd->cLength[i],wd->cLength[i-1],(qim)*sizeof(int)); 
	/*potential to optimize! zwh*/
	return wd->cLength[i];
}
void wdbranch1back(qwdata *wd)
{
	wd->depth--; 
}
/****************************************************************************/
