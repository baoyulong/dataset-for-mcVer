#include "modbin.h"

/*******************************************************************/
char *bin0(char **lcl,int *a,int lc,char *varname,int maxt,char *invcond)
{
	int i;
	char *x,*y;
	char *s,*s1;

	stringnew(s,1024); sprintf(s,"%s=%i",varname,maxt); 
	if (invcond!=NULL) { stringconj(s1,s,invcond); free(s); s=s1; }

	for (i=0;i<lc;i++) {
		stringnew(x,1024);
		if (a[i]<0) sprintf(x,"%s>1",lcl[i]); 
		else sprintf(x,"%s=0",lcl[i]); 
		stringconj(y,s,x); free(x); free(s); s=y;
	}
	return s;
}
/*******************************************************************/
char *bin1(char **lcl,int *a,int lc,char *varn,char *invcond)
{
	int i;
	char *x,*y;
	int t=0;
	char *s;

	for (i=0;i<lc;i++) if (a[i]<0) t++;
	if (invcond!=NULL) { 
		stringnew(s,1024); sprintf(s,"%s=%s-(1-1)",varn,varn); 
		for (i=0;i<lc;i++) if (a[i]<0) {
		   	stringnew(x,1024); sprintf(x,"%s=%s-1",lcl[i],lcl[i]);
		   	stringconj(y,s,x); free(x); free(s); s=y;
		}
		return s;
	}

	switch (t) {
		case 0: stringnew(s,1024); sprintf(s,"%s=1",varn); 
			return s; 
		case 1: for (i=0;i<lc;i++) if (a[i]<0) break;
			stringnew(s,1024); 
			sprintf(s,"%s=(%s-%s)+1",varn,varn,lcl[i]); 
			stringnew(x,1024); sprintf(x,"%s=1",lcl[i]); 
			stringconj(y,s,x); free(x); free(s); s=y;
			return s;
		default:stringnew(s,1024); sprintf(s,"%s=%s-1",varn,varn); 
			for (i=0;i<lc;i++) if (a[i]<0) {
			  stringnew(x,1024); sprintf(x,"%s=%s-1",lcl[i],lcl[i]);
			  stringconj(y,s,x); free(x); free(s); s=y;
			}
			return s;
	}
}
/*******************************************************************/

/*******************************************************************/
int bin(int n,int k)
{
	int i;
	int m0,m1;

	for (m0=1,i=0;i<k;i++) m0=m0*(n-i);
	for (m1=1,i=0;i<k;i++) m1=m1*(k-i);
	return m0/m1;
}
/*******************************************************************/
void binmarkpos(int *a,int n,int k,int i)
{
	int j;
	int x;

	if (k==0) return;
	for (j=1;1;j+=1) { x=bin(n-j,k-1); if (i<=x) break; i-=x; }

	a[j-1]*=-1;
	binmarkpos(&(a[j]),n-j,k-1,i);
}
/*******************************************************************/
