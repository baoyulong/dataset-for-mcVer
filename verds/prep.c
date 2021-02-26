#include "keywords.h"
#include "mod.h"
#include "prep.h"


/*******************************************************************/
extern int opOD;
/*******************************************************************/
char *PREPfile2txt(char *fn)
{
        char *txt;
	FILE *g;

	switch (opOD) {
		case 0: txt=MODapply(fn,NULL); break;
		case 1: txt=file2string(fn); break;
		default: error("Opold");
	}

        stringcomments(txt);
	txt=PREPreplaceENUM(txt);
        txt=PREPreplaceRECORD(txt);
        PREPreplaceASSEQ(txt);

        stringcompress(txt,' ',' ');
        PREPreplaceSTEP(txt);

        stringed(txt,";goto","&"); stringed(txt,":goto",":");

	
 logtmp("PREP",txt); 

/*
 infos(txt);
*/
        return txt;
}
/*******************************************************************/
void PREPreplaceSTEP(char *txt)
{
	char *t;

        for (t=txt;1;) {
                while (*txt&&*txt!='S') *t++=*txt++;
                if (*txt==0) { *t=0; break; }
                if (strncmp(txt,"STEP",4)==0) { 
			if (isalnum(*(txt+4))) { 
				strncpy(t,"s_p=",4); 
				t+=4; txt+=4;
			}
			else { strncpy(t,"s_p",3); t+=3; txt+=4; }
                } else { *t++=*txt++; }
        }
}
/*******************************************************************/
void PREPreplaceASSEQ(char *txt)
{
	int i;
	char *t0a,*t0,*t1,*t2;
	tree *tt;
	char *s0=txt;
	int p;

        while (1) {
		for (i=1;1;txt++) {
                	while (*txt&&*txt!=';'&&*txt!='&') { 
				if (*txt=='=') i=0; txt++; 
			} 
			if (i==0||*txt==0) break;
		}
		if (*txt==0) break;
		t2=txt; i=1;

                for (p=0;*t2&&(*t2!='='||p);t2--) { 
			if (*t2==')') p++; 
			else if (*t2=='(') p--; 
			else if (*t2==',') i=0;
		}

		if (i==1) continue; 
		t0=t2-1; t1=t2+1; 
                while (*t0&&isws(*t0)) t0--; while (*t1&&isws(*t1)) t1++;
		if (*t0!=')') continue;
		t0a=PREPmatch0(t0); t2=PREPmatch1(t1);

		tt=PREPcreatevlist(t0a,t0);
		PREPwritevlist(tt,t0a,t1+1,t2); treefree(tt);
        }
}
/*******************************************************************/
char *PREPmatch0(char *t0)
{
	int i=0;
	for (;1;t0--) {
		if (*t0==')') i++; else if (*t0=='(') i--; if (i==0) return t0;
	}
}
char *PREPmatch1(char *t0)
{
	int i=0;
	for (;1;t0++) {
		if (*t0=='(') i++; else if (*t0==')') i--; if (i==0) return t0;
	}
}
tree *PREPcreatevlist(char *t0,char *t1)
{
	char *t2=t0+1;
	while (1) {
		t0++; while (t0<t1) if (*t0!=',') t0++; else break; *t0=0; 
		if (t0<t1) return treecompose(t2,NULL,PREPcreatevlist(t0,t1));
		return treecompose(t2,NULL,NULL);
	}
}
void PREPwritevlist(tree *tt,char *t0,char *t1,char *t2)
{
	char *t;
	t=tt->node; while (*t) *t0++=*t++; *t0++='=';
	while (*t1!=','&&t1<t2) *t0++=*t1++;
	if (t1==t2) { while (t0<=t2) *t0++=' '; return; }
	*t0++='&'; PREPwritevlist(tt->right,t0,t1+1,t2);
}
/*******************************************************************/


