#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>
#include "tree.h"

/*******************************************************************/
int listlength(tree *t1)
{
        int i=1;
        while (t1->node[0]==';') { t1=t1->right; i++; }
        return i;
}
/*******************************************************************/
tree *listget1n(tree *t,int n)
{
        int lc=0;
        tree *spec=NULL;
        while (lc<n) spec=listgetone(t,lc++); return spec;
}
/*******************************************************************/
tree *listgetone2(tree *t,int next) 
{
	tree *t1;
	static tree *t0;

	if (next==0) t0=t;

	if (t0==NULL) return NULL;
        if (t0->node[0]==';') 
	{ t1=t0->left; t0=t0->right; } else { t1=t0; t0=NULL; } return t1;
}
tree *listgetone(tree *t,int next) 
{
	tree *t1;
	static tree *t0;

	if (next==0) t0=t;

	if (t0==NULL) return NULL;
        if (t0->node[0]==';') 
	{ t1=t0->left; t0=t0->right; } else { t1=t0; t0=NULL; } return t1;
}
tree *listmerge(tree *t0,tree *t1)
{
        tree *a0,*a1;

        if (t0==NULL) return t1; if (t1==NULL) return t0;
        a0=t0; while (a0->node[0]==';') a0=a0->right;
/*
        a1=treecompose(";",treecopy(a0),t1); treereplace(a0,a1);
*/
	a1=treecopy(a0);
	treefree(a0->left); treefree(a0->right);
	a0->left=a1; a0->right=t1; strcpy(a0->node,";");

        return t0;
}
/*******************************************************************/
tree *listcombone(tree *f(),tree *l1,tree *l2)
{
        tree *l0;
        if (l1->node[0]!=';') return f(l1,l2,NULL);
        l0=listcombone(f,l1->right,l2->right);
        return f(l1->left,l2->left,l0);
}
tree *listcombine(tree *f(),tree *x1,tree *x2)
{
        int a1=listlength(x1);
        int a2=listlength(x2);
        tree *r0,*t1,*t2;

        t1=treecopy(x1); t2=treecopy(x2);
        for (;a2>a1;a1++) t1=treecompose(";",treecompose(FALSEs,NULL,NULL),t1);
        for (;a1>a2;a2++) t2=treecompose(";",treecompose(FALSEs,NULL,NULL),t2);

        r0=listcombone(f,t1,t2); treefree(t1); treefree(t2);
	return r0;
}
tree *listcombinedist(tree *f(),tree *x1,tree *x2)
{
        int a1=listlength(x1);
        int a2=listlength(x2);
        tree *r0,*r1,*t1,*t2;

        t1=treecopy(x1); t2=treecopy(x2);
        for (;a2>a1;a1++) t1=treecompose(";",treecopy(x1),t1);
        for (;a1>a2;a2++) t2=treecompose(";",treecopy(x2),t2);

        r0=listcombone(f,t1,t2); treefree(t1); treefree(t2);
	return r0;
}
/*******************************************************************/
void listtraverse(tree *ts,void f())
{
	int lc=0;
        tree *tr2;
        while ((tr2=listgetone(ts,lc++))!=NULL) { f(tr2); }
}
/*******************************************************************/
tree *listaddstring(char *s,tree *v0)
{
	tree *v1;
	if (v0==NULL) return treecompose(s,NULL,NULL);
	v1=v0;
        while (v1->node[0]==';') 
	if (strcmp(v1->left->node,s)==0) return v0; else v1=v1->right;
        if (strcmp(v1->node,s)==0) return v0;
	v1->left=treecopy(v1); v1->right=treecompose(s,NULL,NULL);
	v1->node[0]=';'; v1->node[1]=0;
	return v0;
}
/*******************************************************************/
int listrmstring(char *s,tree *v0)
{
	tree *v1;
	if (v0==NULL) return 0;
        for (v1=v0;v1->node[0]==';';v1=v1->right) 
	if (strcmp(v1->left->node,s)==0) { 
		treereplace(v1,treecopy(v1->right)); return 1;
	} 
        if (strcmp(v1->node,s)==0) { 
		treereplace(v1,treecopy(v1->right)); return 1; 
	}
	return 0;
}
/*******************************************************************/
/*
int listsearchstring(char *s,tree *v0)
{
	tree *v1;
	if (v0==NULL) return 0;
        for (v1=v0;v1->node[0]==';';v1=v1->right) 
	if (strcmp(v1->left->node,s)==0) { return 1; } 
        if (strcmp(v1->node,s)==0) { return 1; }
	return 0;
}
/*******************************************************************/

