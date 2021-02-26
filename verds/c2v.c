#include "c2v.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*********************************************************************/
#define VERDSASSERTION  "verds_assertion(%s%s%s);\n"
#define VERDSASSERTIONa "_verds_value_equ(%s%s%s);\n"
/*********************************************************************/
int spget(char *fn,int *spline,char *spcond[]);
int spput(char *fn,char *fn2,int *spline,char *spcond[],int spcnt,int usea);
/*********************************************************************/
int addspmain(int argc,char *fn1,char *fn2,char *fn3)
{
	FILE *f1=fopen(fn1,"r");
	char line[4096];
	int spline[1024];
	char *spcond[1024];
	int spcnt;
	int i;

	if (argc<3) { fprintf(stdout,"usage: f1 f2\n"); return; }

	for (i=0;i<1024;i++) spcond[i]=(char *)calloc(4096,sizeof(char));
	spcnt=spget(fn2,spline,spcond);
/*
	for (i=0;i<spcnt;i++) fprintf(stdout,"%i:%s\n",spline[i],spcond[i]);
*/
	spput(fn1,fn3,spline,spcond,spcnt,argc==4);
}
/*********************************************************************/
int spget(char *fn,int *spline,char *spcond[])
{
	FILE *f2=fopen(fn,"r");
	char line[4096];
	char *ll,*l2;
	int spcnt=0;
	int i;

	while (1) {
		fgets(line,4096,f2); if (feof(f2)) break;
		ll=line;
		while (*ll&&!isdigit(*ll)) ll++; spline[spcnt]=atoi(ll);
		while (*ll&&!(*ll==':')) ll++; l2=spcond[spcnt];
		if (*ll!=':') continue; ll++;
		for (;*ll;ll++) {
			if (*ll==' '||*ll=='	'||*ll=='\n') continue;
			if (*ll==';') continue;
			if (*ll=='&'&&*(ll+1)=='&') continue;
			if (*ll=='|'&&*(ll+1)=='|') continue; 
			if (*ll=='='&&*(ll+1)=='=') continue; 
			*l2=*ll; l2++;
		}
		*l2=0; spcnt++;
	}
	fclose(f2); return spcnt;
}
/*********************************************************************/
int spput(char *fn,char *fn2,int *spline,char *spcond[],int spcnt,int usea)
{
	FILE *f1=fopen(fn,"r");
	FILE *f3=fopen(fn2,"w");
	char line[4096];
	int lcnt=0;
	int i;

	while (1) {
		fgets(line,4096,f1); if (feof(f1)) break; lcnt++;
		for (i=0;i<spcnt;i++) 
		if (spline[i]==lcnt) 
		if (usea) fprintf(f3,VERDSASSERTIONa,"\"",spcond[i],"\"");
		else fprintf(f3,VERDSASSERTION,"\"",spcond[i],"\"");
		fprintf(f3,"%s",line);
	}
	fclose(f1); fclose(f3); return spcnt;
}
/*********************************************************************/

