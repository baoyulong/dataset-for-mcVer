#include "form.h"
#include "tlf.h"

/*******************************************************************/
#define keepleft(t)   { t0=t->left; treefree(t->right); free(t); return t0; }
#define keepright(t)  { t0=t->right; treefree(t->left); free(t); return t0; }
#define modright(t,a) { t0=t->right; t0->node[0]=a; free(t); return t0; }
/*******************************************************************/
tree *TLFnnfsimpATF(tree *t)
{
        int a,b,c;

        if (t->left==NULL||!isdigit(t->left->node[0])) return t;
        if (t->right==NULL||!isdigit(t->right->node[0])) return t;
        a=atoi(t->left->node); b=atoi(t->right->node);
        switch (t->node[0]) {
          case '-': c=a-b; 
		if (c<0) { /* a=1; while (a<=b) a*=2; c+=a; */ c=0; }
		treereplace(t,treenum(c)); break;
          case '+': treereplace(t,treenum(a+b)); break;
          case '*': treereplace(t,treenum(a*b)); break;
          case '/': treereplace(t,treenum(a/b)); break;
          case '%': treereplace(t,treenum(a%b)); break;
/*zwh
*/
          case '>': treereplace(t,treeconst(a>b)); break;
          case '{': treereplace(t,treeconst(a<=b)); break;
          case '<': treereplace(t,treeconst(a<b)); break;
          case '}': treereplace(t,treeconst(a>=b)); break;
          case '=': treereplace(t,treeconst(a==b)); break;
          case 'B': treereplace(t,treeconst(a!=b)); break;
        }
	return t;
}
/*******************************************************************/
tree *TLFnnfsimp(tree *t)
{
	tree *t0;
	if (t==NULL) return t;

	switch (t->node[0]) {
	  case '&': 
		if (TLFisneg(t->left,t->right)) { 
			treefree(t); return treeconst(0);
		}
		if (TLFissame(t->left,t->right)) { 
			t0=treecopy(t->left); treefree(t); return t0;
		}
	  case DAND:
		if (t->right->node[0]==FALSE) { keepright(t); }
		if (t->right->node[0]==TRUE) { keepleft(t); }
		if (t->left->node[0]==FALSE) { keepleft(t); }
		if (t->left->node[0]==TRUE) { keepright(t); }
		return t;
	  case '|': 
		if (TLFisneg(t->left,t->right)) { 
			treefree(t); return treeconst(1);
		}
		if (TLFissame(t->left,t->right)) { 
			t0=treecopy(t->left); treefree(t); return t0;
		}
		if (t->right->node[0]==FALSE) { keepleft(t); }
		if (t->right->node[0]==TRUE) { keepright(t); }
		if (t->left->node[0]==FALSE) { keepright(t); }
		if (t->left->node[0]==TRUE) { keepleft(t); }
		return t;	/*new20130328*/
	  case BOR:
		if (t->right->node[0]==FALSE) { keepleft(t); }
		if (t->right->node[0]==TRUE) { keepright(t); }
		if (t->left->node[0]==FALSE) { keepright(t); }
		if (t->left->node[0]==TRUE) { keepleft(t); }
		return t;
	  case '!': 
		if (t->right->node[0]==TRUE) { modright(t,FALSE); }
		if (t->right->node[0]==FALSE) { modright(t,TRUE); }
		return t;
	  case ':': 
		t->left=TLFnnfsimp(t->left); t->right=TLFnnfsimp(t->right);
		return t;
	}
	return TLFnnfsimpATF(t);
}
/************************************************treedopost(t,TLFnnfsimp)
tree *TLFnnfsimpall(tree *t)
{
	if (t==NULL) return t;
	t->left=TLFnnfsimpall(t->left); t->right=TLFnnfsimpall(t->right); 
	return TLFnnfsimp(t);
}
/*******************************************************************/

