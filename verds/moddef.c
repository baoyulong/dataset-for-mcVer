#include "moddef.h"

/*******************************************************************/
#define MAXITEM       0x00001000
#define MAXITEML      0x00001000
/*******************************************************************/
char *rmDEF(char *s,MODdefinition *md)
{
	int i;

	if (md->n==-1) {
	  md->left=(char **)malloc(MAXITEM*sizeof(char *));
	  md->right=(char **)malloc(MAXITEM*sizeof(char *));
	  for (i=0;i<MAXITEM;i++) {
		md->left[i]=(char *)malloc(MAXITEML*sizeof(char));
	  	md->right[i]=(char *)malloc(MAXITEML*sizeof(char));
	  }
	  md->n=getdefinitions(s,md->left,md->right);
	  s=removedefinitions(s);
	}
	
	s=rmDEF0(s,md->left,md->right,md->n); return s;
}
/***********************************************************************/
void rmDEFfree(MODdefinition *md)
{
	int i;
	for (i=0;i<MAXITEM;i++) { free(md->right[i]); free(md->left[i]); }
	free(md->right); free(md->left); free(md); 
}
/***********************************************************************/
int getdefinitions(char *s,char **left,char **right)
{
	int cnt;

    	for (;*s;s++) if(strncmp(s,"DEFINE",6)==0||strncmp(s,"VAR",3)==0)break;
    	if (strncmp(s,"VAR",3)==0) return 0;
    	for (cnt=0,s+=6;*s;) {
		while (isws(*s)) s++;
    		if (*s==0||strncmp(s,"VAR",3)==0) break;
		s=getTXTword(s,left[cnt]); 
		while (isws(*s)) s++;
    		if (*s++!='=') { error("missing = in DEFINE"); }
		while (isws(*s)) s++;
		s=getTXTline(s,right[cnt]); 
		cnt++;
	}
	return cnt;
}
/***********************************************************************/
char *removedefinitions(char *s)
{
	int i;
	char *s0=s;

    	for (;*s;s++) if(strncmp(s,"DEFINE",6)==0||strncmp(s,"VAR",3)==0)break;
    	if (strncmp(s,"VAR",3)==0) return s0;
    	for (i=0;i<6;i++) *s++=SP;
    	for (;*s;s++) { if (strncmp(s,"VAR",3)==0) break; if (*s!='\n') *s=SP; }
	return s0;
}
/*******************************************************************/
char *rmDEF0(char *s,char **left,char **right,int n)
{
	int i;

	for (i=n-1;i>=0;i--) { s=rmDEFone(s,left[i],right[i]); }
	return s;
}
/***********************************************************************/
char *rmDEFone(char *s,char *left,char *right)
{
	int ms;
	char *t,*s0,*t0;
	int a,b;
	int succ=0;

	ms=16*(strlen(s)+1); /*MAXSTRING 0x4000*/
	a=strlen(left);
	b=strlen(right);
	t=(char *)malloc(ms*sizeof(char));
	s0=s;
	t0=t;

	if (a==0) { free(t); return s0; }

	while (*s) {
		if (strncmp(s,left,a)==0&&
		    ((s==s0||isdel(*(s-1)))&&(*(s+a)==0||isdel(*(s+a))))) {
				strcpy(t,right); t+=b; s+=a; succ=1;
		} else { *t++=*s++; }
	}
	if (succ) {
		*t=0;
		t0=(char *)realloc(t0,(t-t0+2)*sizeof(char)); 
		free(s0); return t0;
	}
	free(t0); return s0;
}
/***********************************************************************/
char *rmREPanyone(char *s,char *left,char *right)
{
	int ms;
	char *t,*s0,*t0;
	int a,b;
	int succ=0;

	ms=16*(strlen(s)+1); /*MAXSTRING 0x4000*/
	a=strlen(left);
	b=strlen(right);
	t=(char *)malloc(ms*sizeof(char));
	s0=s;
	t0=t;

	if (a==0) { free(t); return s0; }

	while (*s) {
		if (strncmp(s,left,a)==0) { strcpy(t,right);t+=b;s+=a;succ=1;} 
		else { *t++=*s++; }
	}
	if (succ) {
		*t=0;
		t0=(char *)realloc(t0,(t-t0+2)*sizeof(char)); 
		free(s0); return t0;
	}
	free(t0); return s0;
}
/***********************************************************************/
char *rmREPsearch(char *s,char *left)
{
	int a=strlen(left);

	while (*s) { if (strncmp(s,left,a)==0) return s; s++; }
	return NULL;
}
/***********************************************************************/

