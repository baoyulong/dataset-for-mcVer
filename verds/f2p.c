#include "clk.h"
#include "f2p.h"
#include "option.h"
#include "prep.h"
#include "tlf.h"

/*******************************************************************/
tree *PROGaddeq_constructeq(tree *v)
{
	tree *t0,*t1,*t2;

	if (v==NULL) return NULL;
	if (v->node[0]==';') { 
		if (v->right==NULL) return PROGaddeq_constructeq(v->left);
		t0=(tree *)malloc(sizeof(tree)); strcpy(t0->node,"&");
		t0->left=PROGaddeq_constructeq(v->left);
		t0->right=PROGaddeq_constructeq(v->right);
		return t0;
	} 
	if (v->node[0]!=':') error("read"); 

	t0=(tree *)malloc(sizeof(tree)); strcpy(t0->node,"=");
	t1=(tree *)malloc(sizeof(tree)); strcpy(t1->node,v->left->node);
	t2=(tree *)malloc(sizeof(tree)); strcpy(t2->node,v->left->node);
	t0->left=t1; t0->right=t2;
	t1->left=NULL; t1->right=NULL;
	t2->left=NULL; t2->right=NULL;
	return t0;
}
int PROGaddeq_modify(tree *t,tree *v)
{
	tree *t1;

	if (t==NULL) return 0;
	if ((t->node[0]=='&'))
	return PROGaddeq_modify(t->left,v)+PROGaddeq_modify(t->right,v); 

	if (t->node[0]!='=') error("modify"); 

	t1=treesearchvarall(t->left->node,arrayindex(t->left->node),v);
	if (t1!=NULL) { treereplace(t,t1); return treesizeof('&',t); }
	return 0;
}
void *PROGaddeq(tree *t,tree *v)
{
	int lc=0;
	tree *t0,*t1,*l0;
	int i,j=0;

/*
 if (ggb->prb!=NULL) infos("SSa");
*/
	if (vdslog!=NULL) fprintf(vdslog,"\nREPLACED VARIABLES\n");
/*
 if (ggb->prb!=NULL) infos("SSb");
*/
	while ((l0=listgetone(t,lc++))!=NULL) 
	if (l0->node[0]==':') { 
		t0=PROGaddeq_constructeq(v); 
		i=PROGaddeq_modify(t0,l0->right);
		j=treesizeof('&',l0->right);
		if (vdslog!=NULL) {
 		   fprintf(vdslog,"diff %i; eq %i; replaced %i;\n",j-i,j,i);
		   if (j-i>0) { 
			treewrite(l0->right,vdslog); fprintf(vdslog,"\n");
		   }
		}
		treefree(l0->right); l0->right=t0;
	}
}
/*******************************************************************/


/*******************************************************************/
#define progvar         prog->left
#define proginit        prog->right->left
#define progtrs         prog->right->right->left
#define progspec        prog->right->right->right
/*******************************************************************/
int AGcheckmodule(module *mm,char *opckv)
{
	int k;
	module *m0=mm;
	tree *a1,*c0,*act;
	tree *trs=NULL;
	tree *t2a;

	for (;mm!=NULL;mm=mm->next) 
	if (mm->type=='P'&&strcmp(mm->parameters->left->node,opckv)==0) {
        	a1=treecompose("=",treestr("pc_x_"),treenum(1));
        	act=treecompose("&",treecopy(mm->parameters),a1);
        	c0=treecompose("=",treestr("pc_x_"),treenum(0));
		trs=treecompose(":",c0,act);
        	act=treecompose(":",treecopy(a1),treecopy(a1));
		trs=treecompose(";",trs,act);
/*
		treefree(m0->prog->right->right->left);
*/
		m0->prog->right->right->left=trs;

		t2a=mm->prog->right->right->right->left;
		a1=AGcleanassumption(t2a);
		m0->prog->left=listmerge(m0->prog->left,a1);
		a1=treecompose(":",treestr("pc_x_"),treestr("0..1"));
		m0->prog->left=listmerge(m0->prog->left,a1);

/*
		m0->prog->right->left=listmerge(m0->prog->right->left,t2a);
*/
		m0->prog->right->left=t2a;
		a1=treecompose("=",treestr("pc_x_"),treestr("0"));
		m0->prog->right->left=listmerge(m0->prog->right->left,a1);

        	c0=treecompose("=",treestr("1"),treestr("1"));
        	a1=treecompose("A",NULL,treecompose("G",NULL,c0));
        	m0->progspec=listmerge(a1,m0->progspec);
/*
		mm->prog->right->right->right->left=treecompose(";",NULL,t2a);
*/
		return 1;
	}
	return 0;
}
/*******************************************************************/


/*******************************************************************/
#define progvar         prog->left
#define proginit        prog->right->left
#define progtrs         prog->right->right->left
#define progspec        prog->right->right->right
/*******************************************************************/
#define treestr(a)	treecompose(a,NULL,NULL)
/*******************************************************************/
int arraycheckmodule(module *mm)
{
	int k=0;
	tree *t,*a1;
	module *m0=mm;

	for (;mm!=NULL;mm=mm->next) if (mm->type!=0) {
		t=arraychecktrans(mm->progtrs,m0->progvar,mm->type);
		if (t!=NULL) { k=1; mm->progtrs=listmerge(mm->progtrs,t); }
	}
	if (k) {
        	a1=treecompose("=",treestr("err_x_"),treenum(0));
        	m0->proginit=listmerge(m0->proginit,a1); list2cn(m0->proginit);
        	a1=treecompose(":",treestr("err_x_"),treestr("0..1"));
        	m0->progvar=listmerge(m0->progvar,a1);
        	a1=treecompose("=",treestr("err_x_"),treenum(0));
        	a1=treecompose("A",NULL,treecompose("G",NULL,a1));
        	m0->progspec=listmerge(a1,m0->progspec);
	}
	return k;
}
/*******************************************************************/
tree *arraychecktrans(tree *t0,tree *vars,char type)
{
	int lc=0;
        tree *t,*trs=NULL;
	tree *a1,*a2,*a3,*x1,*x2;

        while ((t=listgetone2(t0,lc++))!=NULL) {
                a1=t->right;
                a2=arraygetbound(a1,vars,1);
                if (a2!=NULL) {
                        x1=treecompose("=",treestr("err_x_"),treenum(1));
			x2=treecompose("=",treestr("ret_"),treenum(1));
			if (type=='P') x1=treecompose("&",x1,x2);
			a2=treecompose("&",treecopy(t->left),TLFnegate(a2));
                        a3=treecompose(":",a2,x1);
                        trs=listmerge(a3,trs);
                } 
                a1=TLFnnf(treecopy(t->left));
                a2=arraygetbound(a1,vars,0);
                if (a2!=NULL) {
                        x1=treecompose("=",treestr("err_x_"),treenum(1));
			x2=treecompose("=",treestr("ret_"),treenum(1));
			if (type=='P') x1=treecompose("&",x1,x2);
			a2=treecompose("&",a1,TLFnegate(a2));
                        a3=treecompose(":",a2,x1);
                        trs=listmerge(a3,trs);
                } else { 
			treefree(a1); 
		}
        }
 
        return trs;
}
/*******************************************************************/
tree *arraygetbound(tree *t0,tree *vars,int right)
{
	int i,low,high;
	tree *t1,*t2,*t3;
	char *p1,*p2;
	char n0[MAXVARLEN];
	char node[MAXVARLEN];

	if (t0==NULL) return NULL;
	switch (t0->node[0]) {
		case ':': 
		case '!': return NULL;
		case '&':
		case '|':
		case '=':
		case 'B':
		case '{':
		case '}':
		case '<':
		case '>':
			t1=arraygetbound(t0->left,vars,right);
			t2=arraygetbound(t0->right,vars,right);
			t3=listmerge(t1,t2); 
			if (t3!=NULL) { 
				list2cn(t3); 
				if ((right==0)&
				    (t0->node[0]!='&'&&t0->node[0]!='|'))
				     treereplace(t0,treeconst(1));
			}
			return t3;
	}

        if (i=arrayindex(t0->node)) {
                p2=n0; p1=t0->node; 
                while (*p1!='[') p1++; p1++;
                while (*p1!=']') *p2++=*p1++; *p2=0;
		t1=treesearchvarlist(t0->node,i,vars);
		if (t1==NULL) return NULL;
		p1=t1->left->node; while (*p1!='[') p1++; p1++;
        	low=atoi(p1);
        	while (isdigit(*p1)) p1++; while (!isdigit(*p1)) p1++;
        	high=atoi(p1);
		sprintf(node,"(%i{%s)&(%s{%i)",low,n0,n0,high); 
		return(string2tree(node));
        }

        return NULL;
}
/*******************************************************************/