/*******************************************************************/
int istxtprocedure(char *s)
{
	char *p2;

	for (p2=s;*s&&(isalpha(*s)||isdigit(*s));s++);

	if (*s=='(') {
        	stringreplacechar(p2,'(',':');
        	stringreplacechar(p2,')',';');
        	stringreplacechar(p2,',','|');
		return 1;
	}
	return 0;
}
/*******************************************************************/

/*******************************************************************/
tree *eqstate(int pa,int i,int pb,int j,int k)
{
        tree *t1=(tree *)malloc(sizeof(tree));
        tree *t2=(tree *)malloc(sizeof(tree));

        if (k==0) return treeconst(1); /*for satbmc k=0*/
	if (k<0) k=0; /*for qbfbmc, org k=0, zwh*/

        sprintf(t1->node,"%i",(pa-1)*(k+1)+i); t1->left=NULL; t1->right=NULL;
        sprintf(t2->node,"%i",(pb-1)*(k+1)+j); t2->left=NULL; t2->right=NULL;
        return treecompose("=",t1,t2);
}
void eqstateconstruct(tree *t,int bn)
{
	int a,b,i;
	tree *t0,*t1;

	if (t==NULL) return;
	if (t->node[0]=='=') {
		a=atoi(t->left->node); b=atoi(t->right->node);
		i=1; t0=leq(treenum(a*bn+i),treenum(b*bn+i)); 
		while (i<bn) {
			i++; t1=leq(treenum(a*bn+i),treenum(b*bn+i)); 
			t0=treecompose("&",t0,t1);
		}
		treereplace(t,t0); return;
	}
	else if (t->node[0]=='B') {
		a=atoi(t->left->node); b=atoi(t->right->node);
		i=1; t0=xor(treenum(a*bn+i),treenum(b*bn+i)); 
		while (i<bn) {
			i++; t1=xor(treenum(a*bn+i),treenum(b*bn+i)); 
			t0=treecompose("|",t0,t1);
		}
		treereplace(t,t0); return;
	} 
	eqstateconstruct(t->left,bn); eqstateconstruct(t->right,bn); 
}
/*******************************************************************/

/*******************************************************************/
LOCAL char OP9[9]="#";
LOCAL char OP8[9]=";";
LOCAL char OP7[9]="UR:";
LOCAL char OP6[9]="|";
LOCAL char OP5[9]="&";
LOCAL char OP4[9]="AEFGX!";
LOCAL char OP3[9]=">=<?@{}B";
LOCAL char OP2[9]="+-";
LOCAL char OP1[9]="*/%";
/*******************************************************************/
tree *string2tree_local(char *s,char *op)
{
	char *p=s;
	int l=0;
	tree *t,*t0;

	while (*p&&(l||!charsearch(*p,op))) { 
		if (*p=='('||*p=='[') { 
			l++; 
			if (l<0) error("Syntax");
		}
		else if (*p==')'||*p==']') l--; 
		p++; 
	}
	if (charsearch(*p,op)) { 
		t=(tree *)(malloc(sizeof(tree)));
		t->node[0]=*p; 
		t->node[1]=0; *p=0;
		t->left=string2tree(s); 
		t->right=string2tree(p+1); 
		if (t->right==NULL) { t0=t; t=t0->left; free(t0); } 
		return t;
	}
	return NULL;
}
tree *string2tree(char *s)
{
	char *p=s;
	int i,l=0;
	tree *t;
	char terminal[1024];

	if (*p==0) return NULL;

	if ((t=string2tree_local(s,OP9))!=NULL) return t; 
	if ((t=string2tree_local(s,OP8))!=NULL) return t; 
	if ((t=string2tree_local(s,OP7))!=NULL) return t; 
	if ((t=string2tree_local(s,OP6))!=NULL) return t; 
	if ((t=string2tree_local(s,OP5))!=NULL) return t; 
	if ((t=string2tree_local(s,OP4))!=NULL) return t; 
	if ((t=string2tree_local(s,OP3))!=NULL) return t; 
	if ((t=string2tree_local(s,OP2))!=NULL) return t; 
	if ((t=string2tree_local(s,OP1))!=NULL) return t; 

 
	if (*p=='(') return string2tree(p+1);
	t=(tree *)(malloc(sizeof(tree)));
	for (i=1;*p;p++) {
		if (*p=='(') i++; if (*p==')') i--; if (i==0) break;
		terminal[l++]=*p; 
	}
/*new
	while (*p&&*p!=')') t->node[l++]=*p++; 
*/

	terminal[l]=0;
	if (istxtprocedure(terminal)) { t=string2tree(terminal); return t; }
	strcpy(t->node,terminal); t->left=NULL; t->right=NULL; return t;
}