/*******************************************************************/
LOCAL char **cleft;
LOCAL char **cright;
LOCAL int *clen;
LOCAL int cc=0;
/*******************************************************************/
#define isdchar(c)	(isalpha(c)||isachar(c))
/*******************************************************************/
char *PREPreplaceENUM(char *s)
{
	char *t;
	int i,k,x;
	char *s0=s;
	char *s1=s;
	char *s2;
	int ms=16*strlen(s0); /*MAXSTRING 0x4000*/
/*
	repinit(); 
*/
	kwall(); 

    	for (;*s;s++) 
    	if (strncmp(s,"DEFINE",6)==0||strncmp(s,"VAR",3)==0) break;
    	if (strncmp(s,"VAR",3)!=0) {
		s+=6;
    		for (;*s&&strncmp(s,"VAR",3)!=0;) {
			while (*s&&!isdchar(*s)) s++; if (*s==0) break;
			if (k=repgetdef(s,cc)) { s+=k; cc++; } 
			while (*s&&isws(*s)) { s++; }
		}
	}



    	for (s=s0;*s;s++) 
	if (strncmp(s,"VVM",3)==0) { 
		s+=3; 
		while (strncmp(s,"VAR",3)!=0) *s++=' '; break; 
	}


        for (;*s;s++) {
	    	for (;*s;s++) if (strncmp(s,"VAR",3)==0) break;
	    	for (;*s;s++) {
			if (*s=='I') break;
	    		if (strncmp(s,"record",6)==0) { while (*s!='}') s++; }
			if (*s!='{') continue; t=s; s++; i=0;
			while (k=repgetw(s,cc,i)) { s+=k; cc++; i++; }
			sprintf(t,"%i..%i",0,i-1);
			while (*t) t++; *t=' ';
			while (*t!='}') *t++=' '; *t=' ';
            	}
	}

	repdefinternal(); 
	x=0; /*x=reprewriteok(cleft,cright,clen,cc);*/
	if (x==0) s1=(char *)malloc(ms*sizeof(char));;

	stringkstring(s0,cleft,cright,clen,cc,s1); 
	if (x==0) free(s0); 

	cc=0; 
	REPforp(s1);

	if (cc) { 
		s2=(char *)malloc(ms*sizeof(char));;
		stringkstring(s1,cleft,cright,clen,cc,s2); free(s1); s1=s2;
		s2=(char *)malloc(ms*sizeof(char));;
		stringkstring(s1,cleft,cright,clen,cc,s2); free(s1); s1=s2;
	}
	repclose(); return realloc(s1,strlen(s1)+1); 
}
/*******************************************************************/
void repinit()
{
	int i;
	cleft=(char **)malloc(MAXITEM*sizeof(char *));
	cright=(char **)malloc(MAXITEM*sizeof(char *));
	clen=(int *)malloc(MAXITEM*sizeof(int));
	for (i=0;i<MAXITEM;i++) {
		cleft[i]=(char *)malloc(MAXITEML*sizeof(char));
		cright[i]=(char *)malloc(MAXITEML*sizeof(char));
	}
/*
	infos(cleft[1]); infos(cright[1]);
*/
}
void repclose()
{
	int i;
	for (i=0;i<MAXITEM;i++) { free(cleft[i]); free(cright[i]); }
	free(cleft); free(cright); free(clen);
}
/*******************************************************************/
int repgetw(char *s,int cc1,int i)
{
	char *s0=s;
	char *t=cleft[cc1];

	if (*s=='}') return 0;
	clen[cc1]=0;
	while (isws(*s)) { s++; }
	while (*s!=','&&*s!='}'&&!isws(*s)) { *t++=*s++; clen[cc1]++; }
	while (isws(*s)||*s==',') { s++; }
	sprintf(cright[cc1],"%i",i); return s-s0;
}
int repgetdef(char *s,int cc1)
{
	char *s0=s;
	char *t=cleft[cc1];
	int i;
	tree *tr;
	char cr2[MAXITEML];

	clen[cc1]=0;
	while (isdchar(*s)) { *t++=*s++; clen[cc1]++; }
	while (*s!='=') { s++; } 

	s++; t=cright[cc1]; 
	while (*s) { if (*s=='\\') s+=2; if (*s=='\n') break; *t++=*s++; }
	*t=0; 

	strcpy(cr2,cright[cc1]); tr=string2tree(cr2); 
	if ((i=eval(tr,cleft,cright,cc1))!=-1) sprintf(cright[cc1],"%i",i);
	treefree(tr);

	return s-s0;
}
void stringkstring(char *s,char **a,char **b,int *c,int k,char *s1)
{
	int i,j;
	int ok;
	char *bk;
	while (*s) {
	  	while (*s&&isws(*s)) *s1++=*s++;
	  	for (ok=0,i=0;i<k;i++) {
       	    	if (strncmp(s,a[i],c[i])==0) {
			if (isalpha(*s)&&isdchar(*(s+c[i]))) { continue; }
			ok=1;
			bk=b[i];
			j=strlen(bk); /* if (c[i]<j)error("PREPreplaceENUM"); */
			memcpy(s1,&(b[i][0]),j); s1+=j; 
			for (;j<c[i];j++) { *s1++=' '; }
			s+=c[i]; break;
	    	}
		}
	  	if (ok==0) { 
			if (isalpha(*s)) while (isdchar(*s)) *s1++=*s++; else 
			*s1++=*s++; 
	  	}
	}
	*s1=*s;
}
/*******************************************************************/
int reprewriteok(char **a,char **b,int *c,int k)
{
	int i;
/*
	for (i=0;i<k;i++ ) { infos(cleft[i]); infos(cright[i]); infoi(c[i]); }
*/
	return 0;
}
/*******************************************************************/
void repproc(char *s)
{
	char *s0,*t0;

/*
    	for (;*s;s++) if (strncmp(s,"PROC",4)==0) break; 
	if (*s==0) return; s+=4;
*/
    	for (;*s;s++) if (*s=='#') break; if (*s==0) return; s++;
    	for (;*s;s++) if (*s=='#') break; if (*s==0) return; s++;
	while (1) {
		while (*s&&isws(*s)) { s++; } 
/*
    		if (strncmp(s,"FAIRNESS",8)==0) break; 
    		if (strncmp(s,"SPEC",4)==0) break; 
    		if (strncmp(s,"MODULE",6)==0) break; 
*/

		if (*s=='#') break; 

		s0=s;
		while (*s&&(*s!=':')&&(*s!='#')) { s++; }
		if (*s==0||*s=='#') break; 
		t0=s-1; while (t0>s0&&isws(*t0)) t0--;
		if (*t0==']') { s=s0+repgetproc(s0)+1; }

		while (*s&&(*s!=';')&&(*s!='#')) { s++; }
		if (*s==0||*s=='#') break; s++;
	}
}
int repgetproc(char *s)
{
	int i,k=0,n=0;
	char *t;
	char pname[MAXITEML];
	char pn1[MAXITEML];
	int cc1;
	int low,high;

	cc1=cc;
	t=cleft[cc1]; while (*s!=';') { *t++=*s++; n++; } clen[cc1]=n;
	s=cleft[cc1];
	while (isws(*s)) s++;
	while (*s!='['&&!isws(*s)) { pname[k]=*s++; k++; } pname[k]=0;
	while (*s!='[') *s++; s++; while (isws(*s)) s++; low=atoi(s);
	while (*s!='.') *s++; s+=2; while (isws(*s)) s++; high=atoi(s);
	while (*s!=':') *s++; s+=1;
	for (t=cright[cc1],i=low;i<=high;i++) {
		sprintf(pn1,"%s_x_%i:%s;",pname,i,s); 
		strcpy(t,pn1); t+=strlen(pn1);
	}
	*(t-1)=0;
	cc1++;
	for (i=low;i<=high;i++) {
		sprintf(cleft[cc1],"%s[%i]",pname,i); 
		sprintf(cright[cc1],"%s_x_%i",pname,i); 
		clen[cc1]=strlen(cleft[cc1]); cc1++; 
	}
	cc=cc1; return n;
}
/*******************************************************************/
int repdefinternal()
{
	int i;
	char *s0,*s1;
	s1=(char *)malloc(1000*sizeof(char));;
	for (i=1;i<cc;i++) {
		s0=cright[i];
		stringkstring(s0,cleft,cright,clen,i,s1);
		cright[i]=s1; s1=s0;
	}
	free(s1);
}
/*******************************************************************/
void repadddef(char *s)
{
	int k;
	if (*s==0) return;
	if (k=repgetdef(s,cc)) { cc++; } 
}
/*******************************************************************/
int eval(tree *tr,char **ll,char **rr,int cc1)
{
	int x,i,j;

	if (tr==NULL) return -1;
	if (isdigit(tr->node[0])) return atoi(tr->node);
	switch (tr->node[0]) {
		case '+': 
		case '-': 
		case '*': 
		case '/': 
		case '%': 
			if ((i=eval(tr->left,ll,rr,cc1))==-1) return -1; 
			if ((j=eval(tr->right,ll,rr,cc1))==-1) return -1;
			switch (tr->node[0]) {
				case '+': return i+j;
				case '-': return i-j;
				case '*': return i*j;
				case '/': return i/j;
				case '%': return i%j;
			}
	}
	for (x=0;x<cc1;x++) 
	if (strcmp(tr->node,ll[x])==0&&isdigit(*(rr[x]))) return atoi(rr[x]); 
	return -1;
}
/*******************************************************************/