/*******************************************************************/
tree *PROGarraycond(tree *t,int i)
{
	char node[MAXVARLEN];
	char *p0,*p1; 
	tree *t1,*t2;

	if (t==NULL) return NULL;
	if (t->node[0]=='=') {
		strcpy(node,t->left->node);
		p1=node; 
		while (*p1!='[') p1++; p0=p1+1; 
		while (*p1!=']') p1++; *p1=0;
		return treecompose("=",string2tree(p0),treenum(i));
	} else {
		t1=PROGarraycond(t->left,i);
		t2=PROGarraycond(t->right,i);
		return treecompose("|",t1,t2);
	}
}
tree *PROGreplacearraybody1construct(tree *t,char *node,int k,int m,int n)
{
	int i;
	char var[MAXVARLEN];
	char nvar[MAXVARLEN];
	tree *tvar,*tcond;
	tree *left,*right;

	strcpy(var+1,node); var[k]=0; var[0]='O';

	for (tvar=NULL,i=n;i>=m;i--) {
		tcond=PROGarraycond(t,i);
		sprintf(nvar,"%s_x_%i",var,i); left=treecompose(nvar,NULL,NULL);
		nvar[0]='V'; right=treecompose(nvar,NULL,NULL);
		if (tvar==NULL) 
			tvar=
			treecompose("|",tcond,treecompose("=",left,right));
		else 
			tvar=
			treecompose("&",
			treecompose("|",tcond,treecompose("=",left,right)),
			tvar);
	}
	return tvar;
}
tree *PROGarray1var(char *node,int m,int n)
{
	int i;
	char node1[MAXVARLEN];
	char *p1=node1;
	char *p0;
	char var[MAXVARLEN];
	char nvar[MAXVARLEN];
	tree *index,*tvar,*tcond;

	strcpy(node1,node);
	i=0; while (*p1&&*p1!='[') var[i++]=*p1++; var[i]=0;
	p0=p1+1;
	while (*p1&&*p1!=']') p1++; 
	if (*p1) { *p1=0; index=string2tree(p0); *p1=']'; } else error("array");

	for (tvar=NULL,i=n;i>=m;i--) {
		tcond=treecompose("=",treecopy(index),treenum(i));
		sprintf(nvar,"%s_x_%i",var,i);
		if (tvar==NULL) 
			tvar=
			treecompose(DANDs,tcond,treecompose(nvar,NULL,NULL));
		else 
			tvar=
			treecompose(
			BORs,
			treecompose(DANDs,tcond,treecompose(nvar,NULL,NULL)),
			tvar);
	}
	treefree(index); return tvar;
}
/*******************************************************************/
tree *PROGarray1varlist(char *node,int m,int n,tree *vd,tree *vnext)
{
	int i=0;
	char *p1=node;
	char var[MAXVARLEN];
	char nvar[MAXVARLEN];
	tree *index,*tvar,*tcond;

	while (*p1&&*p1!='[') var[i++]=*p1++; var[i]=0;

	for (tvar=NULL,i=n;i>=m;i--) {
		sprintf(nvar,"%s_x_%i",var,i);
		if (tvar==NULL) {
			tvar=treecompose(":",treecompose(nvar,NULL,NULL),
				treecopy(vd));
			if (vnext!=NULL) tvar=treecompose(";",tvar,vnext);
		} else 
			tvar=
			treecompose(
			";",
		    	treecompose(":",treecompose(nvar,NULL,NULL),
				treecopy(vd)),
		    	tvar);
	}
	return tvar;
}
/*******************************************************************/
void PROGreplacearraybody1(tree *t,char *node,int k,int m,int n)
{
	tree *t1,*t2;
	tree *vcur;
	char var[MAXVARLEN];

        if (t==NULL) return;
	if (t->node[0]!=';') { vcur=t; }
	else { PROGreplacearraybody1(t->right,node,k,m,n); vcur=t->left; } 

	strcpy(var+1,node); var[k+1]=0; var[0]='O'; 
	t1=treesearchvarall(var,arrayindex(var),vcur->right);
	t2=PROGreplacearraybody1construct(t1,node,k,m,n); treefree(t1);
	t1=treecompose("&",treecopy(vcur->right),t2);
	treereplace(vcur->right,t1); 
}       
/*******************************************************************/
void PROGreplacearraybody2(tree *t,char *node,int k,int m,int n)
{
	tree *t0;

        if (t==NULL) return;
	if (strncmp(t->node+1,node,k)==0) {
		t0=PROGarray1var(t->node,m,n);
		treereplace(t,t0); return; 
	}
        PROGreplacearraybody2(t->left,node,k,m,n);
        PROGreplacearraybody2(t->right,node,k,m,n);
}       
/*******************************************************************/
tree *PROGreplacearray(tree *body,tree *vars)
{
	int i=0,k=0;
	char *p1;
	tree *t,*vnext,*v;
	int low,high;

	
	if (vars->node[0]!=';') { vnext=NULL; v=vars; }
	else { vnext=PROGreplacearray(body,vars->right); v=vars->left; }

	p1=v->left->node;
	while (*p1&&*p1!='[') { p1++; k++; }
	if (*p1==0) { if (vnext!=NULL) vars->right=vnext; return vars; }

	p1++;
	low=atoi(p1); 
	while (isdigit(*p1)) p1++; while (!isdigit(*p1)) p1++;
	high=atoi(p1); 
	while (*p1&&*p1!='[') p1++; if (*p1) error("array");
	
	PROGreplacearraybody1(body->right->left,v->left->node,k+1,low,high);
	PROGreplacearraybody2(body,v->left->node,k+1,low,high);
	t=PROGarray1varlist(v->left->node,low,high,v->right,vnext);
	treefree(v->left); return t;
}
/*******************************************************************/
tree *PROGcleanarray(tree *t)
{
        char *p1,*p2;
        if (arrayindex(t->node)) {
                p1=t->node; p2=t->node;
                while (*p1) {
                        if (*p1=='.'&&*(p1+1)=='.') while (*p1!=']') p1++;
                        *p2++=*p1++;
                }
                *p2++=*p1++;
        }
        return t;
}
/*******************************************************************/

