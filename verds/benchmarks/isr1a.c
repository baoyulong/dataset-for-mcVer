#include <stdio.h>
/**************************************************/
int in();
int isr(int x,int k);
int isk(int n,int k);
/**************************************************/
main(int argc, char **argv )
{
	int n=0,m=0,k=1;
	printf("INFO:\tthe system is waiting for inputs\n\n"); 
	while (1) {
		n=in();
		k=isk(n,k);
		m=isr(n,k);
        	printf("RESULT: %i\n\n",m);
	}
}
/**************************************************/
int isr(int x,int k)
{
	int y0=0,y1=0,y2=1,y3=1;
	if (k<=1) y0=1; else y0=k%2;
	while (y3<x+y0) { y1=y1+1; y2=y2+2; y3=y3+y2; }
	return y1;
}
/**************************************************/
int isk(int n,int k)
{
	if (k!=n) k=101; else { k=k+2; if (k>100) k=100; }
	return k;
}
/**************************************************/
int in()
{
    char c=0; int k=0;
    while (1) { 
	k=0;
	putc('N',stdout); putc(':',stdout); putc(9,stdout);
	c=getc(stdin); /* printf("%i\n",c); */
	if (c=='\n') { 
	    printf("INFO:\tthe input must be 1 or 2 digits\n\n"); 
	    continue;
	}
	if (c<'0'||c>'9') {
	    while (1) { 
	    	c=getc(stdin); 
	    	if (c=='\n') break;
	    }
	    printf("INFO:\tthe input must be 1 or 2 digits\n\n"); 
	    continue;
	}
	k=c-'0'; 
	c=getc(stdin); 
	if (c=='\n') { return k; }
	if (c<'0'||c>'9') {
	    while (1) { c=getc(stdin); if (c=='\n') break; }
	    printf("INFO:\tthe input must be 1 or 2 digits\n\n"); 
	    continue;
	}
	k=k*10+(c-'0'); 
	c=getc(stdin); 
	if (c!='\n') {
	    while (1) { c=getc(stdin); if (c=='\n') break; }
	    printf("INFO:\tthe input must be 1 or 2 digits\n\n"); 
	    continue;
	}
	return k;
    }
}
/******************************************************************/