/*******************************************************************/
char *rmCOM(char *s)
{
	char *s0=s;

	for (;*s;s++) 
	if (*s=='/'&&*(s+1)=='/') { 
		*s++=SP; *s++=SP;
		while (*s&&*s!='\n') *s++=SP;
		if (*s=='\n') s++;
	} else if (*s=='/'&&*(s+1)=='*') { 
		*s++=SP; *s++=SP;
		while (1) {
		   	while (*s&&*s!='*') *s++=0;
		   	if (*s=='*'&&*(s+1)=='/') { *s++=SP; *s++=SP; break; }
		   	*s++=SP;
		}
	}
	return s0;
}
/*******************************************************************/

/*******************************************************************/
#define MAXITEM       0x00001000
#define MAXITEML      0x00001000
/*******************************************************************/
char *rmENUM(char *s)
{
	int i,n;
	char *s0=s;
	char **left,**right;

	left=(char **)malloc(MAXITEM*sizeof(char *));
	right=(char **)malloc(MAXITEM*sizeof(char *));
	for (i=0;i<MAXITEM;i++) left[i]=(char *)malloc(MAXITEML*sizeof(char));
	for (i=0;i<MAXITEM;i++) right[i]=(char *)malloc(MAXITEML*sizeof(char));
	
	n=getENUMdefinitions(s,left,right);
	s=rmENUM0(s,left,right,n);
	for (i=0;i<MAXITEM;i++) { free(right[i]); free(left[i]); }
	free(right); free(left); 
	return s;
}
/***********************************************************************/
int getENUMdefinitions(char *s,char **left,char **right)
{
	int cnt=0;
	int i,lcnt;
	char *s1,*x1;
	char *alist=(char *)malloc((strlen(s)+1)*sizeof(char));


	while (1) {
    		for (;*s;s++) if((strncmp(s,"VAR",3)==0)) break;
    		if (*s==0) break; 
    		for (s+=3;*s;) {
    			if((strncmp(s,"INIT",4)==0)) break; 
			if (*s!='{') { s++; continue; }
			s=getTXTuntilchar(s+1,alist,'}');
			lcnt=getTXTatomlist(alist,&(left[cnt]),',',0);
    			for (i=0;i<lcnt;i++) sprintf(right[cnt+i],"%i",i); 
			sprintf(left[cnt+lcnt],"{%s}",alist);
    			sprintf(right[cnt+i],"0..%i",lcnt-1); lcnt++;
			cnt+=lcnt;
		}
	}
	free(alist); return cnt;
}
/***********************************************************************/
char *rmENUM0(char *s,char **left,char **right,int n)
{
	int i,k;
	char **l0=(char **)malloc(MAXITEM*sizeof(char *));
	char **l1=(char **)malloc(MAXITEM*sizeof(char *));

	for (k=0,i=0;i<n;i++) 
	if (left[i][0]=='{') { l0[k]=left[i]; l1[k]=right[i]; k++; }
/*
	for (i=0;i<n;i++) { infos(left[i]); infos(right[i]); }
*/
	s=rmDEF0(s,l0,l1,k); 
	for (i=0;i<k;i++) l0[i][0]=0; free(l0); free(l1); 

	s=rmDEF0(s,left,right,n); 

	return s;
}
/***********************************************************************/