/*******************************************************************/
void PROGreplacecolon(tree *t)
{
        tree *t1,*t2;
        if (t->node[0]==':') {
                PROGreplacecolon(t->right);
                if (t->right->node[0]==':') {
                        t1=treecompose("&",treecopy(t->left),
                                treecopy(t->right->left));
                        t2=treecompose(":",t1,treecopy(t->right->right));
                        treereplace(t,t2);
                }
        }
}
/*******************************************************************/
void PROGreplacesemiand(tree *t)
{
        if (t->node[0]==';') t->node[0]='&';
}
/*******************************************************************/
void PROGreplacechan(tree *body,tree *vars)
{
        tree *l0;
        int j;
        char v1[MAXVARLEN],v2[MAXVARLEN];
        char *p,*q;
        tree *mylist=vars;
        tree *var1,*var2;

        while (1) {
		if (mylist->node[0]==';') l0=mylist->left; else l0=mylist;
                if (j=chanindex(l0->left->node)) {
                        p=l0->left->node; strcpy(v1,p); q=v1;
                        while (*p!='[') { p++; q++; }
                        sprintf(p,"[0..%i]",j-1); sprintf(q,"_x_");
                        sprintf(v2,"0..%i",j); 
                        var1=treecompose(v2,NULL,NULL);
                        var1=treecompose(":",treecompose(v1,NULL,NULL),var1);
                        var2=treecompose(";",var1,treecopy(mylist));
                        treereplace(mylist,var2);
                        strcpy(q,"[.]");
                        PROGreplacechanbody(body->right,v1,j);
                        PROGreplacechaninit(body->left,v1,j);
                }
                if (l0==mylist) break; mylist=mylist->right;
        }
}
/*******************************************************************/
void PROGreplacechanbody(tree *t,char *node,int j)
{
	int i;
	char *p1,*p2;
	char var[MAXVARLEN];
	char var2[MAXVARLEN];
	char node1[MAXVARLEN];
	tree *t1,*t2,*t3;

	if (t==NULL) return;
        PROGreplacechanbody(t->left,node,j);
        PROGreplacechanbody(t->right,node,j);
	if (t->node[0]==CHANW) {
		if (strcmp(t->left->node,node)!=0) return;
		strcpy(node1,node); p1=node1; while (*p1!='[') p1++; *p1=0;
		sprintf(var,"%s_x_",node1);
		t1=treecompose("<",treecompose(var,NULL,NULL),treenum(j));
		t2=treecompose("=",treecompose(var,NULL,NULL),
			treecompose("+",treecompose(var,NULL,NULL),treenum(1)));
		sprintf(var,"%s[%s_x_]",node1,node1);
		t3=treecompose("=",treecompose(var,NULL,NULL),
				   treecopy(t->right));
		treereplace(t,treecompose("&",t1,treecompose("&",t2,t3)));
	} else if (t->node[0]==CHANR) {
		if (strcmp(t->left->node,node)!=0) return;
		strcpy(node1,node); p1=node1; while (*p1!='[') p1++; *p1=0;
		sprintf(var,"%s_x_",node1);
		t1=treecompose(">",treecompose(var,NULL,NULL),treenum(0));
		t2=treecompose("=",treecompose(var,NULL,NULL),
			treecompose("-",treecompose(var,NULL,NULL),treenum(1)));
		sprintf(var,"%s[0]",node1);
		t3=treecompose("=",treecopy(t->right),
			treecompose(var,NULL,NULL));
		t3=treecompose("&",t1,treecompose("&",t2,t3));
		for (i=0;i<j-1;i++) {
			sprintf(var,"%s[%i]",node1,i);
			sprintf(var2,"%s[%i]",node1,i+1);
			t1=treecompose("=",treecompose(var,NULL,NULL),
				treecompose(var2,NULL,NULL));
			t3=treecompose("&",t1,t3);
		}
		treereplace(t,t3); 
	}
}       
/*******************************************************************/
void PROGreplacechaninit(tree *t,char *node,int j)
{
	int i;
	char *p1;
	char var[MAXVARLEN];
	tree *t1,*t2;

	p1=node; while (*p1!='[') p1++; *p1=0;
	sprintf(var,"%s_x_",node);
	t2=treecompose("=",treecompose(var,NULL,NULL),treenum(0));
	for (i=0;i<j;i++) {
		sprintf(var,"%s[%i]",node,i);
		t1=treecompose("=",treecompose(var,NULL,NULL),treenum(0));
		t2=treecompose("&",t1,t2);
	}
	treereplace(t,treecompose("&",t2,treecopy(t)));
}       
/*******************************************************************/
tree *PROGreplacesynnotations(tree *prog)
{
	listtraverse(prog->right->right->left,PROGreplacecolon);
        treedopostvoid(prog->right->left,PROGreplacesemiand);
        PROGreplacechan(prog->right,prog->left);
	return prog;
}
/*******************************************************************/




/*******************************************************************/
#define nnfsimp(a)	a=treedopost(TLFnnf(a),TLFnnfsimp)
/*******************************************************************/
#define lgbprinitppr(i) \
	lgbpr->prscnt=i; \
	lgbpr->prs=(process **)malloc((lgbpr->prscnt+1)*sizeof(process *)); \
	for (j=0;j<lgbpr->prscnt;j++) \
	lgbpr->prs[j]=(process *)malloc(1*sizeof(process)); \
	lgbpr->prs[lgbpr->prscnt]=NULL; \
/*******************************************************************/
#define lgbprinittslist(ppr,t) \
	ppr=prscr(listlength(t)); tj=t; \
	for (j=0;tj->node[0]==';';j++,tj=tj->right) ppr->tslist[j]=tj->left; \
	ppr->tslist[j]=tj; \
