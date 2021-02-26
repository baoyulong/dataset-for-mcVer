 #include <stdio.h>
 /**************************************************/
 main(int argc,char **argv ) {
   int n=0,m=0;
   printf("INFO:   system is now active\n",0);
   while (1) {
     n=in(); m=f91(n);
     printf("RESULT: %i\n\n",m);
   }
 }
 /**************************************************/
 int in() {
   char c=0;
   char d=0;
   while (1) {
     putc('N',stdout); putc(':',stdout); putc(9,stdout); c=getc(stdin);
     if (c<'0'||c>'9') {
       while (1) { c=getc(stdin); if (c=='\n') break; }
       printf("INFO:   the input must be a digit\n\n"); continue;
     }
     d=getc(stdin); if (d=='\n') { return c-'0'; }
     printf("INFO:   the input must be 1 digit\n\n");
   }
 }
 /**************************************************/
 int f91(int x) {
   int y1=x; int y2=1; int z=0;
   while (1) {
     if (y1>100) {if (y2!=1) { y1=y1-10; y2=y2-1; } else { z=y1-10; break; }}
     else { y1=y1+11; y2=y2+1; }
   }
   return z;
 }
 /**************************************************/