/*******************************************************************/
/*******************************************************************/
char *rmFOR(char *s)
{
	char *s1;
        for (s1=NULL;s1!=s;s=rmFOR1(s1)) s1=s;
	return s;
}
/*******************************************************************/
char *rmFOR1(char *s)
{
	char *s0=s;
	char *t0,*t1;
	char *t0a,*t0b,*t0c;
	char name[1024];
	char *s1;
	int a,n,low,high;
	char *u0;
	char **list;

	if ((s1=rmREPsearch(s,"for"))==NULL) return s;

	stringcpy(t0,s1); t1=t0;
	t1+=3;

		skipws(t1); if (*t1++!='(') error("FOR");
		t1=getTXTatom(t1,name);
		skipws(t1); if (*t1++!=':') error("FOR:");

		if (*t1=='[') {
			skipws(t1); if (*t1++!='[') error("FOR[");
			skipws(t1); low=atoi(t1);
			while (isdigit(*t1)) t1++;
			if (strncmp(t1,"..",2)!=0) error("FOR.."); t1+=2;
			skipws(t1); high=atoi(t1);
			while (isdigit(*t1)) t1++;
			skipws(t1); if (*t1++!=']') error("FOR]");
			skipws(t1); if (*t1++!=')') error("FOR)");
			list=(char **)malloc((high-low+1)*sizeof(char *));
			for (n=0,a=low;a<=high;a++,n++) {
				stringcpy(list[a-low],s1);
				sprintf(list[a-low],"%i",a);
			}
		} else {
			skipws(t1); if (*t1++!='{') error("FOR{");
			stringnew(u0,strlen(t1));
			t1=getTXTuntilchar(t1,u0,'}'); 
			skipws(t1); if (*t1++!=')') error("FOR)");
			list=(char **)malloc((1024)*sizeof(char *));
			n=getTXTanylist(u0,list,',',1); free(u0);
		}
		stringnew(t0a,strlen(t1));
		skipws(t1); if (*t1++!='{') error("FOR{");
		t1=getTXTuntilchar(t1,t0a,'}'); *t1=0;
		t0b=NULL; while (t0b!=t0a) { t0b=rmFOR(t0a); t0a=t0b; }
		t0b=expandfor(t0a,name,list,n); free(t0a);
		for (a=0;a<n;a++) free(list[a]); free(list);
	t0c=rmDEFone(s0,t0,t0b); free(t0), free(t0b); return t0c;
}
/*******************************************************************/
char *expandfor(char *s1,char *left,char **rlist,int n)
{
	int tt=0;
	char *t0,*t1;
	char right[1024];
	int a;
	char **list=(char **)malloc((n)*sizeof(char *));

	if (rmREPsearch(s1,":")==NULL) tt=1;
	for (a=0;a<n;a++) {stringcpy(t0,s1);list[a]=rmDEFone(t0,left,rlist[a]);}
	for (t0=list[0],a=1;a<n;a++) {
	    if (tt==0){stringcat(t1,t0,list[a]);}else stringconj(t1,t0,list[a]);
	    free(list[a]); free(t0); t0=t1;
	}
	free(list); return t0;
}
/*******************************************************************/

/*******************************************************************/
#define MAXITEM       0x00001000
#define MAXITEML      0x00001000
/*******************************************************************/
char *rmPDEF(char *s)
{
	int i,n;
	char *s0=s;
	char **left,**right;

	left=(char **)malloc(MAXITEM*sizeof(char *));
	right=(char **)malloc(MAXITEM*sizeof(char *));
	for (i=0;i<MAXITEM;i++) left[i]=(char *)malloc(MAXITEML*sizeof(char));
	for (i=0;i<MAXITEM;i++) right[i]=(char *)malloc(MAXITEML*sizeof(char));
	
	n=getpdefinitions(s,left,right);
	s=rmDEF0(s,left,right,n);
	for (i=0;i<MAXITEM;i++) { free(right[i]); free(left[i]); }
	free(right); free(left); 
	return s;
}
/***********************************************************************/
int getpdefinitions(char *s,char **left,char **right)
{
	int i,cnt;
	char *s1,*x;
	char t1[1024];

    	for (;*s;s++) if(strncmp(s,"PROC",4)==0||strncmp(s,"MODULE",6)==0)break;
    	if (*s==0||strncmp(s,"MODULE",6)==0) return 0;
	stringcpy(s1,s); getTXTuntilkeys(s+4,s1,10);
	cnt=getTXTanylist(s1,left,';',0); free(s1);
	for (i=0;i<cnt;i++) {
		x=left[i]; while (*x&&*x!=':'&&*x!=' ') x++; *x=0;
		strcpy(right[i],left[i]);
		for (x=right[i];*x;x++) 
		if (*x=='[') { strcpy(t1,x+1); strcpy(x,"_x_"); strcpy(x+3,t1);}
		else  if (*x==']') { strcpy(t1,x+1); strcpy(x,t1); } 
	}
	return cnt;
}
/***********************************************************************/

/********************************************************************/
LOCAL int klengths[10]={6,4,6,3,3,4,5,8,3,4};
LOCAL char keywords[10][10]={
		"MODULE","PROC","DEFINE","VVM",
		"VAR","INIT","TRANS","FAIRNESS","INV","SPEC"
	};