/*******************************************************************/
#define isliteral(node)	      (isdigit(node[0])||node[0]=='!')
/*******************************************************************/
int iscclause(tree *t) 
{
	if (t==NULL) return 0; 
	if (isliteral(t->node)) return 1;
	if (t->node[0]=='&') return iscclause(t->left)&&iscclause(t->right);
	return 0;
}
/*******************************************************************/
tree *lcnfcombine(tree *t1,tree *t2)
{
	tree *s1,*s2;
	if (t1->node[0]=='&') { 
		s1=lcnfcombine(t1->left,t2);
		s2=lcnfcombine(t1->right,t2);
		if (s1->node[0]==FALSE) return s2;
		if (s2->node[0]==FALSE) return s1;
		return treecompose("&",s1,s2);
	} 
	if (t2->node[0]=='&') { 
		s1=lcnfcombine(t1,t2->left);
		s2=lcnfcombine(t1,t2->right);
		if (s1->node[0]==FALSE) return s2;
		if (s2->node[0]==FALSE) return s1;
		return treecompose("&",s1,s2);
	}
	if (t1->node[0]=='!') {
		if (t2->node[0]=='!') {
			if (strcmp(t1->right->node,t2->right->node)==0)
			return treecopy(t1); 
			return treecompose("|",treecopy(t1),treecopy(t2));
		} else 
		if (strcmp(t1->right->node,t2->node)==0) return treeconst(0);
		return treecompose("|",treecopy(t1),treecopy(t2));
	} 
	if (t2->node[0]=='!') {
		if (strcmp(t1->node,t2->right->node)==0) return treeconst(0);
		return treecompose("|",treecopy(t1),treecopy(t2));
	}
	if (strcmp(t1->node,t2->node)==0) return treecopy(t1); 
	return treecompose("|",treecopy(t1),treecopy(t2));
}
/*******************************************************************/
int TLFlcnf(tree *t)
{
	tree *t0;

	if (t==NULL) return 1; 
	if (t->node[0]=='|'&&(iscclause(t->left)&&iscclause(t->right))) {
		t0=lcnfcombine(t->left,t->right);
		treereplace(t,t0); 
		return 1;
	}
	return 0;
}

/*******************************************************************/
int TLFwsCTLext(tree *fml)
{
        if (fml==NULL) return 0;
        switch (fml->node[0]) {
           case 'A': 
           case 'E': 
		if (fml->right==NULL) return 0;
		if (fml->right->node[0]=='F') {
			if (fml->right->right==NULL) return 0;
			if (fml->right->right->node[0]!='G') return 0;
			return 1;
		}
		if (fml->right->node[0]=='G') {
			if (fml->right->right==NULL) return 0;
			if (fml->right->right->node[0]!='F') return 0;
			return 1;
		}
        }
	return TLFwsCTLext(fml->left)||TLFwsCTLext(fml->right);
}
/*******************************************************************/
int TLFwsECTL(tree *fml)
{
        if (fml==NULL) return 0;
        if (fml->node[0]=='E') return 1;
	return TLFwsCTLext(fml->left)||TLFwsCTLext(fml->right);
}
/*******************************************************************/
int TLFwsACTL(tree *fml)
{
        if (fml==NULL) return 0;
        if (fml->node[0]=='A') return 1;
	return TLFwsCTLext(fml->left)||TLFwsCTLext(fml->right);
}
/*******************************************************************/