/*******************************************************************/
extern char **cleft;
extern char **cright;
extern int *clen;
extern int cc;
/*******************************************************************/
int REPforp(char *s)
{
	char *s0=s;
	char line[1024];
	char *ll;
	tree *t0;

/*
    	for (;*s;s++) if (strncmp(s,"PROC",4)==0) break; 
	if (*s==0) return; s+=4;
*/
    	for (;*s;s++) if (*s=='#') break; if (*s==0) return s-s0; s++;
    	for (;*s;s++) if (*s=='#') break; if (*s==0) return s-s0; s++;
	while (1) {
		while (*s&&isws(*s)) { s++; } if (*s==0||*s=='#') break;
/*
    		if (strncmp(s,"FAIRNESS",8)==0) break; 
    		if (strncmp(s,"SPEC",4)==0) break; 
    		if (strncmp(s,"MODULE",6)==0) break; 
*/

		ll=line;
		while (*s&&(*s!=';')&&(*s!='#')) { *ll++=*s++; } 
		if (*s==';') *ll++=*s++; *ll=0;

		strcpy(cleft[cc],line); clen[cc]=strlen(line);
		stringcompress(line,' ',' '); t0=string2tree(line);

		if (REPforp1(t0,cright[cc])) cc++;
	}
	return s-s0;
}
int REPforp1(tree *t0,char *pn1)
{
	int i,j,k=0,ffor=0;
	char *s0,*s1;
	char pname[MAXITEML];
	int low,high;
	char *x,*y=pname;
	tree *t1;
	char *s,*t;

	if (t0->node[0]!=':') error("REPforp1");
	s1=t0->left->node;
	while (*s1&&*s1!='[') *s1++; if (*s1==0) return 0; 

	*s1=0; 
	pn1[0]=0;

	s0=t0->left->node; 

	i=ran(s1+1,0),
	j=ran(s1+1,1); 
	if (strncmp(s0,"for",3)==0) {
		s0+=3; x=s0; 
		while (*s0&&(strncmp(s0,"in",2)!=0)) s0++; *s0=0;
		t0=t0->right;
		s0=t0->left->node;
		ffor=1;
	} 

	if (j==0) { j=i; }
        for (;i<=j;i++) {
        	sprintf(y,"%i",i);
                t1=treecopy(t0->right); if (ffor) treereplacestring(t1,x,y);
		k=strlen(pn1); 

		s1=t0->left->node;
		while (*s1&&*s1!='[') *s1++; 
		if (*s1=='[') {
			*s1=0;
			if (strncmp(s1+1,x,strlen(x))!=0) error("REPforp1");
		}
		sprintf(pn1+k,"%s_x_%i:",t0->left->node,i);
		k=strlen(pn1); treewritep(t1,pn1,k,0); treefree(t1);

		cc++;
		sprintf(cleft[cc],"%s[%i]",t0->left->node,i);
		clen[cc]=strlen(cleft[cc]);
		sprintf(cright[cc],"%s_x_%i",t0->left->node,i);
	}

	return *pn1;
}
/*******************************************************************/