/********************************************************************/
char *MODfile2string(char *fn)
{
        int len;
        char *ff;
        FILE *f;

        f=fopen(fn,"r");
        fseek(f,0,2); len=ftell(f); fseek(f,0,0);
        ff =(char*) malloc(len+1); fread(ff,len,1,f); ff[len-1]=0; fclose(f);
        return ff;
}
/*******************************************************************/
char *getTXTatom(char *s,char *w)
{
	char *s0=s;
	while (*s&&isws(*s)) s++;

 	if (w==NULL) error("getTXTatom, NULL pointer");
	for (;*s;) {if(!(isatomchar(*s)))break; *w++=*s++; }
	*w=0; return s;
}
/*******************************************************************/
char *getTXTword(char *s,char *w)
{
	char *s0=s;
	while (*s&&isws(*s)) s++;
	for (;*s;) {if(!(iswordchar(*s)))break;*w++=*s++;}
	*w=0; return s;
}
/*******************************************************************/
char *getTXTvar(char *s,char *w)
{
	char *s0=s;
	while (*s&&isws(*s)) s++;
	for (;*s;) {if(!(isvarchar(*s)))break;*w++=*s++;}
	*w=0; return s;
}
/*******************************************************************/
char *getTXTline(char *s,char *w)
{
	char *s0=s;
	for (;*s;) { if (*s=='\\') s+=2; if (*s=='\n') break; *w++=*s++; } 
	*w=0; return s;
}
/*******************************************************************/
char *getTXTuntilkeys(char *s,char *w,int n)
{
	int i;
	char *s0=s;
	for (;*s;) { 
		for (i=0;i<n;i++) 
		if (strncmp(s,keywords[i],klengths[i])==0) { *w=0; return s; }
		*w++=*s++; 
	}
	*w=0; return s;
}
/*******************************************************************/
char *getTXTuntilcharorend(char *s,char *w,char c)
{
	int p1,p2,p3;
	char *s0=s;
	for (p1=p2=p3=0;;) { 
		if (*s==0||*s==c&&p1==0&&p2==0) {*w=0; if (*s) s++; return s;}
		switch (*s) {
			case '(': p1++; break;
			case ')': p1--; break;
			case '{': p2++; break;
			case '}': p2--; break;
			case '[': p3++; break;
			case ']': p3--; break;
		}
		*w++=*s++; 
	}
	error("getTXTuntilcharorend"); 
}
/*******************************************************************/
char *getTXTuntilchar(char *s,char *w,char c)
{
	s=getTXTuntilcharorend(s,w,c);
	if (*(s-1)!=c) error("getTXTuntilchar");
	return s;
}
/*******************************************************************/
int getTXTatomlist(char *s,char **w,char d,int *aall)
{
	int lcnt=0;

	while (*s&&isws(*s)) s++; 
	while (1) {
        	if (aall) w[lcnt]=(char *)malloc((strlen(s)+1)*sizeof(char));
		s=getTXTatom(s,w[lcnt]); lcnt++;
		while (*s&&isws(*s)) s++; if (*s==0||*s==')') break;
		if (*s++!=d) { infos(s); error("missing delimeter atom-list"); }
	}
	return lcnt;
}
/*******************************************************************/
int getTXTwordlist(char *s,char **w,char d,int aall)
{
	int lcnt=0;

	while (*s&&isws(*s)) s++; 
	while (1) {
        	if (aall) w[lcnt]=(char *)malloc((strlen(s)+1)*sizeof(char));
		s=getTXTword(s,w[lcnt]); lcnt++;
		while (*s&&isws(*s)) s++; if (*s==0||*s==')') break;
		if (*s++!=d) { infos(s); error("missing delimeter word-list"); }
	}
	return lcnt;
}
/*******************************************************************/
int getTXTanyliststrict(char *s,char **w,char d,int aall)
{
	int lcnt=0;

	while (*s&&isws(*s)) s++; 
	while (1) {
        	if (aall) w[lcnt]=(char *)malloc((strlen(s)+1)*sizeof(char));
		s=getTXTuntilchar(s,w[lcnt],d); lcnt++;
		while (*s&&isws(*s)) s++; if (*s==0) break;
	}
	return lcnt;
}
/*******************************************************************/
int getTXTanylist(char *s,char **w,char d,int aall)
{
	int lcnt=0;

	while (*s&&isws(*s)) s++; 
	while (1) {
        	if (aall) w[lcnt]=(char *)malloc((strlen(s)+1)*sizeof(char));
		s=getTXTuntilcharorend(s,w[lcnt],d); lcnt++;
		while (*s&&isws(*s)) s++; if (*s==0) break;
	}
	return lcnt;
}
/*******************************************************************/
char *move2word(char *s,char *w)
{
	int a=strlen(w);
	int b=strlen(s);

	if (a==0) return NULL;
	while (b-a>=0) { if (strncmp(s,w,a)==0) return s; s++; b--; }
	return NULL;
}
/*******************************************************************/
int getTXTatomicp(char *s2,char *t2,char *t1)
{
	int i;

	for (;*t2;t2--) if (!(iswordchar(*t2)||isrel(*t2))) break; t2++;
	for (;*t2;t2) {
		if (!(iswordchar(*t2)||isrel(*t2))) break; 
		*t1++=*t2++; i++; 
	}

	*t1=0; return i;
}
/***********************************************************************/
int isArray(char *p)
{
	while (*p&&*p!='[') p++; return (*p=='[');
}
/***********************************************************************/