/*******************************************************************/
FILE *LOGfile;
FILE *VVM;
FILE *VVC;
/*******************************************************************/
#define LOG(s)	fprintf(LOGfile,"IGNORED: %s\n",s)
/*******************************************************************/
void c2v(char *fn1,char *fn2,char *fn3)
{
	char fn[256];
	fun *ff;
	fun *fsp=NULL;

 	LOGfile=fopen("c2v.log","w");

	if (fn2==NULL) strcpy(fn,fn1);
	else { gfilename(fn1,fn); strcat(fn,".vvc"); addspmain(3,fn1,fn2,fn); }

	if (fn3!=NULL) { fsp=file2fsp(fn3); }
	ff=file2fun(fn);

 	gfilename(fn1,fn); strcat(fn,".vvm"); VVM=fopen(fn,"w");
 	gfilename(fn1,fn); strcat(fn,".vvc"); VVC=fopen(fn,"w");
	c2fun(ff,fsp);

	fclose(LOGfile); fclose(VVM); fclose(VVC); 
}
/*******************************************************************/
void c2stm(stm *s0,int n,int c,int b,int j,int m)
{
	int a,a1;
	int i=0;
	char vv[MAXSL];

	if (s0==NULL) return;
	strcpy(vv,s0->str); cleanline(s0->str);
	if (strcmp(s0->key,"while")==0) { 
		fprintf(VVC,"pc=%i:\t",n);
		fprintf(VVC,"%s (%s)\n",s0->key,s0->str);
		fprintf(VVM,"\tpc=%i&(%s):	(pc):=(%i);\n",n,s0->str,n+1);
		c2stm(s0->left,n+1,n,j,n,m); 
		fprintf(VVM,"\tpc=%i&!(%s):	(pc):=(%i);\n",n,s0->str,j);
		return;
	}
	if (strcmp(s0->key,"for")==0) { 
/*
		fprintf(VVC,"pc=%i:\t",n);
		fprintf(VVC,"%s (%s)\n",s0->key,s0->str);
*/
		LOG(s0->key); 
		return;
	}
	if (strcmp(s0->key,"switch")==0) { 
/*
		fprintf(VVC,"pc=%i:\t",n);
		fprintf(VVC,"%s (%s)\n",s0->key,s0->str);
*/
		LOG(s0->key); 
		return;
	}
	if (strcmp(s0->key,"case")==0) { 
/*
		fprintf(VVC,"pc=%i:\t",n);
                fprintf(VVC,"%s %s:\n",s0->key,s0->str);
*/
		LOG(s0->key); 
		return;
	}
	if (strcmp(s0->key,"default")==0) { 
/*
		fprintf(VVC,"pc=%i:\t",n);
                fprintf(VVC,"%s %s:\n",s0->key,s0->str);
*/
		LOG(s0->key); 
		return;
	}
	if (strcmp(s0->key,"break")==0) {
		fprintf(VVC,"pc=%i:\t",n);
                fprintf(VVC,"%s;\n",s0->key);
		fprintf(VVM,"\tpc=%i:	(pc):=(%i);\n",n,b); 
		return;
	}
	if (strcmp(s0->key,"continue")==0) {
		fprintf(VVC,"pc=%i:\t",n);
                fprintf(VVC,"%s;\n",s0->key);
		fprintf(VVM,"\tpc=%i:	(pc):=(%i);\n",n,c); 
		return;
	}
	if (strcmp(s0->key,"return")==0) {
		fprintf(VVC,"pc=%i:\t",n);
		fprintf(VVC,"%s %s;\n",s0->key,s0->str);
		fprintf(VVM,"\tpc=%i:\t(pc,return_val):=(%i,%s)&RETURN;\n",
			n,m,s0->str);
		return;
	}
	if (strcmp(s0->key,"if")==0) { 
		fprintf(VVM,"\tpc=%i&(%s):	(pc):=(%i);\n",n,s0->str,n+1);
		if (s0->next==NULL) { 
		   fprintf(VVC,"pc=%i:\t",n);
                   fprintf(VVC,"%s (%s)\n",s0->key,s0->str);
		   c2stm(s0->left,n+1,c,b,j,m); 
		   fprintf(VVM,"\tpc=%i&!(%s):\t(pc):=(%i);\n",n,s0->str,j);
		} else {
		   fprintf(VVC,"pc=%i:\t",n);
                   fprintf(VVC,"%s (%s)\n",s0->key,s0->str);
		   c2stm(s0->left,n+1,c,b,j,m); 
		   i=kk(s0->left,0)+1; if (i==1) i=kk(s0->left,n)+1;
		   fprintf(VVM,"\tpc=%i&!(%s):\t(pc):=(%i);\n",n,s0->str,n+i);
		   fprintf(VVC,"pc=%i:\t",n);
                   fprintf(VVC,"else \t\t\t/* !(%s) */\n",vv);
		   c2stm(s0->next,n+i,c,b,j,m); 
		}
		return;
	}
        if (strcmp(s0->key,";")==0) {
/*
                fprintf(VVC,"{\n");
		i=kk(s0->left,0); if (i==0) i=kk(s0->left,n);
		a=kk(s0->next,0); if (a==0) a=kk(s0->next,n);
		if (i==0) { c2stm(s0->next,n,c,b,j,m); }
		else if (a==0) { c2stm(s0->left,n,c,b,j,m); }
                else {
			c2stm(s0->left,n,c,b,n+i,m); 
			c2stm(s0->next,n+i,c,b,j,m); 
		}
                fprintf(VVC,"}\n");
		return;
*/
		a=kk(s0->left,0); if (a==0) i=kk(s0->left,n);
		if (a==0) { c2stm(s0->next,n,c,b,j,m); }
		else {
                	fprintf(VVC,"\t{\n");
                	for (;s0!=NULL;s0=s0->next,n+=i) { 
				i=kk(s0->left,0); 
				if (i==0) { i=kk(s0->left,n); continue; }
				a=kk(s0->next,0); if (a==0) a=kk(s0->next,n);
				if (a!=0) c2stm(s0->left,n,c,b,n+i,m);
				else { c2stm(s0->left,n,c,b,j,m); break; }
			}
               		fprintf(VVC,"\t}\n");
		}
                return;

	}
	if (strcmp(s0->key,"=")==0) {
		fprintf(VVC,"pc=%i:\t",n);
                fprintf(VVC,"%s;\n",s0->str);
		if (!iseq(s0->str)) {
			LOG(s0->str);
			fprintf(VVM,"\tpc=%i:\t(pc):=(%i);\n",n,j); return;
		}
		strcpy(vv,s0->str);
		a=0; while (vv[a]&&vv[a]!='=') a++;
		if (vv[a]!='=') { 
			fprintf(VVM,"\tpc=%i:\t%s&(pc):=(%i);\n",n,vv,j);
			return;
		} 
		vv[a]=0;
		if (isfun(vv+a+1)) { 
			if (iskey(vv+a+1,"getc")) {
				fprintf(VVM,"\tpc=%i:\t(pc,%s):=(%i,*);\n",
					n,vv,j);
				return;
			}
			a1=a+1; while (vv[a1]&&vv[a1]!=')') a1++; 
			if (isfun(vv+a+1)>1) vv[a1]=','; else vv[a1]=0;
			fprintf(VVM,"\tpc=%i:\t%s%s)&(pc):=(%i);\n",
				n,vv+a+1,vv,j);
			return;
		} 
		fprintf(VVM,"\tpc=%i:\t(pc,%s):=(%i,%s);\n",n,vv,j,vv+a+1);
		return;
	}
}
/*******************************************************************/
void c2par(par *p0)
{
	if (p0==NULL) fprintf(VVM,"%s","return_val");
	else {
		strcpy(p0->init,"par_"); strcat(p0->init,p0->name);
		fprintf(VVM,"par_%s",p0->name); fprintf(VVM,","); 
		c2par(p0->next);
	}
}
/*******************************************************************/
void c2wspec(par *p0)
{
	if (p0==NULL) return;
	fprintf(VVM,"\tAG(!(pc=%s)|(%s));\n",p0->type,p0->init);
	c2wspec(p0->next);
}
/*******************************************************************/
void c2var(par *p0)
{
	int i,j;
	char name[MAXNL];

	if (p0==NULL) return;
	if (*p0->init==0) strcpy(p0->init,"0");
	if ((i=isarray(p0->name))==-1) {
		fprintf(VVM,"\t%s:	%s;\n",p0->name,p0->type);
	} else {
		strcpy(name,p0->name);
		for (j=0;name[j]!='[';j++); name[j]=0;
		fprintf(VVM,"\t%s[0..%i]:	%s;\n",name,i-1,p0->type);
	}
	c2var(p0->next);
}
/*******************************************************************/
void c2init(par *p0)
{
	int i,j;
	char name[MAXNL];
	char ii[MAXCL];

	if (p0==NULL) return;
	if (*p0->init==0) strcpy(p0->init,"0");
	if ((i=isarray(p0->name))==-1) {
		fprintf(VVM,"\t%s=%s;\n",p0->name,p0->init);
	} else {
		strcpy(name,p0->name);
		for (j=0;name[j]!='[';j++); name[j]=0;
		for (j=0;j<i;j++) {
	         	fprintf(VVM,"\t%s[%i]=%s;\n",name,j,
				ktharrayvalue(p0->init,p0->type,j,ii));
		}
	}
	c2init(p0->next);
}
/*******************************************************************/
void c2fun(fun *ff,fun *fsp)
{
	int m;
	if (ff==NULL) return;
	if (ff->name[0]==0) return;
	if (strcmp(ff->name,"main")==0) {
		fprintf(VVM,"VVM\t%s\n",ff->name); 
	} else {
		fprintf(VVM,"\nPROCEDURE %s(",ff->name); c2par(ff->p0);
		fprintf(VVM,")\n"); 
	}
	fprintf(VVC,"/**********************************************/\n");
	fprintf(VVC,"\t%s %s()\n",ff->type,ff->name);
	m=kk(ff->s1,0); 
	fprintf(VVM,"VAR\n"); 
	if (strcmp(ff->name,"main")!=0) c2var(ff->p0); 
	c2var(ff->v0); fprintf(VVM,"\t%s:	%i..%i;\n","pc",0,m);
	fprintf(VVM,"INIT\n"); 
	if (strcmp(ff->name,"main")!=0) c2init(ff->p0); 
	c2init(ff->v0); fprintf(VVM,"\t%s=%i;\n","pc",0);
	fprintf(VVM,"TRANS\n"); 
	c2stm(ff->s1,0,0,0,m,m);
	if (strcmp(ff->name,"main")==0) {
		fprintf(VVM,"\tpc=%i:	(pc):=(%i);\n",m,m);
		fprintf(VVM,"SPEC\n"); c2wspec(ff->sp); 
	} else {
		fprintf(VVM,"\tpc=%i:	(pc):=(%i)&RETURN;\n",m,m);
		checkfsp(ff,fsp);
	}
	c2fun(ff->next,fsp);
}
/*******************************************************************/
int isarray(char *vv)
{
	char *v1=vv;
	int i;

	if (*vv=='[') return -1;
	while (*v1&&*v1!='['&&isalnum(*v1)) v1++; if (*v1!='[') return -1;
	v1=skipspace(v1+1); 
	i=atoi(v1); return i;
}
/*******************************************************************/
int isfun(char *vv)
{
	char *v1=vv;

	if (*vv=='(') return 0;
	while (*v1&&*v1!='('&&isalnum(*v1)) v1++; if (*v1!='(') return 0;
	v1=skipspace(v1+1); 
	if (*v1==')') return 1; 
	return 2;
}
/*******************************************************************/
int iseq(char *vv)
{
	char *v1=vv;
	while (*v1&&*v1!='=') v1++; return (*v1=='=');
}
/*******************************************************************/
void addspec(fun *ff,int n,char *sp)
{
	par *v1;
	par *vlist;

	v1=(par *)calloc(1,sizeof(par));
	sp=skipspace(sp);
	strcpy(v1->init,sp+2); v1->init[strlen(v1->init)-2]=0;
	sprintf(v1->type,"%i",n);

        if (ff->sp==NULL) ff->sp=v1;
        else {
                vlist=ff->sp;
                while (vlist->next!=NULL) vlist=vlist->next;
                vlist->next=v1;
        }
}
/*******************************************************************/
void checkfsp(fun *ff,fun *fsp)
{
        stm *s0;
	par *p0,*p1;
	if (fsp==NULL) return;
	if (strcmp(ff->name,fsp->name)!=0) { checkfsp(ff,fsp->next); return; }

	p0=ff->p0;
	p1=fsp->p0;
	while (p0!=NULL) { 
		replacefsp(fsp,p1->name,p0->init); 
		p0=p0->next; p1=p1->next; 
	}
	replacefsp(fsp,fsp->type,"return_val"); 

        s0=fsp->s1;
        while (s0!=NULL) { 
		if (s0->left!=NULL) {
			if (strcmp(s0->left->key,"ASSUMPTION")==0) {
			   fprintf(VVM,"%s\n",s0->left->key);
	p0=ff->p0;
	while (p0!=NULL) { 
		fprintf(VVM,"\t%s:%s;\n",p0->init,p0->type);
		p0=p0->next;
	}
	fprintf(VVM,"\t%s:%s;\n","return_val",ff->type);

			} else if (strcmp(s0->left->key,"GUARANTEE")==0) {
			   fprintf(VVM,"%s\n",s0->left->key);
	p0=ff->p0;
	while (p0!=NULL) { 
		fprintf(VVM,"\t%s=P(%s);\n",p0->init,p0->init);
		p0=p0->next;
	}
			}
			else fprintf(VVM,"\t%s;\n",cleanline(s0->left->str));
		}
		s0=s0->next; 
	}

        writefsp(ff->next);
}
/*******************************************************************/
char *ktharrayvalue(char *s0,char *t0,int k,char *ii)
{
	int i=0;

	strcpy(ii,"0"); 
	if (strcmp(t0,"char")==0) {
		while (*s0&&*s0!='"') s0++; s0++;
		for (i=0;i<=k;i++) if (s0[i]=='"') return ii;
		sprintf(ii,"'%c'",s0[k]); return ii; 
	} else {
		while (*s0&&*s0!='{') s0++; s0++;
		while (1) {
			if (i==k) { sprintf(ii,"%i",atoi(s0)); return ii; }
			while (*s0&&*s0!=',') s0++; 
			if (*s0==0) return ii; 
			s0++; i=i+1;
		}
	} 
}
/*******************************************************************/