/*******************************************************************/
#define addcmdargv(k) sprintf(cmd+strlen(cmd),"%s ",argv[k])
/*******************************************************************/
ggbase *f2p0(char *fn,int opck,char *opckv,char **argv,int argc)
{
	int i=0,j,k;
	char *string;
	char cmd[100];
	module *mm,*mm0;
        tree *prog,*clktr,*tj;
	ggbase *lgb=(ggbase *)calloc(1,sizeof(ggbase)); /*sizeof(ggb)???*/
	prg *p1;


	string=PREPfile2txt(fn); 
	if (hasint(string)) 
	for (j=0,i=1;i<=31;i++) {
/*
		sprintf(cmd,"%s -ck i%i %s > .log%i.tmp",argv0,i,fn,i);
*/
		sprintf(cmd,"%s -ck i%i ",argv[0],i);

		for (k=0;k<argc;k++) {
			if (strcmp(argv[k],"-c")==0) { k++; continue; }
			else if (strcmp(argv[k],"-sp")==0) { k++; continue; }
			else if (strcmp(argv[k],"-fsp")==0) { k++; continue; }
			else if (strncmp(argv[k],"-D",2)==0) addcmdargv(k); 
			else if (strncmp(argv[k],"-f",2)==0) addcmdargv(k); 
			else if (strncmp(argv[k],"-O",2)==0) addcmdargv(k); 
		}
		sprintf(cmd+strlen(cmd),"%s > .log%i.tmp",fn,i);
/*
		infos(cmd);
*/
		j=system(cmd);
		if (j==256) break;
	}
	if (i>31) error("int too big"); 
	printf("INFO:       int=i%i\n",i);
	ireplacestring(string,i); 

	mm=PROGtxt2modules(string); 

	for (mm0=mm;mm0!=NULL;mm0=mm0->next) {
		PROGreplacefor(mm0->prog->right);
	}

	switch (opck) {
		case -1: arraycheckmodule(mm); break;
		case -2: *opckv=7+AGcheckmodule(mm,opckv); break;
	}

	lgb->mm=mm; lgb->pr=(prg *)calloc(1,sizeof(prg));

	prog=PROGmerge(mm,lgb->pr); 
	prog=PROGreplacesynnotations(prog);

 if (opck==-3) {
	FILE *g;
	addvar2prog(prog,"err_x_");
 	if ((clktr=intcheckvlist(mm,prog,NULL))!=NULL) {
	    addtrs2ggb(lgb,clktr);
	    prog->right->right->left=listmerge(prog->right->right->left,clktr);
	}
	logtmptree("f2p trs",prog->right->right->left);
 }
 if ((clktr=clkcheckvlist(prog))!=NULL) {
	addtrs2ggb(lgb,clktr);
	prog->right->right->left=listmerge(prog->right->right->left,clktr);
 }

	lgb->file=fn; 
	lgb->string=NULL; 
	lgb->pr->prog=prog; 
	lgb->pr->vlist=treecopy(lgb->pr->prog->left);
/*
 infos("prog"); treew(prog);
*/
	ggbtssplitt(lgb->pr,lgb->mm); 

	fnessaddprog(lgb->pr); 


	free(string); return lgb;
}
/*******************************************************************/
void f2p1INFO(ggbase *lgb)
{
	module *mm;
/*
	if (lgb->pr->vlist!=NULL) treefree(lgb->pr->vlist);
	lgb->pr->vlist=treecopy(lgb->pr->prog->left);
*/

        lgb->pr->varconstr=INFOconstr_a(lgb->pr->prog->left);
        lgb->pr->booln=INFOvlist(lgb->pr->prog->left,0);

        lgb->pr->puretrans=INFOpuret(lgb->pr->prog->right->right->left);
	for (mm=lgb->mm;(mm!=NULL)&&(mm->type!='P');mm=mm->next);
	if ((mm!=NULL)&&(mm->type=='P')) lgb->pr->puretrans=0;

	deadlocktradd(lgb); nnfsimp(lgb->pr->deadlocktr);
        lgb->pr->deadlockfreecond=lgb->pr->deadlocktr->left;
}
/*******************************************************************/
void f2p1(tree *prog1)
{
	tree *t0;
	prog1=PROGreplacemarray(prog1);
	PROGaddeq(prog1->right->right->left,prog1->left);
	prog1->right=treedopost(prog1->right,PROGcleanarray);
	PROGaddnextv(prog1,1);
	prog1->left=PROGreplacearray(prog1->right,prog1->left);
	PROGaddnextv(prog1,0);

	PROGreplacem1(prog1->right,prog1->left);
	PROGreplacestarORcond(prog1->right->right->left,prog1->left,0);

	prog1->right=TLFnnf(prog1->right); 
	prog1->right=treedopost(prog1->right,TLFnnfsimp);
/*
        nnfsimp(prog1->right); 
*/

}
/*******************************************************************/
void deadlocktradd(ggbase *lgb)
{
	tree *t0=INFOdeadlockfreecond(lgb->pr->prog->right->right->left);
	lgb->pr->deadlocktr=treecompose(":",treecompose("!",NULL,t0),NULL);
	PROGaddeq(lgb->pr->deadlocktr,lgb->pr->prog->left);
	PROGaddnext(lgb->pr->deadlocktr);
	PROGaddvc(lgb->pr->deadlocktr,lgb->pr->prog->left); 
}
/*******************************************************************/
tree *PROGmerge(module *mm,prg *lgbpr)
{
	int lc=0;
	tree *var,*init,*trans,*specs;
	tree *t0,*t1,*t2,*tj;
	char ni[64];
	int j;

	mm->prog->right->right->right=
		specmergeg1(mm->prog->right->right->right);
	if (mm->next==NULL||mm->next->type!='M') { 
		t0=treecopy(mm->prog);
		lgbprinitppr(1);
		lgbprinittslist(lgbpr->prs[0],t0->right->right->left);
		return t0;
	}

	var=treecopy(mm->prog->left);
	init=treecopy(mm->prog->right->left);
	trans=NULL;
	specs=treecopy(mm->prog->right->right->right);
	t0=mm->prog->right->right->left; 
	lgbprinitppr(listlength(t0));

	while ((t1=listgetone(t0,lc++))!=NULL) {
		t2=treecopyreplace(t1,mm->next);

                sprintf(ni,"%3i",lc-1);
                if (ni[0]==' ') ni[0]='0'; if (ni[1]==' ') ni[1]='0';
 		replacepid1(t2,ni,PIDkw);

		var=listmerge(var,t2->left);
		init=treecompose("&",init,t2->right->left);
		trans=listmerge(trans,t2->right->right->left);
		lgbprinittslist(lgbpr->prs[lc-1],t2->right->right->left);
		specs=specmerge(specs,t2->right->right->right,lc-1);
	}
	if (specs->right==NULL) 
	   specs->right= treecompose("=",treenum(1),treenum(1));

	return PROGcompose(var,init,trans,specs);
}
/*******************************************************************/
prg *prog2prg(tree *prog,module *mm)
{
	int j;
	tree *tj;
	prg *lgbpr;

        lgbpr=(prg *)calloc(1,sizeof(prg));
        lgbpr->prog=prog; lgbpr->prscnt=1;

	lgbprinitppr(1);
	lgbprinittslist(lgbpr->prs[0],prog->right->right->left);

 if (clkcheckvlist(prog)!=NULL) { error("clock in procedure"); }

	ggbtssplitt(lgbpr,mm);

	lgbpr->vlist=treecopy(prog->left);

	f2p1(lgbpr->prog); 

        lgbpr->varconstr=INFOconstr_a(lgbpr->prog->left);
        lgbpr->booln=INFOvlist(lgbpr->prog->left,0);

        lgbpr->puretrans=INFOpuret(lgbpr->prog->right->right->left);
	for (;(mm!=NULL)&&mm->type!='P';mm=mm->next);
	if ((mm!=NULL)&&(mm->type=='P')) lgbpr->puretrans=0;

        lgbpr->deadlockfreecond=NULL;

/*
 infos("PC"); treewrite(lgbpr->prog,stdout);
*/
	return lgbpr;
}
/*******************************************************************/
#define progvar         prog->left
#define proginit        prog->right->left
#define progtrs         prog->right->right->left
#define progfness       prog->right->right->right->left
#define progspec        prog->right->right->right->right
/*******************************************************************/
void addvar2prog(tree *prog,char *s)
{
	tree *a1;

	a1=treecompose("=",treestr(s),treenum(0));
       	proginit=listmerge(proginit,a1); list2cn(proginit);
       	a1=treecompose(":",treestr(s),treestr("0..1"));
       	progvar=listmerge(progvar,a1);
       	a1=treecompose("=",treestr(s),treenum(0));
       	a1=treecompose("A",NULL,treecompose("G",NULL,a1));
       	progspec=listmerge(a1,progspec);
}
/*******************************************************************/
void addtrs2ggb(ggbase *lgb,tree *trs)
{
	int j;
	tree *tj;
	prg *p1=lgb->pr; 

	p1->prscnt++;
	p1->prs=(process **)realloc(p1->prs,(p1->prscnt+1)*sizeof(process *)); 
	lgbprinittslist(p1->prs[p1->prscnt-1],trs);
        p1->prs[p1->prscnt]=NULL;
}
/*******************************************************************/
int hasint(char *s)
{
        while (*s) { if (strncmp(s,":int",4)==0) return 1; s++; }
        return 0;
}
/*******************************************************************/
void ireplacestring(char *s,int i)
{
	char a,b;

	if (i==0) return;
	if (i<10) { a='0'; b='0'+i; } else { a='1'; b='0'+i-10; } 

	while (*s) { if (strncmp(s,":int",4)==0) { *(s+2)=a; *(s+3)=b; }; s++; }
}
/*******************************************************************/