/*******************************************************************/
#define isvar(c)	((isdigit(c)||islower(c)||c=='.'||c==']'||c=='['))
#define isws3(c)	(c==' '||c=='	'||c=='\n')
/*******************************************************************/
char *PREPreplaceRECORD(char *ss)
{
	char *s0,*s1,*s2,*s3,*r0;
	tree *t0,*t1;
	char r1[MAXITEML];
	char *r1a=r1;
	int a1=strlen(ss);
	int lc=0;
	char *ss6=ss+a1-6;


     	for (s0=ss;s0<ss6;s0++)  if (strncmp(s0,"record",6)==0) break;

   	if (strncmp(s0,"record",6)!=0) return ss;


        s1=s0; while (*s1!='{') s1++; *s1++=0;
        s2=s1; while (*s2!='}') s2++; *s2++=0;
	while (*s2==';'||isws3(*s2)) s2++;
 
	strcpy(r1,s1);
	t0=string2tree(r1);

        while (*s0!=':') s0--; *s0--=0; 
        while (!(isvar(*s0))) s0--; 
	if (*s0==']') { s0--; while (!(isvar(*s0))) s0--; }
        while ( (isvar(*s0))) s0--; 
        r0=s0; *(s0-1)=0;
        while (*s0&&*s0!='[') s0++; 
	if (*s0) {
		*s0++=0; 
		*r1a++='['; while (*s0) *r1a++=*s0++; *r1a=0;
	} else {
		*s0++=0; 
		*r1a=0;
	}

   	s0=(char *)malloc((a1+2*(strlen(r0)+1)*listlength(t0))); 
	/*multiplied by 2, zwh, 20200325, avoiding some mem-problem "record" */

   	strcpy(s0,ss); s1=s0+strlen(ss);
   	while ((t1=listgetone(t0,lc++))!=NULL) {
		if (t1->node[0]!=':') continue;
              	strcpy(s1,r0); s1+=strlen(r0); *s1++='_';
             	strcpy(s1,t1->left->node); s1+=strlen(t1->left->node); 
             	strcpy(s1,r1); s1+=strlen(r1); 
             	strcpy(s1,":"); s1+=1;
             	strcpy(s1,t1->right->node); s1+=strlen(t1->right->node); 
		*s1++=';';
   	}

	s2=PREPreplaceRECORD1(s2,r0); 
	strcpy(s1,s2); free(ss); ss=s0; treefree(t0);
   	while (1) { s1=PREPreplaceRECORD(ss); if (s1==ss) return ss; ss=s1; }
}
/*******************************************************************/
char *PREPreplaceRECORD1(char *ss,char *r0)
{
	char *s0,*s0a,*s1a,*s2a;
    	char s1[MAXITEML];
     	char s2[MAXITEML];
     	int a1=strlen(ss);
	int a0;
	char *ss6;
	char p;

	while (!isvar(*r0)) r0++; s0=r0; while (isvar(*s0)) s0++; *s0=0; 
	a0=strlen(r0); ss6=ss+a1-a0;

     	for (s0=ss;s0<ss6;s0++)  if (strncmp(s0,r0,a0)==0) {
		p=*(s0-1); 
		if (isdigit(p)||islower(p)) continue;
         	s0+=a0;
		if (*s0=='.') { *s0++='_'; continue; }
         	if (*s0=='[') {
           		s0a=s0;
            		s1a=s1;
            		s2a=s2;
           		while (!(*s0a==']'&&*(s0a+1)=='.')) *s1a++=*s0a++;
           		*s1a++=*s0a++; *s1a=0;

			*s2a++='_'; s0a++;
           		while (isvar(*s0a)) *s2a++=*s0a++; 
			*s2a=0;

           		strncpy(s0,s2,strlen(s2)); 
			strncpy(s0+strlen(s2),s1,strlen(s1));
			s0+=strlen(s2)+strlen(s1);
		}
       }
	return ss;
}
/*******************************************************************/