/*******************************************************************/
#define isatom(a)		(isalnum(a)||a=='_')
/*******************************************************************/
fun *file2fsp(char *fn)
{
	char *ll; 
	fun *p2;
	fun *p1=NULL;
	fun *plist=NULL;

	ll=file2string(fn); ll=skipspace(ll);
	while (iskey(ll,"FUNCTION")) {
		ll=skipstring(ll,"FUNCTION");
        	plist=(fun *)calloc(1,sizeof(fun)); 
		ll=fspfun(ll,plist);
		if (p1==NULL) { p1=plist; p2=p1; }
                else { p2->next=plist; p2=p2->next; }
	}
/*
	writefsp(p1);
*/
	return p1;
}
/*******************************************************************/
char *fspfun(char *ll,fun *ff) 
{
        char c1[MAXCL];

	ll=gword(ll,ff->type); 
	if (*ll!='=') error(ll,7); ll=skipstring(ll,"=");

	ll=gword(ll,ff->name); ll=gparlist(ll,ff);

	ll=gcase(ll,"FUNCTION",c1);
	ff->s1=(stm *)calloc(1,sizeof(stm));
	(void)getfunstm(c1,ff->s1); return ll;
}
/*******************************************************************/
void writefsp(fun *ff)
{
	stm *s0;

 return;

        if (ff==NULL) return;
        if (ff->name[0]==0) return;
        fprintf(stdout,"%s=%s(",ff->type,ff->name);
        writepar(ff->p0);
        fprintf(stdout,")\n");

	s0=ff->s1;
        while (s0!=NULL) { writestm(s0->left); s0=s0->next; }

        writefsp(ff->next);
}
/*******************************************************************/
void replacefsp(fun *ff,char *s1,char *s2)
{
	stm *s0=ff->s1;
        while (s0!=NULL) { 
		if (s0->left!=NULL) replacestring(s0->left->str,s1,s2); 
		s0=s0->next; 
	}
}
/*******************************************************************/
void replacestring(char *s0,char *s1,char *s2)
{
	char ss[MAXCL];
	int len1=strlen(s1);
	int len2=strlen(s2);
	char *ss0;
	char *s01=s0;

	if (len1==0) return; strcpy(ss,s0); ss0=ss;

	while (*ss0) {
		while (*ss0&&!isatom(*ss0)) *s0++=*ss0++;
		if (strlen(ss0)<len1) break;
		if (strncmp(ss0,s1,len1)==0&&!isatom(*(ss0+len1))) {
			strcpy(s0,s2); s0+=len2; ss0+=len1;
		}
		while (*ss0&&isatom(*ss0)) *s0++=*ss0++;
	}
	*s0=0;
}
/*******************************************************************/