/*******************************************************************/
#define isliteral(node)	      (isdigit(node[0])||node[0]=='!')
/*******************************************************************/
tree *TLFcnf(tree *t)
{
        tree *s1,*s2;

        if (isliteral(t->node)) return t;
        if (t->node[0]==TRUE||t->node[0]==FALSE) return t;

        s1=TLFcnf(t->left); 
	s2=TLFcnf(t->right); 

        if (t->node[0]=='&') { t->left=s1; t->right=s2; } 
	else if (t->node[0]!='|') { error("TLFcnf"); } 
	else { /* free(t); */ t=TLFcnfdistr(s1,s2); }

	/*t=TLFremovesubs(t); /*not effective*/ 
        t=treedopost(t,TLFnnfsimp); 
	t=TLFcnfsimp(t); /*a problem fixted 20120911*/
	return t;
}
/*******************************************************************/
tree *TLFcnfdistr(tree *s1,tree *s2)
{
        switch (s1->node[0]) {
		case '&': s1->left=TLFcnfdistr(s1->left,treecopy(s2)); 
			  s1->right=TLFcnfdistr(s1->right,s2); break;
		case '^': treefree(s2); break;
		case '_': treefree(s1); s1=s2;
		default : s1=TLFcnfdistr2(s1,s2); break;
	}
	return s1;
}
/*******************************************************************/
tree *TLFcnfdistr2(tree *s1,tree *s2)
{
        switch (s2->node[0]) {
		case '&': s2->left=TLFcnfdistr2(treecopy(s1),s2->left); 
			  s2->right=TLFcnfdistr2(s1,s2->right); return s2;
		case '^': treefree(s1); return s2;
		case '_': treefree(s2); return s1;
		default : return TLFcnfmerge(s1,s2);
	}
}
/*******************************************************************/
tree *TLFcnfmerge(tree *s1,tree *s2)
{
	tree *t1;

        switch (s1->node[0]) {
		case '&': error("TLFmerge");
		case '|': t1=TLFcnfmerge(s1->left,TLFcnfmerge(s1->right,s2));
			 free(s1); return t1;
		case '^': treefree(s2); return s1;
		case '_': treefree(s1); return s2;
		case '!': return TLFcnfinsert(s1,s2,1); 
		default : return TLFcnfinsert(s1,s2,0); 
	}
}
/*******************************************************************/
tree *TLFcnfinsert(tree *s1,tree *s2,int ng)
{
	int a1,a2;
        tree *t1,*t2;

	if (s2->node[0]=='^') { treefree(s1); return s2; }
	if (s2->node[0]=='_') { treefree(s2); return s1; }

	if (ng) a1=atoi(s1->right->node); else a1=atoi(s1->node);
	if (s2->node[0]=='!') {
        	a2=atoi(s2->right->node);
		if (a1==a2) { 
			if (ng==1) { treefree(s2); return s1; }
			treefree(s1); treefree(s2); 
			return treecompose(TRUEs,NULL,NULL);
		}
		if (a1<a2) return treecompose("|",s1,s2);
		return treecompose("|",s2,s1);
	} else if (isdigit(s2->node[0])) {
        	a2=atoi(s2->node);
		if (a1==a2) { 
			if (ng==0) { treefree(s2); return s1; }
			treefree(s1); treefree(s2); 
			return treecompose(TRUEs,NULL,NULL);
		}
		if (a1<a2) return treecompose("|",s1,s2);
		return treecompose("|",s2,s1);
	} else { 
		t1=TLFcnfinsert(s1,s2->left,ng); t2=s2->right; free(s2);
		if (t1->node[0]==TRUE) { treefree(t2); return t1; }
		if (isliteral(t1->node)) return treecompose("|",t1,t2);
		t1->right=TLFcnfinsert(t1->right,t2,t1->right->node[0]=='!'); 
		return t1;
	}
}
/*******************************************************************/
tree *TLFcnfsimp(tree *s1)
{
	tree *t1;
        if (s1->node[0]=='&') {
		s1->left=TLFcnfsimp(s1->left);
		s1->right=TLFcnfsimp(s1->right); 
		t1=TLFcnfsimp2(s1->left,s1->right);
		free(s1); s1=t1; 
	}
	return s1;
}
/*******************************************************************/
tree *TLFcnfsimp2(tree *s1,tree *s2)
{
        tree *t1;
        switch (s1->node[0]) {
                case '&': t1=TLFcnfsimp2(s1->left,TLFcnfsimp2(s1->right,s2));
                          free(s1); return t1;
                case '^': treefree(s1); return s2;
                case '_': treefree(s2); return s1;
                default : return TLFcnfsimpinsert(s1,s2);
        }
}
/*******************************************************************/
tree *TLFcnfsimpinsert(tree *s1,tree *s2)
{
        tree *t1;
        tree *t2;
	int k;

        switch (s1->node[0]) {
                case '^': 
                case '_': 
                case '&': error("TLFcnfsimpinsert");
                default : if (TLFin(s1,s2)) { treefree(s1); return s2; } 
                       	  return treecompose("&",s1,s2);
        }
}
/*******************************************************************/
int TLFin(tree *s1,tree *s2)
{
	int i;
	if (s2->node[0]=='&') return TLFin(s1,s2->left)||TLFin(s1,s2->right);
	return TLFinc(s2,s1);
}
/*******************************************************************/
int TLFinc(tree *s1,tree *s2)
{
	int i;
	if (s1->node[0]=='|') return TLFinc(s1->left,s2)&&TLFinc(s1->right,s2);

	return TLFinc1(s1,s2);
}
/*******************************************************************/
int TLFinc1(tree *s1,tree *s2)
{
	int i;
	if (s2->node[0]=='|')return TLFinc1(s1,s2->left)||TLFinc1(s1,s2->right);

	return TLFissame(s1,s2);
}
/*******************************************************************/
int TLFcnfclausecmp0(tree *s1,tree *s2) 
{ 
 /*the rest is not in use*/ 
	int i;

        if (s1->node[0]=='|'&&s2->node[0]=='|') {
                i=TLFcnfclausecmp1(s1->left,s2->left);
                if (i!=0) return i;
                return TLFcnfclausecmp0(s1->right,s2->right);
        }
        if (s1->node[0]=='|'&&s2->node[0]!='|') {
                i=TLFcnfclausecmp1(s1->left,s2);
                if (i!=0) return i;
                return -1;
        }
        if (s1->node[0]!='|'&&s2->node[0]=='|') {
                i=TLFcnfclausecmp1(s1,s2->left);
                if (i!=0) return i;
                return 1;
        }
        return TLFcnfclausecmp1(s1,s2);
}
/*******************************************************************/
int TLFcnfclausecmp1(tree *s1,tree *s2)
{
	int a1,a2;
	if (s1->node[0]=='!') a1=atoi(s1->right->node); else a1=atoi(s1->node);
	if (s2->node[0]=='!') a2=atoi(s2->right->node); else a2=atoi(s2->node);
	if (a1==a2) return 0; else if (a1<a2) return 1; else return -1;
}
/*******************************************************************/
int TLFcnfclausecmp0s(tree *s1,tree *s2)
{
        if (s1->node[0]=='|'&&s2->node[0]=='|') {
                if (s1->left->node[0]=='!') {
                        if (s2->left->node[0]=='!') return 0; else return 1;
		} else {
                        if (s2->left->node[0]!='!') return 0; else return -1;
		}
	}

	if (s1->node[0]=='!') { if (s2->node[0]=='!') return 0; else return 1; }
        if (s2->node[0]!='!') return 0; else return -1;
}
/*******************************************************************/
tree *TLFremovesubs(tree *s1)
{
	tree *s0;

 /*not effective*/ return s1;
	for (s0=s1;s0->node[0]=='&';s0=s0->right)
		if (TLFsubs(s0->left,s0->right)) 
{
			treereplace(s0->left,treecompose("^",NULL,NULL));
}

	return s1;
}
/*******************************************************************/
int TLFsubs(tree *x0,tree *s1)
{
	if (s1->node[0]=='&') 
	return TLFsubs(x0,s1->left)||TLFsubs(x0,s1->right);

	return TLFsubs1(s1,x0);
}
/*******************************************************************/
int TLFsubs1(tree *x0,tree *s1)
{
	if (x0->node[0]=='|') 
	return TLFsubs1(x0->left,s1)&&TLFsubs1(x0->right,s1);

	return TLFsubs2(x0,s1);
}
/*******************************************************************/
int TLFsubs2(tree *x0,tree *s1)
{
	if (s1->node[0]=='|') 
	return TLFsubs1(x0,s1->left)&&TLFsubs1(x0,s1->right);

	return TLFissame(x0,s1);
}
/*******************************************************************/