/*******************************************************************/
#define iscomment(s)			(*s=='/'&&*(s+1)=='/')
/*******************************************************************/
void stringcomments(char *s)
{
        char *t=s;

        while (*s) {
                if (iscomment(s)) { while (*s&&*s!=13&&*s!=10) s++; continue; }
                *t++=*s++;
        }
	*t=*s;
}
void stringcompress(char *s,char a,char b)
{
        char *t=s;
        char *p=s;

        while (*s) {
                if (*s==9||*s==10||*s==13||*s==a||*s==b) { s++; continue; }
                if (iscomment(s)) { while (*s&&*s!=13&&*s!=10) s++; continue; }
                if (*s=='!') { if (*(t-1)==']'&&*(t-2)=='.') *s=CHANW; }
                *t++=*s++;
        }
	*t=*s;
}
char *stringed(char *s,char *a,char *b)
{
        int i=strlen(a);
        int j=strlen(b);
        char *t,*t0;

        for (t=t0=s;1;) {
                while (*s&&*s!=*a) *t++=*s++;
                if (*s==0) { *t=0; break; }
                if (strncmp(s,a,i)==0) { strncpy(t,b,j); t+=j; s+=i; }
                else { *t++=*s++; }
        }
        return t0;
}
/*******************************************************************/
char *stringsearch(char *s,char *a,char *b)
{
	int i=strlen(a),j;

	if (b!=NULL) 
	for (j=strlen(b);1;s++) {
                while (*s&&*s!=*a&&*s!=*b) s++; if (*s==0) { return s; }
                if (strncmp(s,a,i)==0||strncmp(s,b,j)==0) { return s; }
        }
	for (;1;s++) {
                while (*s&&*s!=*a) s++; if (*s==0) { return s; }
                if (strncmp(s,a,i)==0) { return s; }
        }
}
/*******************************************************************/
void *stringreplacechar(char *s,char a,char b)
{
        while (*s&&*s!='#') if (*s==a) *s++=b; else s++;
}
int stringreplacestring(char *s,char *a,char *b)
{
        if (strcmp(s,a)==0) { strcpy(s,b); return 1; } return 0;
}
int nodereplace(tree *t,tree *a,tree *b)
{
        if (strcmp(t->node,a->node)==0) { 
		treereplace(t,treecopy(b)); return 1; 
	} 
	return 0;
}
int stringreplacestringfixlength(char *s,char *a,char *b)
{
        int la=strlen(a);
        int lb=strlen(b);
        char tmp[MAXVARLEN];

        if (strncmp(s,a,la)==0&&(!isalnum(s[la])||s[la-1]=='[')) { 
			/*zwh add lastcond 121131*/
			/*zwh second half of lastcond 20130328*/
                strcpy(tmp+lb,s+la); strncpy(tmp,b,lb);
                strcpy(s,tmp); return 1;
        }
        return 0;
}
/*******************************************************************/
void treereplacestring(tree *t,char *a,char *b)
{
	if (t==NULL) return;
	treereplacestring(t->left,a,b); treereplacestring(t->right,a,b);
	stringreplacesubstring(t->node,a,b);
}
/*******************************************************************/
void stringreplacesubstring(char *s,char *a,char *b)
{
        int la=strlen(a);
        int lb=strlen(b);
        char tmp[MAXVARLEN];

	for (;*s;s++) if (strncmp(s,a,la)==0) {
		if (!(isdigit(s[la]||islower(s[la])))) {
                	strcpy(tmp+lb,s+la); strncpy(tmp,b,lb);
                	strcpy(s,tmp); s+=lb;
        	}
	}
}
/*******************************************************************/
int ran(char *s,int left)
{
	int a;
	int k=1;

	if (*s=='i'&&isdigit(*(s+1))) {
		if (left==0) return 0;
		a=atoi(s+1); while (a) { k*=2;a--; } return k-1;
	}
	switch (left) {
		case 0: while (!isdigit(*s)&&*s!=']') s++; return atoi(s);
		case 1: while (!isdigit(*s)) s++; while (isdigit(*s)) s++; 
			while (!isdigit(*s)&&*s!=']') s++; return atoi(s);
	}
	
}
/*******************************************************************/