/*******************************************************************/
/*******************************************************************/
int iskey(char *s,char *k) 
{
	int len=strlen(k);
	if ((strncmp(s,k,len)==0)&&isdelsp(s[len])) return 1; return 0;
}
/*******************************************************************/
int isvar(char *in) 
{
	while (*in&&(*in!=';')) { if (*in=='(') return 0; else in++; } 
	return 1;
}
/*******************************************************************/
int isfundef(char *in) 
{
	while (*in&&(*in!=';')) { if (*in=='{') return 0; else in++; } 
	return 1;
}
/*******************************************************************/
char *skipspace(char *in) 
{
	while (*in) { 
		if (isspace(*in)) { in++; continue; }
		if (strncmp(in,"//",2)==0) { in=skip2c(in,'\n'); continue; }
                if (strncmp(in,"/*",2)==0) { in=skip2s(in,"*/"); continue; }
                if (*in=='#') { in=skip2c(in,'\n'); continue; }
		break;
	} 
	return in;
}
/*******************************************************************/
char *skipstring(char *in,char *str) 
{
	int len=strlen(str);
	return skipspace(in+len);
}
/*******************************************************************/
char *skip2c(char *in,char c) 
{
	while (*in) { if (*in!=c) in++; else break; } 
	return skipspace(in+1);
}
/*******************************************************************/
char *skip2s(char *in,char *str) 
{
	int len=strlen(str);
	while (*in) { if (strncmp(in,str,len)!=0) in++; else break; } 
	return skipspace(in+len);
}
/*******************************************************************/
/*******************************************************************/
char *cleanline(char *line)
{
        char *ll;

        for (ll=line;*ll;ll++) {
                if (*ll=='&'&&*(ll+1)=='&') { *ll=' '; continue; }
                if (*ll=='|'&&*(ll+1)=='|') { *ll=' '; continue; }
                if (*ll=='='&&*(ll+1)=='=') { *ll=' '; continue; }
        }
        return line;
}
/*********************************************************************/