/*******************************************************************/
#define pspec	prog->right->right->right
/*******************************************************************/
void fnesscheck(tree *prog)
{
	tree *fness;

	if (pspec->node[0]!='#') { error("checkfness"); } 
	fness=pspec->left; reprunning2(fness,-1); 
}
/*******************************************************************/
tree *fnessget(tree *prog)
{
	tree *p0;
	tree *fness;

	if (pspec->node[0]!='#') { error("checkfness"); }
	fness=pspec->left; p0=pspec; pspec=pspec->right; free(p0);

	return fness;
}
/*******************************************************************/
tree *fnessaddvlist(tree *list,int n)
{
	tree *t0;
	char v0[64],v1[64];
	
	sprintf(v0,PIDs); sprintf(v1,"%i..%i",0,n-1); 
	t0=treecompose(":",treecompose(v0,NULL,NULL),treecompose(v1,NULL,NULL));

	return listmerge(list,t0);
}
/*******************************************************************/
tree *fnessaddilist(tree *list,int n)
{
	tree *t0;
	char v1[64];
	
	sprintf(v1,"%i",0); 
	t0=treecompose("=",
		treecompose(PIDs,NULL,NULL),treecompose(v1,NULL,NULL));
	t0=treecompose("&",t0,treecopy(list)); treereplace(list,t0);
	return list;
}
/*******************************************************************/
void fnessaddtlist(prg *pr)
{
	int i,j;
	tree *tr1;

        for (i=0;i<pr->prscnt;i++) 
       	for (j=0;j<pr->prs[i]->n;j++) fnessaddt(tr1=pr->prs[i]->tslist[j],i);
}
/*******************************************************************/
void fnessaddt(tree *list,int n)
{
	tree *t0;
	char v1[64];
	
	sprintf(v1,"%i",n); 
	t0=treecompose("=",
		treecompose(PIDs,NULL,NULL),treecompose(v1,NULL,NULL));
	list->right=treecompose("&",t0,list->right);
}
/*******************************************************************/
void *fnessaddprog(prg *pr)
{
	int n;
	tree *tn;
	tree *prog=pr->prog;

	fnesscheck(prog);
	tn=treesearch(PIDs,0,pspec->left); if (tn==NULL) return;
	prog->left=fnessaddvlist(prog->left,pr->prscnt);
	prog->right->left=fnessaddilist(prog->right->left,pr->prscnt);
	fnessaddtlist(pr);
}
/*******************************************************************/

/*******************************************************************/
int PROGisfor(tree *v)
{
	tree *t0,*t1,*t2;

	if (v->node[0]!=':') return 0;

	if (v->left==NULL&&v->right->node[0]==':') 
	if (strncmp(v->right->left->node,"for",3)==0) return 1; /*not used*/

	if (strncmp(v->left->node,"for",3)==0) return 2;

	return 0;
}
/*******************************************************************/
void PROGreplacefor(tree *v)
{
	int i,j;
	tree *t,*t0;
	tree *t1;
	char *x;
	char y[64];
	char *s0;
	int isfor=1;

	if (v==NULL) return;
	switch (PROGisfor(v)) {
		case 1: x=v->right->left->node+3; t=v->right->right; break;
		case 2: x=v->left->node+3; t=v->right; break;
		default: isfor=0; break;
	}

	if (isfor) {
		s0=x;
		while (1) if (strncmp(s0,"in[",3)) s0++; else break;
		*s0++=0;
		t0=treeconst(1);
		for (i=ran(s0,0),j=ran(s0,1);i<=j;i++) {
			sprintf(y,"%i",i);
			t1=treecopy(t); treereplacestring(t1,x,y);
			t0=treecompose("&",t0,t1);
		}
		treereplace(v,t0);
	}

	PROGreplacefor(v->left); PROGreplacefor(v->right); 
}
/*******************************************************************/
tree *listflatten(tree *v)
{
	tree *t0,*t1;

	if (v->node[0]=='&'||v->node[0]==';') {
		t0=listflatten(v->left);
		t1=listflatten(v->right);
		v->left=NULL; v->right=NULL; 
		treereplace(v,listmerge(t0,t1));
	}
	if (v->node[0]=='^') { free(v); return NULL; }
	if (v->left==NULL&&v->node[0]==':') { t1=v->right; free(v); return t1; }
	return v;
}
/*******************************************************************/
int PROGisstar(tree *v)
{
	if (v->node[0]!='=') return 0; 
	return v->right->node[0]==0;
}
/*******************************************************************/
int PROGism1(tree *v)
{
	switch (v->node[0]) {
		case '=': 
		case 'B': 
		case '>': 
		case '<': 
		case '}': 
		case '{': break;
		default: return 0;
	}

	if (!(v->right->left==NULL&&v->right->node[0]=='-')) return 0; 
	return atoi(v->right->right->node)==1;
}
/*******************************************************************/
void PROGreplacestarORcond(tree *v,tree *vlist,int cond)
{
	tree *t1;
	int i,j;
	int c=0;
	char nd[MAXVARLEN];
	
	if (v==NULL) return;
	if (PROGisstar(v)) {
        	t1=treesearchvarlist(v->left->node+1,0,vlist);
		i=ran(t1->right->node,0);
		j=ran(t1->right->node,1);
        	t1=treevarconstr(v->left->node,i,j);
		treereplace(v,t1); return;
	}
	switch (v->node[0]) {
		case '=': return;
		case '>':
		case '}':
		case '<':
		case '{':
		case 'B': c=1; break;
		default:  if (cond&&v->node[0]=='V') { v->node[0]='O'; return; }
	}


	if (v->node[0]!=':') PROGreplacestarORcond(v->left,vlist,c); 
	PROGreplacestarORcond(v->right,vlist,c); 

}
/*******************************************************************/
void PROGreplacem1(tree *v,tree *vlist)
{
	tree *t1;
	int j;

	if (v==NULL) return;
	PROGreplacem1(v->left,vlist); PROGreplacem1(v->right,vlist); 
	if (!PROGism1(v)) return;

/*
 treewrite(v,stdout); infos("m1");
*/

        t1=treesearchvarlist(v->left->node+1,0,vlist);
	j=ran(t1->right->node,1);
	treereplace(v->right,treenum(j));
}
/*******************************************************************/