/*******************************************************************/
int treesize(tree *t0) 
{
	if (t0==NULL) return 0;
	return treesize(t0->left)+treesize(t0->right)+1;
}
int treesizeof(char nd,tree *t0) 
{
	if (t0==NULL) return 0;
	if (t0->node[0]!=nd) return 1;
	return treesizeof(nd,t0->left)+treesizeof(nd,t0->right);
}
tree *treesearchvarlist(char *s,int n,tree *t)
{
	int i;
	int lc=0;
	tree *l0;
	
	while ((l0=listgetone(t,lc++))!=NULL) {
		if (n) i=strncmp(l0->left->node,s,n); 
		else i=strcmp(l0->left->node,s); 
		if (i==0) return l0;
	}

	return NULL;
}
int treesearchvarlistnum(char *s,int n,tree *t)
{
	int i;
	int lc=0;
	tree *l0;
	
	while ((l0=listgetone(t,lc++))!=NULL) {
		if (n) i=strncmp(l0->left->node,s,n); 
		else i=strcmp(l0->left->node,s); 
		if (i==0) return lc-1;
	}

	return -1;
}
tree *treesearchstringlist(char *s,int n,tree *t)
{
	int i;
	int lc=0;
	tree *l0;
	
	while ((l0=listgetone(t,lc++))!=NULL) {
		if (n) i=strncmp(l0->node,s,n); else i=strcmp(l0->node,s); 
		if (i==0) return l0;
	}

	return NULL;
}
tree *treesearchvarall(char *node,int n,tree *t)
{
	int i;
        tree *t1,*t2;
	char n0;

        if (t==NULL) return NULL; n0=t->node[0];
	
	if (t->node[0]=='&'||t->node[0]=='|') {
        	t1=treesearchvarall(node,n,t->left); 
		t2=treesearchvarall(node,n,t->right);
        	if (t1==NULL) return t2; if (t2==NULL) return t1;
        	return treecompose(t->node,t1,t2);
	}

	if (n) i=strncmp(t->left->node,node,n); 
	else i=strcmp(t->left->node,node); 
	if (i==0) return treecopy(t); return NULL;
}
tree *treedopost(tree *t,tree *f())
{
	if (t==NULL) return t;
	t->left=treedopost(t->left,f); t->right=treedopost(t->right,f); 
	return f(t);
}
void treedopostvoid(tree *t,void f())
{
	if (t==NULL) return;
	treedopostvoid(t->left,f); treedopostvoid(t->right,f); f(t);
}
/*
tree *treedopre(tree *t,int f0())
{
        if (isprop(t)) return (f0(t));
	t->left=treedopre(t->left,f0); t->right=treedopre(t->right,f0); 
	return t;
}
/*******************************************************************/
void treedoprevoid(tree *t,int f0())
{
        if (f0(t)) return;
        treedoprevoid(t->left,f0); treedoprevoid(t->right,f0);
}
/*******************************************************************/
void treefree(tree *t) 
{
	if (t==NULL) return;
	treefree(t->left); treefree(t->right); free(t);
}
tree *treecopy(tree *t) 
{
	tree *t0;

	if (t==NULL) return NULL;
	t0=(tree *)(malloc(sizeof(tree)));
	strcpy(t0->node,t->node);
	t0->left=treecopy(t->left); t0->right=treecopy(t->right); return t0;
}
/*******************************************************************/
void treereplace(tree *t0,tree *t1) 
{
	treefree(t0->left); treefree(t0->right); 
	t0->left=t1->left; t0->right=t1->right; strcpy(t0->node,t1->node); 
	free(t1);
}
tree *treecompose(char *s,tree *t0,tree *t1) 
{
	tree *t;

	t=(tree *)(malloc(sizeof(tree)));
	if (t==NULL) error("treecompose");
	strcpy(t->node,s); t->left=t0; t->right=t1; 
	return t;
}
/*******************************************************************/
tree *treeconst(int positive)
{               
        switch (positive) {
          case 1: return treecompose(TRUEs,NULL,NULL);
          case 0: return treecompose(FALSEs,NULL,NULL);
        }       
}               
/**************************************************************************/
tree *treeconstab(int positive,int i)
{
        tree *t=(tree *)malloc(sizeof(tree));
        tree *t0=(tree *)malloc(sizeof(tree));
        tree *t1=(tree *)malloc(sizeof(tree));

	sprintf(t0->node,"%ix",i); t0->left=NULL; t0->right=NULL;
        strcpy(t1->node,"!");  t1->left=NULL; t1->right=treecopy(t0);
        if (positive) strcpy(t->node,"|"); else strcpy(t->node,"&");
        t->left=t0; t->right=t1;
        return t;
}
/**************************************************************************/
tree *treenum(int n)
{
        tree *t=(tree *)(malloc(sizeof(tree)));
        sprintf(t->node,"%i",n); t->left=NULL; t->right=NULL;
        return t;
}
tree *treestring(char *s)
{
        tree *t=(tree *)(malloc(sizeof(tree)));
        sprintf(t->node,"%s",s); t->left=NULL; t->right=NULL;
        return t;
}
tree *treevarconstr(char *s,int low,int high)
{
        tree *t1,*t2;

	t2=treecompose("{",treestring(s),treenum(high));
	if (low==0) return t2;

	t1=treecompose("{",treenum(low),treestring(s));
        return treecompose("&",t1,t2);
}
tree *treesearch(char *s,int n,tree *t)
{
	int i;
	tree *t0;
	if (t==NULL) return NULL;
	if (n) i=strncmp(t->node,s,n); else i=strcmp(t->node,s); 
	if (i==0) return t;
	t0=treesearch(s,n,t->left); if (t0!=NULL) return t0;
	return treesearch(s,n,t->right); 
}
/*******************************************************************/
tree *treetr(tree *tr,int a,int b,int pn)
{
        return
        treeaddnumberconditional(
	treeaddnumber(treecopy(tr),a*pn),(b-a-1)*pn,(a+1)*pn);
}
/*******************************************************************/
tree *treeaddnumber(tree *t,int i)
{
        if (t==NULL) return NULL;
        if (isdigit(t->node[0])) sprintf(t->node,"%i",atoi(t->node)+i);
        t->left=treeaddnumber(t->left,i); t->right=treeaddnumber(t->right,i); 
	return t;
}
/*******************************************************************/
tree *treeaddnumberconditional(tree *t,int i,int c)
{
        int n;
        if (t==NULL) return NULL;
        if (isdigit(t->node[0])) {
                n=atoi(t->node); if (n>c) sprintf(t->node,"%i",n+i);
        }
        t->left=treeaddnumberconditional(t->left,i,c);
        t->right=treeaddnumberconditional(t->right,i,c); return t;
}
/************************************************************************/
/*
int intlen(int *aa)
{
	int i=0; while (*aa) { aa++; i++; } return i;
}
/************************************************************************/
int treediff(tree *s,tree *t)
{
        if (s==NULL) return t!=NULL;
        if (t==NULL) return s!=NULL;
	if (strcmp(s->node,t->node)!=0) return 1;
        return treediff(s->left,t->left)||treediff(s->right,t->right);
}
/************************************************************************/