/*******************************************************************/
int kk(stm *s0,int n)
{
	int i=0,j=0;
	if (s0==NULL) return 0;
	if (strcmp(s0->key,"while")==0) { 
		return kk(s0->left,n)+1; 
	}
	if (strcmp(s0->key,"for")==0) { 
		if (n==-1) fprintf(stdout,"%s (%s)",s0->key,s0->str);
		return kk(s0->left,n)+3; 
	}
	if (strcmp(s0->key,"switch")==0) { 
		if (n==-1) fprintf(stdout,"%s (%s)",s0->key,s0->str);
		return kk(s0->left,n)+1; 
	}
	if (strcmp(s0->key,"case")==0) { 
		if (n==-1) fprintf(stdout,"%s %s:",s0->key,s0->str);
		return kk(s0->left,n); 
	}
	if (strcmp(s0->key,"default")==0) { 
		if (n==-1) fprintf(stdout,"%s %s:",s0->key,s0->str);
		return kk(s0->left,n); 
	}
	if (strcmp(s0->key,"break")==0) {
		if (n==-1) fprintf(stdout,"%s;\n",s0->key);
		return 1;
	}
	if (strcmp(s0->key,"continue")==0) {
		if (n==-1) fprintf(stdout,"%s;\n",s0->key);
		return 1;
	}
	if (strcmp(s0->key,"return")==0) {
		if (n==-1) fprintf(stdout,"%s %s;\n",s0->key,s0->str);
		return 1;
	}
	if (strcmp(s0->key,"if")==0) { 
                if (n==-1) fprintf(stdout,"%s (%s) ",s0->key,s0->str);
		i=kk(s0->left,n)+1; 
		if (s0->next!=NULL) { 
                        if (n==-1) fprintf(stdout,"else ");
			j=kk(s0->next,n)+1; 
		}
		return i+j;
	}
        if (strcmp(s0->key,";")==0) {
                if (n==-1) fprintf(stdout,"{\n");
                i+=kk(s0->left,n);
                while (s0->next!=NULL) { i+=kk(s0->next->left,n); s0=s0->next;}
                if (n==-1) fprintf(stdout,"}\n");
                return i;
	}
	if (strcmp(s0->key,"verds_assertion")==0&&n) { 
                /* if (n==-1) fprintf(stdout,"\t%s%s;\n",s0->key,s0->str); */
		addspec(gff,n,s0->str); 
		s0->key[0]=0;
		return 0;
	}
	if (strcmp(s0->key,"=")==0) {
                if (n==-1) fprintf(stdout,"%s;\n",s0->str);
		return 1;
	}
	return 0;
}
/*******************************************************************/