/*******************************************************************/
int INFOvlist_count(tree *v)
{
        if (v==NULL) return 1;
        return INFOvlist_count(v->left)+INFOvlist_count(v->right);
}
int INFOvlist(tree *v,int p)
{
        tree *tvar,*tdomain;
        int i,j;
	char low[MAXVARLEN],high[MAXVARLEN];
	char *p1;

        if (v==NULL) error("tree");
        if (v->node[0]==';') { tvar=v->left->left; tdomain=v->left->right; }
        else if (v->node[0]==':') { tvar=v->left; tdomain=v->right; }
	else error("teee");

	if (tvar->left!=NULL) free(tvar->left);
	if (tvar->right!=NULL) free(tvar->right);

	if (tdomain->left==NULL&&tdomain->right==NULL) {
		i=1;
		p1=tdomain->node;
		while (isdigit(*p1)) p1++; 
		if (*p1++=='.'&&*p1++=='.') i+=atoi(p1);
	} else {
		i=INFOvlist_count(tdomain)/2;
	} 

	i=i-1; j=1; while ((i=i/2)>0) j++;
	sprintf(low,"%i",p+1);
	sprintf(high,"%i",p+j);
	tvar->left=treecompose(low,NULL,NULL);
	tvar->right=treecompose(high,NULL,NULL);

        if (v->node[0]==';') return INFOvlist(v->right,p+j); 
	return  p+j;
}
/*******************************************************************/
int INFOpuret(tree *t)
{
        if (t==NULL) return 1;
        if (t->node[0]==';') return INFOpuret(t->left)&&INFOpuret(t->right);
        if (t->node[0]==':') return INFOpuret(t->right);
        if (t->node[0]=='&') return INFOpuret(t->left)&&INFOpuret(t->right);
        if (t->node[0]=='=') if (t->left->node[0]=='O') return 1; else return 0;
        return 1;
}
/*******************************************************************/
tree *INFOdeadlockfreecond(tree *t)
{
	int lc=0;
        tree *t0,*l0;

	t0=NULL;
	while ((l0=listgetone(t,lc++))!=NULL) if (l0->left!=NULL) {
		if (t0==NULL) t0=treecopy(l0->left);
		else t0=treecompose("|",t0,treecopy(l0->left));
        }
        return t0;
/*
	l0=listgetone(t,lc++); 
	if (l0!=NULL) return TLFnegate(treecopy(l0->left)); return NULL;
*/
}
/*******************************************************************/
tree *INFOconstr_a(tree *v)
{
        tree *tvar,*tdomain,*t;
        int i;
	char *p1;
	char node[MAXVARLEN];

        if (v==NULL) error("tree");
        if (v->node[0]==';') { tvar=v->left->left; tdomain=v->left->right; }
        else if (v->node[0]==':') { tvar=v->left; tdomain=v->right; }
	else error("teee");

	if (tdomain->left==NULL&&tdomain->right==NULL) {
		i=1;
		p1=tdomain->node;
		while (isdigit(*p1)) p1++; 
		if (*p1++=='.'&&*p1++=='.') i+=atoi(p1);
	} else { i=INFOvlist_count(tdomain)/2; } 
	
	strcpy(node+1,tvar->node); node[0]='V';
	t=neg(treecompose(">",treecompose(node,NULL,NULL),treenum(i-1)));

        if (v->node[0]==';') return treecompose("&",t,INFOconstr_a(v->right)); 
	return  t;
}
/**
tree *constr(tree *v)
{
        tree *tvar,*tdomain,*t;
        int i;
	char *p1;

        if (v==NULL) error("tree");
        if (v->node[0]==';') { tvar=v->left->left; tdomain=v->left->right; }
        else if (v->node[0]==':') { tvar=v->left; tdomain=v->right; }
	else error("teee");

	if (tdomain->left==NULL&&tdomain->right==NULL) {
		i=1;
		p1=tdomain->node;
		while (isdigit(*p1)) p1++; 
		if (*p1++=='.'&&*p1++=='.') i+=atoi(p1);
	} else { i=INFOvlist_count(tdomain)/2; } 
	t=neg(treecompose(">",treecopy(tvar),treenum(i-1)));

        if (v->node[0]==';') return treecompose("&",t,constr(v->right)); 
	return  t;
}
/*******************************************************************/

/*******************************************************************/
void PROGmarraycount(char *node,int *ma)
{
	*ma=0;
	while (*node)
	if (*node=='[') {
			while (*node!='.') node++; 
			while (!isdigit(*node)) node++;
			ma[++*ma]=atoi(node)+1;
	} else node++;
}
/*******************************************************************/
tree *PROGreplacemarray(tree *prog)
{
	int lc=0;
	int i,j,mb;
	int ma[32]; 
	tree *l0;
	char *p1;
	tree *body,*vars;

	body=prog->right;	
	vars=prog->left;
	while ((l0=listgetone(vars,lc++))!=NULL) 
	if (j=marrayindex(l0->left->node)) {
		PROGmarraycount(l0->left->node,ma);
		for (mb=1,i=1;i<=ma[0];i++) mb*=ma[i];
		p1=l0->node;
		while (*p1!='[') p1++; 
		sprintf(p1,"[0..%i]",mb-1); 
		PROGreplacemarraybody(body,l0->left->node,j,ma);
	}
	return prog;
}
/*******************************************************************/
void PROGreplacemarraybody(tree *t,char *node,int j,int *ma)
{
	int i;
	char *p1,*p2;
	char var[MAXVARLEN];

	if (t==NULL) return;
        PROGreplacemarraybody(t->left,node,j,ma);
        PROGreplacemarraybody(t->right,node,j,ma);
	if (strncmp(t->node,node,j)!=0) return;

	p2=t->node+j-1; 
	strcpy(var,t->node); p1=var+j; 
	for (i=1;i<*ma;i++) *p1++='(';
	for (i=0;i<*ma;i++) {
		if (i>0) { sprintf(p1,")*%i+",ma[i+1]); p1+=strlen(p1); }
		p2++; 
		if (i>0) *p1++='('; 
		while (*p2!=']') *p1++=*p2++; 
		if (i>0) *p1++=')';
		p2++;
	}
	*p1++=']'; *p1=0;
	strcpy(t->node,var);
}       
/*******************************************************************/