/**********************************************************************/
#ifndef CLK_TCK
#define CLK_TCK       100 /*CLOCKS_PER_SEC*/
#endif
/**********************************************************************/
float utiltime(int finish)
{
        struct tms *strtemps=(struct tms *)malloc(sizeof(struct tms));
        static clock_t start;

        if (finish==0) {
                times(strtemps);
                start= strtemps->tms_utime;
                return 0.0;
        }

        times(strtemps);
        return (float)(strtemps->tms_utime-start)/(float)CLK_TCK;
}
/*******************************************************************/
int fileok(char *s)
{
	FILE *g=fopen(s,"r");
	if (g==NULL) { 
		fprintf(stdout,"ERROR:      %s not found\n",s); 
		return 0; 
	} 
	fclose(g); return 1;
}
/*******************************************************************/
char *file2string(char *fn)
{
        int len;
        char *ff;
	FILE *f;

/*
  fprintf(stdout,"%lx\n",(long)ggb->prb);
  if (ggb->prb!=NULL) infos("SSSSSSSSSSSSSSx");
*/
	f=fopen(fn,"r"); 
/*
  fprintf(stdout,"%lx\n",(long)ggb->prb);
  if (ggb->prb!=NULL) infos("SSSSSSSSSSSSSSn");
*/
	fseek(f,0,2); len=ftell(f); fseek(f,0,0);
        ff =(char*) malloc(len+1); fread(ff,len,1,f); ff[len-1]=0; fclose(f);
        return ff;
}
/*******************************************************************/
int arrayindex(char *node)
{
        int i=1;
        while (*node&&*node!='[') { i++; node++; }
        if (*node) return i; else return 0;
}
/*******************************************************************/
int pararrayindex(char *node)
{
        int i=1;
        while (*node&&*node!='[') { i++; node++; }
        if (*node&&*(node+1)==']') return i; else return 0;
}
/*******************************************************************/
int marrayindex(char *node)
{
        int i=1;
        while (*node&&*node!='[') { node++; i++; }
        if (*node==0) return 0;
        node++; while (*node&&*node!='[') node++;
        if (*node==0) return 0;
        return i;
}
/*******************************************************************/
int chanindex(char *node)
{
        int i;
        while (*node&&*node!='[') node++;
        if (*node==0) return 0;
        node++; i=atoi(node); while (*node&&isdigit(*node)) node++;
        if (*node==']') return i;
        return 0;
}
/*******************************************************************/
char charsearch(char a,char* s)
{
        while (*s&&*s!=a) s++; return *s;
}
/*******************************************************************/
void quicksort_ab(int *a,int *b,int l,int r ) /* decreasing */
{
        int v,i,j,t;
        if (r>l) {
                v=a[r]; i=l-1;j=r;
                for (;;) {
                        for (i++;(a[i]>v);i++);
                        for (j--;j>=l&&(a[j]<v);j--); if (j<l) j=1;
/*
                        while (a[++i] > v); while (a[--j] < v);
*/
                        if (i>=j) break;
                        t=a[i]; a[i]=a[j]; a[j]=t; t=b[i]; b[i]=b[j]; b[j]=t;
                }
                t=a[i]; a[i]=a[r]; a[r]=t;
                t=b[i]; b[i]=b[r]; b[r]=t;
                quicksort_ab(a,b,l,i-1); quicksort_ab(a,b,i+1,r);
        }
}
/**********************************************************************/
void gfilename(char *s,char *ss)
{
        int len;

        strcpy(ss,s);
        len=strlen(ss)-1;
        while (len&&ss[len]!='.') len--;
        ss[len]=0; return;
}
/*******************************************************************/