/*******************************************************************/
#define isliteral(node)	      (isdigit(node[0])||node[0]=='!')
/*******************************************************************/
tree *TLFdnf(tree *t)
{
	t=TLFnegate(t); t=TLFcnf(t); t=TLFnegate(t);
}
/*******************************************************************/

/*******************************************************************/
extern FILE *vdslog;
/*******************************************************************/
#define isatomic(node)	      		(isdigit(node[0])) 
/*******************************************************************/
LOCAL char DUALOPRS[19]="EAXXFGUR&|>{<}=B^_";
/*
LOCAL char TLFOPRS[9]="EAXXFGUR";
LOCAL char BOFOPRS[6]="&|~I!";
LOCAL char ATFOPRS[9]=">{<}=B^_";
/*******************************************************************/
int TLFdualopr(char a)
{
        char *p=DUALOPRS;
        int i=0; 
        while (*p&&*p!=a) { p++; i++; } if (*p) return i+1; else return 0;
}
/*******************************************************************/
tree *TLFnegate(tree *t)
{
	int i;
        tree *t0;

        if (t==NULL) return NULL;
        if (isatomic(t->node)) return treecompose("!",NULL,t);


	if (i=TLFdualopr(t->node[0])) {
        	i=i+i%2-(i+1)%2; t->node[0]=DUALOPRS[i-1];
		if (i<=10) {
    			t->left=TLFnegate(t->left); 
			t->right=TLFnegate(t->right); 
		}
		return t;		
	}
	switch (t->node[0]) {
	  case '!': t0=t->right; free(t); return t0;
	  case 'I': t->node[0]='&'; t->right=TLFnegate(t->right); return t;
	  case '~': t->right=TLFnegate(t->right); return t;
	}
 	pprts(t->node);
	error("undefined operator 0");
}
/*******************************************************************/
tree *TLFnnf(tree *t)
{
	int i;
        tree *t0,*t1,*t2;

        if (t==NULL) return NULL;
        if (isatomic(t->node)) return t;
	if (t->node[0]=='#'||t->node[0]==';'||t->node[0]==':') {
    		t->left=TLFnnf(t->left); t->right=TLFnnf(t->right); 
		return t;
	}
	if (i=TLFdualopr(t->node[0])) {
        	i=i+i%2-(i+1)%2;
		if (i<=10) {
    			t->left=TLFnnf(t->left); 
			t->right=TLFnnf(t->right); 
		}
		return t;
	}
	switch (t->node[0]) {
	  case '!': 
		if (isatomic(t->right->node)) return t;
		t0=TLFnegate(t->right); free(t); return TLFnnf(t0);
	  case 'I': 
		t->node[0]='|'; 
		t->left=TLFnnf(TLFnegate(t->left)); t->right=TLFnnf(t->right); 
		return t;
	  case '~': 
		t->node[0]='&'; 
		t1=treecompose("|",
			TLFnnf(neg(treecopy(t->left))),
			TLFnnf(treecopy(t->right)));
		t2=treecompose("|",
			TLFnnf(t->left),
			TLFnnf(neg(t->right)));
		t->left=t1; t->right=t2; return t;
	}
	fprintf(vdslog,"undefined operator 1: %s\n",t->node);
/*
 	pprts(t->node);
	error("undefined operator 1");
*/
}
/*******************************************************************/
tree *TLFsimplifyLTL(tree *t)
{
	int i;
        tree *t0;
        tree *t1,*tr;

        if (t==NULL) return NULL;
        if (isatomic(t->node)) return t;

	switch (t->node[0]) {
	  case 'R': 
		    t->left=TLFsimplifyLTL(t->left);
	  	    t->right=TLFsimplifyLTL(t->right);
		    t0=treecompose("|",treecompose("G",NULL,treecopy(t->right)),
			treecompose("U",treecopy(t->right),
				treecompose("&",t->right,t->left)));
		    free(t); return t0;		
	  case 'U': 
	  case '&': 
	  case '|': 
		    t->left=TLFsimplifyLTL(t->left);
	  	    t->right=TLFsimplifyLTL(t->right);
		    return t;		
	  case 'A': error("TLFsimplifyLTL");
	  case 'E': /*for CTL*/
/*
		    if (t->right->node[0]=='R') {
    tr=t->right;
    tr->left=TLFsimplifyLTL(tr->left);
    tr->right=TLFsimplifyLTL(tr->right);
    t0=treecompose("G",NULL,treecopy(tr->right));
    t0=treecompose("E",NULL,t0);
    t1=treecompose("U",treecopy(tr->right),treecompose("&",treecopy(tr->right),treecopy(tr->left)));
    t1=treecompose("E",NULL,t1);
	treefree(t->left); treefree(t->right);
	t->left=t1; t->right=t0; t->node[0]='|'; return t;		
			}
*/
	  case 'X': 
	  case 'G': 
	  case 'F': 
	  case '!': 
	  	    t->right=TLFsimplifyLTL(t->right);
		    return t;		
	  default:
		    fprintf(stdout,"WARNING:	unexpected LTL operator\n");
		    return t;		
	}
}
/*******************************************************************/
tree *TLFsimplifyFALSE(tree *t)
{
        tree *t0,*t1;

        if (t==NULL) return NULL;
	switch (t->node[0]) {
	  case '&': 
			t0=TLFsimplifyFALSE(t->left);
			t1=TLFsimplifyFALSE(t->right);
			if (t0->node[0]=='_') { treefree(t1); return t0; }
			if (t1->node[0]=='_') { treefree(t0); return t1; }
			t->left=t0; t->right=t1; return t;
	  case '|': 
			t0=TLFsimplifyFALSE(t->left);
			t1=TLFsimplifyFALSE(t->right);
			if (t0->node[0]=='_') { treefree(t0); return t1; }
			if (t1->node[0]=='_') { treefree(t1); return t0; }
			t->left=t0; t->right=t1; return t;
	  default: 
			return t;
	}
}
/*******************************************************************/