/*******************************************************************/
#define newmp(p)	(*p&&(*p!='M')&&(strncmp(p,"PROCEDURE",9)!=0))
/*******************************************************************/
void reppar(char *s) 
{
	for (;*s;s++) 
	if (*s=='('&&*(s+1)==')'&&*(s+2)==';') { *(s)=':'; *(s+1)='1'; }
}
/*******************************************************************/
tree *PROGcheck(tree *t)
{
        tree *t0;

        t0=t->right->right;
        if (t0->node[0]!='#') t->right->right=treecompose("#",t0,NULL);
        t0=t->right->left;
        if (t0==NULL) t->right->left=treecompose("=",treenum(1),treenum(1));
        return t;
}
/*******************************************************************/
tree *PROGcompose(tree *t0,tree *t1,tree *t2,tree *t3)
{
        return treecompose("#",t0,treecompose("#",t1,treecompose("#",t2,t3)));
}
/*******************************************************************/
void PROGgetmain(char *p2,module *mm,int last)
{
	char *p0=p2;

	if (!last) {
		reppar(p0);
		while (*p0!='#') p0++; p0++;
		while (*p0!='#') p0++; p0++;
		stringreplacechar(p0,'(',':');
		stringreplacechar(p0,')',' ');
		stringreplacechar(p0,',','|');
		stringcompress(p2,' ',' '); 
	}
	mm->type='V';
	mm->parameters=NULL;
	mm->prog=PROGcheck(string2tree(p2)); 
}
void PROGgetmodule(char *p2,module *mm,int c)
{
	char *p0=p2;

	while (*p0&&*p0!=')') p0++; 
	*p0=0;
	stringreplacechar(p2,'(',':');
	stringreplacechar(p2,')',';');
	stringreplacechar(p2,',','|');

	mm->type=(char)('G'+c);
	mm->parameters=string2tree(p2);
	if (mm->parameters->node[0]!=':') 
	mm->parameters=treecompose(":",mm->parameters,treenum(1)); /*reppar*/
	mm->prog=PROGcheck(string2tree(p0+1)); 
}
module *PROGtxt2modules(char *s)
{
	module *mm,*mm0;
	char *p1,*p2;
	int last=0;
	int nextp=0;
	int mp;

	p1=s; 

	if (strncmp(p1,"MAIN",4)==0) p1+=4;
	else if (strncmp(p1,"VVM",3)==0) p1+=3;
	else error("VVM must be specified at the beginning of the input file.");

	p2=p1; 
	while (1) {
		while (newmp(p1)) p1++; if (*p1==0) last=1;
		nextp=(strncmp(p1,"PROCEDURE",9)==0);
			*p1=0; 
			mm=(module *)malloc(sizeof(module));
			PROGgetmain(p2,mm,last||nextp); 
			if (last) { mm->next=NULL; return mm; }
			if (strncmp(p1+1,"ODULE",5)==0) mp=6; else mp=9;
			p1+=mp; p2=p1; break;
	}
	mm0=mm; 
	while (1) {
		while (newmp(p1)) p1++; if (*p1==0) last=1;
			*p1=0; 
			mm->next=(module *)malloc(sizeof(module));
			mm=mm->next;
			PROGgetmodule(p2,mm,mp);
			if (last) { mm->next=NULL; return mm0; }
			if (strncmp(p1+1,"ODULE",5)==0) mp=6; else mp=9;
			p1+=mp; p2=p1; 
	}
}
/*******************************************************************/
tree *treecopyreplace(tree *spec,module *ms)
{
	char *s0;
	tree *prog;
	tree *pname=spec->left;
	tree *mname=spec->right->left;
	tree *t1,*t2;

	while (ms!=NULL) {
	  if (mname==NULL) break;
	  if (strcmp(ms->parameters->left->node,mname->node)) ms=ms->next;
	  else break;
	}
	if (ms==NULL) error("module");

	prog=treecopy(ms->prog);

	if (prog->left==NULL) { 
		t1=ms->parameters->right; t2=spec->right->right;
	} else {
		t1=treecompose(":",NULL,NULL); t2=treecompose(":",NULL,NULL);
		constructt1t2(pname->node,prog->left,t1,t2);
		t1=treecompose("|",t1,ms->parameters->right);
		t2=treecompose("|",t2,spec->right->right);
	}
	if (t1!=NULL) { treereplacecleanname(t1); treereplacecleanname(t2); }
	treereplacenames(prog,t1,t2);


	return prog;
}
void constructt1t2(char *pname,tree *vars,tree *t1,tree *t2)
{
	tree *a1,*a2,*a3,*a4;
	char *p;

	if (vars->node[0]==':') {
		strcpy(t1->node,vars->left->node);
		p=t1->node; 
		while (*p&&*p!='[') p++; if (*p=='[') { p++; *p++=']'; *p=0; }
/*
*/
		sprintf(t2->node,"%s.%s",pname,t1->node); return;
	}
	a1=treecompose(":",NULL,NULL); a2=treecompose(":",NULL,NULL);
	a3=treecompose(":",NULL,NULL); a4=treecompose(":",NULL,NULL);
	constructt1t2(pname,vars->left,a1,a2);
	constructt1t2(pname,vars->right,a3,a4);
	treereplace(t1,treecompose("|",a1,a3));
	treereplace(t2,treecompose("|",a2,a4));
}
void treereplacenames(tree *prog,tree *t1,tree *t2)
{
	int i;
	if (prog==NULL) return;
	treereplacenames(prog->left,t1,t2);
	treereplacenames(prog->right,t1,t2);
	if (isvarconst(prog->node)) {
		if (i=arrayindex(prog->node)) { /*par*/
			treereplacename(prog,t1,t2,1);
			i=arrayindex(prog->node);
			treereplacenamearray(prog->node,i,t1,t2);
		} else treereplacename(prog,t1,t2,0);
	}
}
int treereplacename(tree *t,tree *t1,tree *t2,int fl)
{
	if (t1==NULL) return;
	if (isvarconst(t1->node)) {
		if (fl==0) return nodereplace(t,t1,t2);
/*
		return stringreplacestring(node,t1->node,t2->node);
*/
		return stringreplacestringfixlength(t->node,t1->node,t2->node);
	}
	if (!treereplacename(t,t1->left,t2->left,fl)) 
	return treereplacename(t,t1->right,t2->right,fl); 
	return 0;
}
void treereplacecleanname(tree *t1)
{
	char *p1;
	if (t1==NULL) return;
	if (t1->node[0]=='|') {
		treereplacecleanname(t1->left);
		treereplacecleanname(t1->right); return;
	}
	p1=t1->node; while (*p1&&*p1!='[') p1++;
	if (*p1&&*(p1+1)==']') *(p1+1)=0;
}
void treereplacenamearray(char *node,int i,tree *t1,tree *t2)
{
	char *s0;
	char tmp[MAXVARLEN],tmp1[MAXVARLEN],tmp2[MAXVARLEN];
	tree *t0;
	int j;

	strcpy(tmp,node); 
	s0=tmp+i; while (*s0!=']') s0++; *s0=0;

	strcpy(tmp2,tmp); 
	t0=string2tree(tmp+i);
	treereplacenames(t0,t1,t2);
	treewrites(t0,tmp1,0);
	stringreplacestringfixlength(node+i,tmp2+i,tmp1);
	j=i+strlen(tmp1)+1;
	if (*(node+j)=='[') treereplacenamearray(node,j+1,t1,t2);
}
/*******************************************************************/
tree *specmergeg1(tree *s1)
{
	tree *s1l,*s1r;
	if (s1==NULL) { 
		s1l=NULL; 
		s1r=NULL; 
	} else if (s1->node[0]=='#') {
                s1l=treecopy(s1->left->right);
                s1r=s1->right; treefree(s1->left); free(s1);
        } else if (s1->node[0]==';'&&s1->left->node[0]=='=') {
		s1l=s1->right;
		s1r=NULL; treefree(s1->left); free(s1);
	} else {
		s1l=NULL;
		s1r=s1;
	}
	return treecompose("#",s1l,s1r);
}
tree *specmerge(tree *s1,tree *s2,int n)
{
	tree *t1;
	char id[64];
	int lc=0;

	s2=specmergeg1(s2); 
	reprunning2(s2->left,n);
/*
	if (s2->left!=NULL) {
	 for (t1=s2->left;t1->node[0]==';';t1=t1->right) reprunning(t1->left,n);
	 reprunning(t1,n); 
	}
*/
	t1=treecompose("#",
		listmerge(s1->left,s2->left), listmerge(s1->right,s2->right));
	free(s1); free(s2);
 
	return t1;
}
void reprunning(tree *t1,int n)
{
	char id[64];

	if (strcmp(t1->node,"running")==0) {
		sprintf(id,"%i",n);
		treereplace(t1,
		treecompose("=",treecompose(PIDs,NULL,NULL),
		treecompose(id,NULL,NULL)));
	}
}
void reprunning2(tree *t1,int n)
{
	char id[64];

	if (t1==NULL) return;
	if (strcmp(t1->node,"running")==0) {
		if (n>=0) {
			sprintf(id,"%i",n);
			treereplace(t1,
			treecompose("=",treecompose(PIDs,NULL,NULL),
			treecompose(id,NULL,NULL)));
		} else {
			treereplace(t1,treecompose("=",treenum(1),treenum(1)));
		}
	}
	reprunning2(t1->left,n); reprunning2(t1->right,n);
}
/*******************************************************************/
#define separator(c)    (!(isalnum(c)||c=='_'))
/*******************************************************************/
void replacepid1(tree *ll,char *ni,char *old)
{
        char *node;
	int i,k;

        if (ll==NULL) return;
        replacepid1(ll->left,ni,old); replacepid1(ll->right,ni,old);
        if (strcmp(ll->node,old)==0) { strcpy(ll->node,ni); return; }
	
        if (strcmp(old,PIDkw)==0)
        for (node=ll->node+1,i=strlen(node),k=3;k<=i;node++,k++) {
        	if (strncmp(node,PIDkw,3)==0&&separator(node[3]))
        	{ memcpy(node,ni,3*sizeof(char)); k+=3; node+=3; }
	}
}
/*******************************************************************/
int PROGchecktrans(char *p2)
{
	for (;*p2;p2++) if (strncmp(p2,"TRANS",5)==0) return 1; return 0;
}
/*******************************************************************/