/*******************************************************************/
void treew(tree *t)
{
        fprintf(stdout,"treew 0\n"); treewrite(t,stdout); 
	fprintf(stdout,"treew 1\n"); 
}
void treewrite(tree *t,FILE *f)
{
	debug();
        if (t==NULL) return;
        if (t->left==NULL) { fprintf(f,"%s ",t->node); treewrite(t->right,f); }
        else {
                fprintf(f,"("); treewrite(t->left,f);
                fprintf(f,"%s ",t->node);
                if (t->node[0]==';'||t->node[0]=='#') fprintf(f,"\n");
                treewrite(t->right,f); fprintf(f,")");
        }
}
/*******************************************************************/
void stringwrite(char *ss,int a,int b,FILE *g)
{
        int i;
	debug();
        for (i=a;i<=b;i++) fprintf(g,"%c",ss[i]); fprintf(g,"\n");
}
/*******************************************************************/
void modulewrite(module *mm,FILE *f)
{
	debug();
	fprintf(f,"MAIN\n"); 
	treewrite(mm->prog,f); fprintf(f,"\n"); mm=mm->next;
	while (mm!=NULL) {
		fprintf(f,"MODULE ("); treewrite(mm->parameters,f); 
		fprintf(f,")\n"); 
		treewrite(mm->prog,f); fprintf(f,"\n"); 
		mm=mm->next;
	}
}
/*******************************************************************/
int treewrites(tree *t,char *f,int i)
{
        if (t==NULL) return i;
        if (t->left==NULL) {
                sprintf(f+i,"%s",t->node); i+=strlen(t->node);
                i=treewrites(t->right,f,i);
        } else {
                sprintf(f+i,"("); i++;
                i=treewrites(t->left,f,i);
                sprintf(f+i,"%s",t->node); i+=strlen(t->node);
                i=treewrites(t->right,f,i);
                sprintf(f+i,")"); i++;
        }
        return i;
}
/*******************************************************************/
void inttablewrite(int **tt,int mrows,int ncols,FILE *ff)
{
        int i,j;

        fprintf(stdout,"TABLE\n");
        for (i=0;i<mrows;i++) {
                for (j=0;j<ncols;j++) { fprintf(ff,"%i ",tt[i][j]);}
                fprintf(ff,"\n");
        }
}
/*******************************************************************/
void prgwrite(prg *pr1,FILE *f)
{
	int i,k;
	process *prs;

       	fprintf(f,"PROG=\n");treewrite(pr1->prog,f);fprintf(f,"\n");
       	fprintf(f,"BOOLN=%i\n",pr1->booln);
       	fprintf(f,"VARC=\n");treewrite(pr1->varconstr,f);fprintf(f,"\n");
       	fprintf(f,"DEAD=\n");treewrite(pr1->deadlockfreecond,f);fprintf(f,"\n");
       	fprintf(f,"VLIST=\n");treewrite(pr1->vlist,f);fprintf(f,"\n");
       	fprintf(f,"PURETRANS=%i\n",pr1->puretrans);
       	fprintf(f,"PRCNT=%i\n",pr1->prscnt);
	for (k=0,prs=pr1->prs[0];prs!=NULL;prs=pr1->prs[k+1],k++) {
		for (i=0;i<prs->n;i++) {
			fprintf(f,"P%iT%i:",k+1,i+1); 
			treewrite(prs->tslist[i],f); fprintf(f,"\n");
			treewrite(prs->tslistpc[i],f); fprintf(f,"\n");
		}
	}
	fprintf(f,"P%iT%i:",0,0); 
	treewrite(pr1->deadlocktr,f); fprintf(f,"\n");
}
/*******************************************************************/
void ggbwrite(ggbase *ggb,FILE *f)
{
	prg *pr;
	module *mm;

	debug();
        if (ggb==NULL) return;
        if (ggb->file!=NULL) fprintf(f,"FILE=%s\n",ggb->file);
        if (ggb->string!=NULL) fprintf(f,"STRING=%s\n",ggb->string);
	for (mm=ggb->mm;mm!=NULL;mm=mm->next) {
		fprintf(f,"PROG=\n"); treewrite(mm->prog,f); fprintf(f,"\n");
        	fprintf(f,"TYPE=%c\n",mm->type);
        	fprintf(f,"PAR=\n");treewrite(mm->parameters,f);fprintf(f,"\n");
	}
	if ((pr=ggb->pr)!=NULL) { fprintf(f,"PR\n"); prgwrite(pr,f); }
	if ((pr=ggb->prb)!=NULL) { fprintf(f,"PR\n"); prgwrite(pr,f); }
}
/*******************************************************************/
int treewritep(tree *t,char *f,int i,int j)
{
        if (t==NULL) return i;
	if (j==0) {
		if (t->node[0]!=':') {
               		sprintf(f+i,"%s",t->node); i+=strlen(t->node);
			if (t->right==NULL) { sprintf(f+i,"()"); i+=2; }
                	sprintf(f+i,";"); i+=1; 
		} else {
                	i=treewritep(t->left,f,i,1); sprintf(f+i,"("); i+=1; 
                	i=treewritep(t->right,f,i,1); sprintf(f+i,");"); i+=2; 
		}
	} else {
        	if (t->node[0]=='|') {
                	i=treewritep(t->left,f,i,1); sprintf(f+i,","); i++;
                	i=treewritep(t->right,f,i,1);
		} else {
                	i=treewritep(t->left,f,i,1);
                	sprintf(f+i,"%s",t->node); i+=strlen(t->node);
                	i=treewritep(t->right,f,i,1);
		}
        }
        return i;
}
/*******************************************************************/
void intstringwrite(int *a)
{
	while (*a) { fprintf(stdout,"%i ",*a); a++; } fprintf(stdout,"\n"); 
}
/*******************************************************************/