/*******************************************************************/
fun *gff;
/*******************************************************************/
fun *file2fun(char *fn)
{
	char *ll; 

        gff=(fun *)calloc(1,sizeof(fun)); 

	ll=file2string(fn); 
	ll=skipspace(ll);
	gfunlist(ll,gff);
/*
        writefun(ff);
*/
	return gff;
}
/*******************************************************************/
void gfunlist(char *ll,fun *ff)
{
	char key[MAXKL];

	while (*ll) {
		if (iskey(ll,"int")) { strcpy(key,"int"); }
		else if (iskey(ll,"char")) { strcpy(key,"char"); } else *key=0;
		ll=skipstring(ll,key); 
		if (isvar(ll)) { ll=gvarlist(ll,key,gff,1); continue; } 
		if (isfundef(ll)) { ll=skip2c(ll,';'); continue; }
		if (1) /*isfun(ll)*/ {
			strcpy(ff->type,key);
			ll=gfun(ll,ff); 
			ff->next=(fun *)calloc(1,sizeof(fun)); 
			ff=ff->next; continue;
		} 
		error(ll,1);
	}
}
/************************************************************************/
char *gvarlist(char *ll,char *key,fun *ff,int global) 
{
	char name[MAXNL];
	char init[MAXNL];
	par *v1=NULL;
	par *v2=NULL;
	par *vlist=NULL; 

	while (*ll!=';') { 
		init[0]=0;
		ll=gword(ll,name); 
		if (*ll=='=') ll=gword(skipspace(ll+1),init); 
		if (*ll!=';'&&*ll!=',') error(ll,2); 
		if (*ll!=';') ll=skipstring(ll,",");
		vlist=(par *)calloc(1,sizeof(par)); 
		if (v1==NULL) { v1=vlist; v2=v1; }
		else { v2->next=vlist; v2=v2->next; }
		strcpy(vlist->type,key);
		strcpy(vlist->name,name);
		strcpy(vlist->init,init);
		if (global) vlist->type[15]='1'; else vlist->type[15]='0';
	}

	if (ff->v0==NULL) ff->v0=v1; 
	else {
		vlist=ff->v0;
		while (vlist->next!=NULL) vlist=vlist->next;
		vlist->next=v1;
	}
	return skipstring(ll,";");
}
/************************************************************************/
char *gfun(char *ll,fun *ff) 
{
	char key[MAXKL];
	char name[MAXNL];
	par *plist=ff->p0;

	ll=gword(ll,name); strcpy(ff->name,name); 
	ll=gparlist(ll,ff); 
	
	ff->s1=(stm *)calloc(1,sizeof(stm));
	if (*ll!='{') { ll=getfunstm(ll,ff->s1); return ll; }

	ll=skipstring(ll,"{");
	while (1) {
		if (iskey(ll,"int")) { strcpy(key,"int"); }
		else if (iskey(ll,"char")) { strcpy(key,"char"); } else break; 
		ll=skipstring(ll,key); ll=gvarlist(ll,key,ff,0); 
	}
	ll=getfunstmlist(ll,ff->s1); ll=skipstring(ll,"}"); return ll;
}
/************************************************************************/
char *gword(char *ll,char *name) 
{
	int p=0,a=0;

	if (*ll=='{') {
		p=1;
		*name++=*ll++;
		while (*ll&&p) { 
			if (*ll=='{') p=p+1;
			else if (*ll=='}') p=p-1;
			*name++=*ll++; 
		}
	} else if (*ll=='"') {
		a=1;
		*name++=*ll++;
		while (*ll&&a) { if (*ll=='"') a=1-a; *name++=*ll++; }
	} else {
		while (*ll) { if (isdel(*ll)) break; *name++=*ll++; }
	}

	*name=0;
	return skipspace(ll);
}
/************************************************************************/
char *gcond(char *ll,char *name) 
{
	int i=1;
	int p=0;

	if (*ll!='(') return ll; 
	for (*name++=*ll++;*ll&&i;*name++=*ll++) {
		if (*ll=='"') p=1-p;
		if (p==0) { if (*ll=='(') i++; else if (*ll==')') i--; }
	}
	*name=0;
	return skipspace(ll);
}
/************************************************************************/
char *g2stm(char *ll,char *name,char a,char b) 
{
	int p=0;
	while (*ll&&(p==1||(*ll!=a&&*ll!=b))) { 
		if (*ll=='"') p=1-p;
		*name++=*ll++; 
	} 
	*name=0; 
	if (*ll==a) ll=skipspace(ll+1);
	return ll;
}
/************************************************************************/
char *gparlist(char *ll,fun *ff) 
{
	char key[MAXKL];
	char name[MAXNL];
	par *p1=NULL;
	par *p2=NULL;
	par *plist=NULL; 

	if (*ll!='(') error(ll,3); ll=skipstring(ll,"(");
	while (*ll!=')') {
		if (iskey(ll,"int")) { strcpy(key,"int"); }
		else if (iskey(ll,"char")) { strcpy(key,"char"); } else *key=0;
		ll=skipstring(ll,key); 
		ll=gword(ll,name); 
		if (*ll!=')'&&*ll!=',') error(ll,4);
		if (*ll!=')') ll=skipstring(ll,",");
		plist=(par *)calloc(1,sizeof(par)); 
		if (p1==NULL) { p1=plist; p2=p1; }
		else { p2->next=plist; p2=p2->next; }
		strcpy(plist->type,key);
		strcpy(plist->name,name);
	}

	if (ff->p0==NULL) ff->p0=p1; 
	else {
		plist=ff->p0;
		while (plist->next!=NULL) plist=plist->next;
		plist->next=p1;
	}
	return skipstring(ll,")");
}
/************************************************************************/
char *gcase(char *ll,char *def,char *c1) 
{
	int p=0;
	int a=strlen(def);

	*c1++='{';
	while (*ll&&
	      (p==1||
	      (strncmp(ll,"case",4)!=0&&strncmp(ll,def,a)!=0&&*ll!='}')))
	{ 
	
		if (*ll=='"') p=1-p;
		*c1++=*ll++; 
	} 
	*c1++='}'; *c1=0; 
	return skipspace(ll);
}
/************************************************************************/