/*******************************************************************/
#define isconstant(node)   	(node[0]=='C')
/********************************************************************
* constants are of the form Cs0wait,Cs1think                        *
/*******************************************************************/
#define addnextvc(x,t)		newnode[0]=x; \
				strcpy(newnode+1,t->node); \
				strcpy(t->node,newnode); \
/*******************************************************************/
void PROGaddnext_assign(tree *t) 
{
	char newnode[MAXVARLEN];

	if (t==NULL) return;
	if (t->node[0]=='='&&!isdigit(t->left->node[0])) { 
		addnextvc('O',t->left); return; 
	} 
	PROGaddnext_assign(t->left); PROGaddnext_assign(t->right); 
}
void PROGaddnext(tree *t) 
{
	if (t==NULL) return;
	if (t->node[0]==':') { PROGaddnext_assign(t->right); return; }
	PROGaddnext(t->left); PROGaddnext(t->right);
}
/*******************************************************************/
void PROGaddvc(tree *t,tree *v) 
{
	char x;
	char newnode[MAXVARLEN];

	if (t==NULL) return;
	if (isvarconst(t->node)) { 
	  x=((treesearchvarlist(t->node,arrayindex(t->node),v)==NULL)?'C':'V');
	  addnextvc(x,t); 
	  return; 
	}
	PROGaddvc(t->left,v); PROGaddvc(t->right,v); return;
}
/*******************************************************************/
void PROGreplaceconstants(tree *t)
{
        char *p,*q;

        if (t==NULL) { return; }
        if (isconstant(t->node)) {
		if (vdslog!=NULL) 
		fprintf(vdslog,"\nREPLACED CONSTANTS: %s\n",t->node);
                p=q=t->node;
                while (!isdigit(*p)) p++;
                while (isdigit(*p)) *q++=*p++; *q=0; return;
        }
        PROGreplaceconstants(t->left); PROGreplaceconstants(t->right); return;
}
/*******************************************************************/
void PROGaddnextv(tree *prog,int addnext) 
{
	if (addnext) PROGaddnext(prog->right->right->left); 
	PROGaddvc(prog->right,prog->left); 
	PROGreplaceconstants(prog->right);
}
/*******************************************************************/




/*******************************************************************/
 extern OPT op0;
/*******************************************************************/
module *isnodeprocedure(char *s,module *mm)
{
 	for (;mm!=NULL;mm=mm->next) 
       	if (mm->type=='P') 
	if (strcmp(s,mm->parameters->left->node)==0) { return mm; }
	return NULL;
}
/*******************************************************************/
tree *procedurecreate(tree *phead,module *mm,tree *vlist,int ag)
{
	module *mm0;
        tree *var,*init,*trans,*specs;
	tree *t0,*t2;
	char pname[64];
	char ni[64];
	char rname[64]=RETs;
 static pcount=0;
	tree *pcall,*prog;
	tree *t2a,*t2b;

	mm0=isnodeprocedure(phead->left->node,mm);
	strcpy(pname,phead->left->node);
        sprintf(ni,"%3i",pcount); pcount++;
        if (ni[0]==' ') ni[0]='0'; if (ni[1]==' ') ni[1]='0'; 
	strcat(pname,"_x_"); strcat(pname,ni);
	pcall=treecompose(":",treecompose(pname,NULL,NULL),phead);

	t2=treecopyreplace(pcall,mm0); 
	strcat(rname,ni); replacepid1(t2,rname,RETkw); 

        var=listmerge(treecopy(vlist),treecopy(t2->left));
        sprintf(ni,"%i..%i",0,1);
        t0=treecompose(ni,NULL,NULL),
        t0=treecompose(":",treecompose(rname,NULL,NULL),t0); 
        var=listmerge(var,t0);

        init=t2->right->left;
        trans=t2->right->right->left;

	if (ag==-1||t2->right->right->right==NULL) {
		specs=treecompose(";",NULL,NULL);
        	prog=PROGcompose(var,init,trans,specs);
		if (op0.ck==-3) 
       		prog->right->right->left
		=listmerge(prog->right->right->left,intcheckvlist(mm,prog,rname));
	} else {
		t2a=treecopy(t2->right->right->right->left); 
        	t2b=treecopy(t2->right->right->right->right); 
		if (t2a->node[0]!=';'&&t2b->node[0]!=';') {
			free(t2a); t2a=NULL; free(t2b); t2b=NULL;
		} else {
			AGcleanassumption(t2a);
			list2cn(t2a); list2cn(t2b); 
			specs=treecompose(";",t2a,t2b);
			treefree(var); var=treecopy(vlist);
		}
        	prog=PROGcompose(var,init,trans,specs);
	}

	return prog;
}
/*******************************************************************/
#define tscorrection(ts)     if (ts->node[0]==';') ts=ts->left;
/*******************************************************************/
void ggbtssplitt(prg *pr,module *mm)
{
	int i,k;
	process *prs;
	tree *t;

	for (k=0,prs=pr->prs[0];prs!=NULL;prs=pr->prs[k+1],k++) {
	    for (i=0;i<prs->n;i++) { tscorrection(prs->tslist[i]); }
	    for (i=0;i<prs->n;i++) { 
		t=prs->tslist[i]->right;
		if (t->node[0]=='&') {
		   if (isnodeprocedure(t->left->left->node,mm)!=NULL) {
			prs->tslistpc[i]=t->left;
			prs->tslist[i]->right=t->right; free(t);
		   }
		} else {
		   if (isnodeprocedure(t->left->node,mm)!=NULL) {
			prs->tslistpc[i]=t;
			prs->tslist[i]->right=treeconst(1); 
		   }
		}
	    }
	}
}
/*******************************************************************/
void AGrewritevo(tree *t) 
{
	if (t==NULL) return;
	if (t->node[0]=='V') { t->node[0]='O'; return; }
	if (t->node[0]==':') {
		if (t->left->node[0]!='P') error("AGrewritevo P");
		treereplace(t,treecopy(t->right)); return;
	}
	AGrewritevo(t->left); AGrewritevo(t->right); return;
}
/*******************************************************************/
tree *AGcleanassumption(tree *t) 
{
	int i,j;
	tree *varl=NULL;
	tree *v;
	tree *t1a,*t1b;

	if (t==NULL) return NULL;
	if (t->node[0]==':') {
		varl=listmerge(varl,treecopy(t));
/*
		i=ran(t->right->node,0); j=ran(t->right->node,1); 
		t1a=treecompose(t->left->node,NULL,NULL);
		v=treecompose("{",treenum(i),treecopy(t1a));
		v=treecompose("&",v,treecompose("{",t1a,treenum(j)));
*/
		v=treecompose("=",treenum(1),treenum(1));
		treereplace(t,v);
	}
	varl=listmerge(varl,AGcleanassumption(t->left)); 
	varl=listmerge(varl,AGcleanassumption(t->right)); 
	return varl;
}
/*******************************************************************/

/*******************************************************************/
process *prscr(int n)
{
	process *prs1;

	prs1=(process *)malloc(1*sizeof(process)); 
	prs1->n=n;
	prs1->tslist=(tree **)calloc((n+1),sizeof(tree *));
	prs1->tslistpc=(tree **)calloc((n+1),sizeof(tree *));
	return prs1;
}
/*******************************************************************/