/*******************************************************************/
#define isplit(t)   (isdigit(t->node[0]))
#define isnlit(t)   (t->node[0]=='!'&&isplit(t->right))
#define islit(t)    (isplit(t)||isnlit(t))
/*******************************************************************/
#define eq0(t,t0)   (isdigit(t->node[0])&&strcmp(t->node,t0->node)==0)
#define eq1(t,t0)   (t->node[0]=='!'&&t0->node[0]=='!'&&eq0(t->right,t0->right))
/*******************************************************************/
#define ng0(t,t0)   (t->node[0]=='!'&&eq0(t->right,t0))
#define ng1(t,t0)   (ng0(t0,t))
/*******************************************************************/
tree *tislit(tree *t,char c)
{
	tree *t0;
	if (islit(t)) return treecopy(t);
	if (t->node[0]!=c) return NULL;
	if ((t0=tislit(t->left,c))!=NULL) return t0;
	if ((t0=tislit(t->right,c))!=NULL) return t0;
	return NULL;
}
/*******************************************************************/
tree *tis2lit(tree *t,char c)
{
	char c0='|';
	tree *t0;

	if (c=='|') c0='&';
	if (t->node[0]==c0) {
		if (islit(t->left)&&islit(t->right)) return treecopy(t);
	}
	if (t->node[0]!=c) return NULL;
	if ((t0=tis2lit(t->left,c))!=NULL) return t0;
	if ((t0=tis2lit(t->right,c))!=NULL) return t0;
	return NULL;
}
/*******************************************************************/
tree *TLFnnfred(tree *t)
{
	char c;
	tree *t0,*t1,*t2;

	if (t==NULL) return t;

	switch (t->node[0]) {
		case ';': 
		case '!': 
			  t->right=TLFnnfred(t->right); return t;
		case '|':
		case '&': c=t->node[0];
			  if ((t1=tislit(t,c))!=NULL) {
				if (c=='|') t1=TLFnegate(t1);
				t=TLFnnfred0(t,t1); t=TLFnnfred(t);
				if (c=='|') t1=TLFnegate(t1);
				return treecompose(t->node,t,t1);
			  }
/*
                          if (c=='&') {
                          	if ((t1=tis2lit(t,'&'))!=NULL) {
                                        t2=TLFnnfred0(treecopy(t),t1->left);
					t2=TLFnnfred(t2);
                                        t=TLFnnfred0(t,t1->right);
					t=TLFnnfred(t);
                                        t2=treecompose("&",t2,t1->left);
                                        t=treecompose("&",t,t1->right);
                                        free(t1); return treecompose("|",t,t2);
				}
			  } 
                          if (c=='|') {
				t=TLFnegate(t);
                          	if ((t1=tis2lit(t,'&'))!=NULL) {
                                        t2=TLFnnfred0(treecopy(t),t1->left);
					t2=TLFnnfred(t2);
                                        t=TLFnnfred0(t,t1->right);
					t=TLFnnfred(t);
                                        t2=treecompose("&",t2,t1->left);
                                        t=treecompose("&",t,t1->right);
                                        free(t1); 
					return TLFnegate(treecompose("|",t,t2));
				}
				t=TLFnegate(t);
			  }
*/
			  t->left=TLFnnfred(t->left); 
			  t->right=TLFnnfred(t->right); return t;
		default: return t;
	}
}
/*******************************************************************/
tree *TLFnnfred0(tree *t,tree *t0)
{
	int p=-1;
	switch (t->node[0]) {
		case ';': 
		case '|': 
		case '&': t->left=TLFnnfred0(t->left,t0);
		case '!': 
			  t->right=TLFnnfred0(t->right,t0); return t;
		default:  if (islit(t)) {
				if (eq0(t,t0)||eq1(t,t0)) p=1;
				else if (ng0(t,t0)||ng1(t,t0)) p=0;
				if (p!=-1) treereplace(t,treeconst(p));
			  }
			  return t;
	}
}
/*******************************************************************/
tree *TLFred(tree *t)
{

	if (t==NULL) return NULL;
	if (TLFisprop(t)) {
		t=TLFnnfred(t); t=treedopost(t,TLFnnfsimp); 
		t=TLFnnfred(t); t=treedopost(t,TLFnnfsimp); return t;
	}
	t->left=TLFred(t->left); t->right=TLFred(t->right); return t;
}
/*******************************************************************/
int TLFisprop(tree *t)
{
	if (t==NULL) return 1;

	switch (t->node[0]) {
		case '!': 
		case '|':
		case '&': return TLFisprop(t->left)&&TLFisprop(t->right);
		default:  return isplit(t);
	}
}
int TLFisneg(tree *s1,tree *s2)
{
        if (isdigit(s1->node[0])&&s2->node[0]=='!') {
                if (isdigit(s2->right->node[0]))
                return strcmp(s1->node,s2->right->node)==0; return 0;
        }
        if (isdigit(s2->node[0])&&s1->node[0]=='!') {
                if (isdigit(s1->right->node[0]))
                return strcmp(s2->node,s1->right->node)==0; return 0;
        }
        return 0;
}
/*******************************************************************/
int TLFissame(tree *s1,tree *s2)
{
        if (isdigit(s1->node[0])&&isdigit(s2->node[0])) {
                return strcmp(s1->node,s2->node)==0;
        }
        if (s1->node[0]=='!'&&s2->node[0]=='!') {
                return TLFissame(s1->right,s2->right);
        }
        return 0;
}
/*******************************************************************/
/*
			  if ((t1=tislit(t->left,c))!=NULL) {
				if (c=='|') t1=TLFnegate(t1);
				t->left=TLFnnfred0(t->left,t1);
				t->right=TLFnnfred0(t->right,t1);
				t=TLFnnfred(t);
				if (c=='|') t1=TLFnegate(t1);
				return treecompose(t->node,t,t1);
			  }
			  if ((t1=tislit(t->right,c))!=NULL) {
				if (c=='|') t1=TLFnegate(t1);
				t->left=TLFnnfred0(t->left,t1);
				t->right=TLFnnfred0(t->right,t1);
				t=TLFnnfred(t);
				if (c=='|') t1=TLFnegate(t1);
				return treecompose(t->node,t,t1);
			  }
*/