/*******************************************************************/
char *getfunstm(char *ll,stm *s0)
{
	char c1[MAXCL];

        if (*ll=='{') { 
		ll=skipstring(ll,"{");
		ll=getfunstmlist(ll,s0);  
		return skipstring(ll,"}"); 
	}
	if (iskey(ll,"while")) { 
		ll=skipstring(ll,"while"); strcpy(s0->key,"while");
		ll=gcond(ll,s0->str);
        	s0->left=(stm *)calloc(1,sizeof(stm));
		ll=getfunstm(ll,s0->left); return ll;
	}
	if (iskey(ll,"for")) { 
		ll=skipstring(ll,"for"); strcpy(s0->key,"for");
		ll=gcond(ll,s0->str);
        	s0->left=(stm *)calloc(1,sizeof(stm));
		ll=getfunstm(ll,s0->left); return ll;
	}
	if (iskey(ll,"switch")) { 
		ll=skipstring(ll,"switch"); strcpy(s0->key,"switch");
		ll=gcond(ll,s0->str);
        	s0->left=(stm *)calloc(1,sizeof(stm));
		ll=getfunstm(ll,s0->left); return ll;
	}
	if (iskey(ll,"case")) { 
		ll=skipstring(ll,"case"); strcpy(s0->key,"case");
		ll=g2stm(ll,s0->str,':',0);
		ll=gcase(ll,"default",c1);
        	s0->left=(stm *)calloc(1,sizeof(stm));
		(void)getfunstm(c1,s0->left); return ll;
	}
	if (iskey(ll,"default")) { 
		ll=skipstring(ll,"default"); strcpy(s0->key,"default");
		ll=g2stm(ll,s0->str,':',0);
		ll=gcase(ll,"default",c1);
        	s0->left=(stm *)calloc(1,sizeof(stm));
		(void)getfunstm(c1,s0->left); return ll;
	}
	if (iskey(ll,"break")) { 
		ll=skipstring(ll,"break"); strcpy(s0->key,"break");
		if (*ll==';') ll=skipstring(ll,";");
		return ll;
	}
	if (iskey(ll,"continue")) { 
		ll=skipstring(ll,"continue"); strcpy(s0->key,"continue");
		if (*ll==';') ll=skipstring(ll,";");
		return ll;
	}
	if (iskey(ll,"return")) { 
		ll=skipstring(ll,"return"); strcpy(s0->key,"return");
		ll=g2stm(ll,s0->str,';',0); return ll;
	}
	if (iskey(ll,"if")) { 
		ll=skipstring(ll,"if"); strcpy(s0->key,"if");
		ll=gcond(ll,s0->str);
        	s0->left=(stm *)calloc(1,sizeof(stm));
		ll=getfunstm(ll,s0->left); 
		if (iskey(ll,"else")) {
			ll=skipstring(ll,"else"); 
        		s0->next=(stm *)calloc(1,sizeof(stm));
			ll=getfunstm(ll,s0->next); 
		}
		return ll;
	}
	if (iskey(ll,"verds_assertion")) { 
		ll=skipstring(ll,"verds_assertion"); 
		strcpy(s0->key,"verds_assertion");
		ll=g2stm(ll,s0->str,';',0); return ll;
	}
	if (iskey(ll,"ASSUMPTION")) { 
		ll=skipstring(ll,"ASSUMPTION"); strcpy(s0->key,"ASSUMPTION");
		return ll;
	}
	if (iskey(ll,"GUARANTEE")) { 
		ll=skipstring(ll,"GUARANTEE"); strcpy(s0->key,"GUARANTEE");
		return ll;
	}
	if (1) /*(isassign(ll))*/ { 
		strcpy(s0->key,"=");
		ll=g2stm(ll,s0->str,';','}'); return ll;
	}
	error(ll,5);
}
/*******************************************************************/
char *getfunstmlist(char *ll,stm *s0)
{
	while (*ll!='}') {
		strcpy(s0->key,";");
		s0->left=(stm *)calloc(1,sizeof(stm));
		ll=getfunstm(ll,s0->left);
		s0->next=(stm *)calloc(1,sizeof(stm));
		s0=s0->next;
	}
	return ll;
}
/*******************************************************************/