/*******************************************************************/
#define tt(t,i)	if (i&t) return (128)|t; else return i|t;
/*******************************************************************/
char TLFnnftype(tree *fml)
{
	int t;
	if (fml==NULL) return 0;
	t=TLFnnftype(fml->left)|TLFnnftype(fml->right); 
	switch (fml->node[0]) {
	  case 'A': tt(t,1);
	  case 'E': tt(t,2);
	  case 'F': tt(t,4);
	  case 'G': tt(t,8);
	  case 'R': tt(t,16);
	  case 'U': tt(t,32);
	  case 'X': tt(t,64);
	}
	return t;
}
/*******************************************************************/
char TLFisCTLx(tree *fml)
{
	int t;
	if (fml==NULL) return 1;
	switch (fml->node[0]) {
	case 'F': case 'G': case 'R': case 'U': case 'X': return 0;
	case 'A': case 'E':
		switch (fml->right->node[0]) {
	  	case 'F': fml=fml->right; 
			  if (fml->right->node[0]=='G') fml=fml->right; 
			  break;
		case 'G': fml=fml->right; 
			  if (fml->right->node[0]=='F') fml=fml->right; 
			  break;
		case 'R': 
		case 'U': 
		case 'X': fml=fml->right; 
			  break;
		default:  return 0;
		}
	}
	return TLFisCTLx(fml->left)&&TLFisCTLx(fml->right);
}
/*******************************************************************/
char TLFisCTL(tree *fml)
{
	int t;
	if (fml==NULL) return 1;
	switch (fml->node[0]) {
	  case 'F': case 'G': case 'R': case 'U': case 'X': return 0;
	  case 'A': case 'E':
		switch (fml->right->node[0]) {
	  		case 'F': case 'G': case 'R': case 'U': case 'X': 
				fml=fml->right; break;
			default: return 0;
		}
	}
	return TLFisCTL(fml->left)&&TLFisCTL(fml->right);
}
/*******************************************************************/
tree *TLFgetAGp(tree *fml)
{
	char a=TLFnnftype(fml);

	if (a&0xf6) return NULL;
	if (fml->node[0]=='A') fml=fml->right;
	if (fml->node[0]=='G') return treecopy(fml->right); else return NULL;
}
/*******************************************************************/
tree *TLFgetAFp(tree *fml)
{
	char a=TLFnnftype(fml);

	if (a&0xfa) return NULL;
	if (fml->node[0]=='A') fml=fml->right;
	if (fml->node[0]=='F') return treecopy(fml->right); else return NULL;
}

/*******************************************************************/
tree *TLFgetAG(tree *fml)
{
	char a=TLFnnftype(fml);
	if ((a&0x09)==0) return NULL;
	if (fml->node[0]=='A') fml=fml->right;
	if (fml->node[0]=='G') return treecopy(fml->right); else return NULL;
}
/*******************************************************************/
int TLFisAEorEe(tree *fml)
{
	char a=TLFnnftype(fml);
	if ((a&0x3)==0x3||(a&0x2)==0x2) return 1;
	if (TLFisCTL(fml)) return 0;
	return 1;
}
/*******************************************************************/