/*******************************************************************/
void writestm(stm *s0)
{
	if (s0==NULL) return;
	if (strcmp(s0->key,"while")==0) { 
		fprintf(stdout,"%s (%s)",s0->key,s0->str);
		writestm(s0->left); return;
	}
	if (strcmp(s0->key,"for")==0) { 
		fprintf(stdout,"%s (%s)",s0->key,s0->str);
		writestm(s0->left); return;
	}
	if (strcmp(s0->key,"switch")==0) { 
		fprintf(stdout,"%s (%s)",s0->key,s0->str);
		writestm(s0->left); return;
	}
	if (strcmp(s0->key,"case")==0) { 
		fprintf(stdout,"%s %s:",s0->key,s0->str);
		writestm(s0->left); return;
	}
	if (strcmp(s0->key,"default")==0) { 
		fprintf(stdout,"%s %s:",s0->key,s0->str);
		writestm(s0->left); return;
	}
	if (strcmp(s0->key,"break")==0||strcmp(s0->key,"continue")==0) { 
		fprintf(stdout,"%s;\n",s0->key); 
		return; 
	}
	if (strcmp(s0->key,"return")==0) { 
		fprintf(stdout,"%s %s;\n",s0->key,s0->str); 
		return; 
	}
	if (strcmp(s0->key,"if")==0) { 
		fprintf(stdout,"%s (%s) ",s0->key,s0->str);
		writestm(s0->left); 
		if (s0->next!=NULL) {
			fprintf(stdout,"else ");
			writestm(s0->next); 
		}
		return;
	}
	if (strcmp(s0->key,";")==0) { 
		fprintf(stdout,"{\n");
		writestm(s0->left); 
		while (s0->next!=NULL) { writestm(s0->next->left); s0=s0->next;}
		fprintf(stdout,"}\n");
		return;
	}
	if (strcmp(s0->key,"verds_assertion")==0) { 
		fprintf(stdout,"\t%s%s;\n",s0->key,s0->str); return;
	}
	if (strcmp(s0->key,"ASSUMPTION")==0) { 
		fprintf(stdout,"%s\n",s0->key); return;
	}
	if (strcmp(s0->key,"GUARANTEE")==0) { 
		fprintf(stdout,"%s\n",s0->key); return;
	}
	if (strcmp(s0->key,"=")==0) { 
		fprintf(stdout,"%s;\n",s0->str);
		writestm(s0->left); 
		return;
	}
}
/*******************************************************************/
void writepar(par *p0)
{
	if (p0==NULL) return;
	fprintf(stdout,"%s %s",p0->type,p0->name);
	if (p0->next!=NULL) fprintf(stdout,",");
	writepar(p0->next);
}
/*******************************************************************/
void writevar(par *p0)
{
	if (p0==NULL) return;
	if (*p0->init==0) strcpy(p0->init,"0");
	fprintf(stdout,"%s %s=%s;\n",p0->type,p0->name,p0->init);
	writevar(p0->next);
}
/*******************************************************************/
void writefun(fun *ff)
{
/*
 infos("writefun");
*/
	if (ff==NULL) return;
	if (ff->name[0]==0) return;
	fprintf(stdout,"%s %s(",ff->type,ff->name); 
	writepar(ff->p0);
	fprintf(stdout,")\n"); 
	fprintf(stdout,"{\n"); writevar(ff->v0); writestm(ff->s1); 
	fprintf(stdout,"}\n"); 
	writefun(ff->next);
}
/*******************************************************************/
