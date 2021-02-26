#include "tbdop.h"

/************************************************************************/
int iTBDmp(TBD *w)
{
        TBD *r,*a,*b,*c;

	if (isomg(w)||istau(w)) return isomg(w);
        if (w->x>0) {
		if (istau(w->c)) return iTBDmp(w->a)&&iTBDmp(w->b);
		return iTBDmp(TBDcn(w->a,w->c))&&iTBDmp(TBDcn(w->b,w->c));
	}
        return iTBDmp(TBDnnd(w->a,w->c))&&iTBDmp(TBDnnd(w->b,w->c));
}
/*******************************************************************/
int iTBDeq(TBD *s,TBD *t)
{
/*
	if (s==t) return 1;
	if (isomg(s)) return iTBDmp(t); if (istau(s)) return iTBDmp(TBDng(t));
	if (isomg(t)) return iTBDmp(s); if (istau(t)) return iTBDmp(TBDng(s));
	switch (TBDcase(s->x,t->x)) {
	case 0: return iTBDeq(TBDng(s),TBDng(t)); 
	case 1: return iTBDeq(TBDnnd(s->a,s->c),TBDcn(t->a,t->c))&&
			iTBDeq(TBDnnd(s->b,s->c),TBDcn(t->b,t->c));
	case 2: return iTBDeq(TBDnnd(t->a,t->c),TBDcn(s->a,s->c))&&
			iTBDeq(TBDnnd(t->b,t->c),TBDcn(s->b,s->c));
	case 3: return iTBDeq(TBDcn(t->a,t->c),TBDcn(s->a,s->c))&&
			iTBDeq(TBDcn(t->b,t->c),TBDcn(s->b,s->c));
	case 4: return iTBDeq(TBDnnd(s->a,s->c),t)&&iTBDeq(TBDnnd(s->b,s->c),t);
	case 5: return iTBDeq(TBDcn(s->a,s->c),t)&&iTBDeq(TBDcn(s->b,s->c),t);
	case 6: return iTBDeq(TBDnnd(t->a,t->c),s)&&iTBDeq(TBDnnd(t->b,t->c),s);
	case 7: return iTBDeq(TBDcn(t->a,t->c),s)&&iTBDeq(TBDcn(t->b,t->c),s);
	}
	return TBDmp(TBDcn(s,TBDng(t)))&&TBDmp(TBDcn(t,TBDng(s)));
*/
	return TBD0cm(TBDds(TBDcn(s,t),TBDcn(TBDng(s),TBDng(t))));
}
/*******************************************************************/

/************************************************************************/
int TBD0cm(TBD *s)      /*completeness*/
{
        return (TBDiterativeabs(s)==TBDtau);
}
/************************************************************************/
int TBDmp(TBD *s)
{
        return TBD0cm(TBDng(s));
}
/************************************************************************/
TBD *TBDbs(int x,TBD *w)
{
	TBD *w0=TBDrestrict(w,x);

	if (w==w0) return w; 
	return TBDcn(w0,TBDrestrict(w,-x));
}
/************************************************************************/
TBD *TBDxb(int x,TBD *s)
{
        return TBDng(TBDbs(x,TBDng(s)));
}
/************************************************************************/
TBD *TBDiterativeabs(TBD *w)
{
	int x;

        if (istoo(w)) return w;
	x=TBDmaxlabel(w); TBDmmnodeclear(w,TBDCSTMAX);
        return TBDiterativeabs(TBDbs(x,w));
}
/************************************************************************/
/*
TBD *TBDiterativeabs(TBD *w)
{
	int x;
	int i;

        if (istoo(w)) return w;
	x=TBDmaxlabel(w); TBDmmnodeclear(w,TBDCSTMAX);
	for (i=x;i>=1;i--) w=TBDbs(x,w); return w;
	for (i=1;i<=x;i++) w=TBDbs(x,w); return w;
}
/************************************************************************/
int TBDmaxlabel(TBD *w)
{
        int x,y,z,n;

        if (istoo(w)||w->y>0) return 0; w->y+=TBDCSTMAX;
        x=TBDmaxlabel(w->a); y=TBDmaxlabel(w->b); z=TBDmaxlabel(w->c);
        n=abs(w->x); if (n>x&&n>y&&n>z) return n;
        if (x>y) return (x>z?x:z); else return (y>z?y:z);
}
/************************************************************************/
TBD *TBDiterativeabseven(TBD *w,int even)
{
	int i,x,y;

        if (istoo(w)) return w;
	x=TBDmaxlabel(w); TBDmmnodeclear(w,TBDCSTMAX); if (x%2==even) x--;
        y=abs(w->x); if (y%2==even) y++; 
        for (i=x;i>=y;i-=2) w=TBDbs(i,w); return w;
}
/************************************************************************/

/************************************************************************/
TBD *TBDap(int a)
{
	if (a>0) return TBDnewnode(a,TBDomg,TBDtau,TBDtau); 
	return TBDnewnode(-a,TBDtau,TBDomg,TBDtau); 
}
/************************************************************************/
TBD *TBDng(TBD *s)
{
	checkng(s); return TBDnewnode(-s->x,s->a,s->b,s->c);
}
/************************************************************************/
TBD *TBDngpos(TBD *s)
{
	return TBDng(s);
}
/************************************************************************/

/************************************************************************/
/*
#define TBDoDISa
/************************************************************************/
TBD *TBDcn(TBD *s,TBD *t)
{
        TBD *r,*r0,*a,*b,*c;
	int k;

	checkcn(s,t); if (s==t) return s; getctb(r0,TBDCSTND,s,t);
	switch (TBDcase(s->x,t->x)) {
/*
	    case 0: a=TBDds(TBDcn(s->a,s->c),TBDcn(t->a,t->c));
		    b=TBDds(TBDcn(s->b,s->c),TBDcn(t->b,t->c)); c=TBDtau; 
		    r=TBDnewnode(t->x,a,b,c); break;
	    case 1: a=TBDcn(TBDcn(t->a,t->c),TBDnnd(s->a,s->c));
                    b=TBDcn(TBDcn(t->b,t->c),TBDnnd(s->b,s->c)); c=TBDtau;
		    r=TBDnewnode(t->x,a,b,c); break;
	    case 2: a=TBDcn(TBDcn(s->a,s->c),TBDnnd(t->a,t->c));
                    b=TBDcn(TBDcn(s->b,s->c),TBDnnd(t->b,t->c)); c=TBDtau;
		    r=TBDnewnode(s->x,a,b,c); break;
	    case 3: a=TBDcn(s->a,t->a); b=TBDcn(s->b,t->b); 
		    c=TBDcn(s->c,t->c); r=TBDnewnode(s->x,a,b,c); break;
	    case 4: a=TBDds(TBDng(t),(s->a)); b=TBDds(TBDng(t),(s->b));
                    c=TBDds(TBDng(t),(s->c)); r=TBDnewnode(s->x,a,b,c); break;
	    case 5: r=TBDnewnode(s->x,s->a,s->b,TBDcn(s->c,t)); break;
	    case 6: a=TBDds(TBDng(s),(t->a)); b=TBDds(TBDng(s),(t->b));
                    c=TBDds(TBDng(s),(t->c)); r=TBDnewnode(t->x,a,b,c); break;
	    case 7: r=TBDnewnode(t->x,t->a,t->b,TBDcn(t->c,s)); break;
*/
	    case 0: a=TBDnnd(TBDnnd(s->a,s->c),TBDnnd(t->a,t->c));
		    b=TBDnnd(TBDnnd(s->b,s->c),TBDnnd(t->b,t->c)); c=TBDtau; 
		    r=TBDnewnode(t->x,a,b,c); break;
	    case 1: a=TBDnnd(TBDcn(t->a,t->c),TBDnnd(s->a,s->c));
                    b=TBDnnd(TBDcn(t->b,t->c),TBDnnd(s->b,s->c)); c=TBDtau;
		    r=TBDnewnode(s->x,a,b,c); break;
	    case 2: a=TBDnnd(TBDcn(s->a,s->c),TBDnnd(t->a,t->c));
                    b=TBDnnd(TBDcn(s->b,s->c),TBDnnd(t->b,t->c)); c=TBDtau;
		    r=TBDnewnode(t->x,a,b,c); break;
	    case 3: a=TBDcn(s->a,t->a); b=TBDcn(s->b,t->b); 
		    c=TBDcn(s->c,t->c); r=TBDnewnode(s->x,a,b,c); break;
	    case 4: a=TBDnnd(t,TBDng(s->a)); b=TBDnnd(t,TBDng(s->b));
                    c=TBDnnd(t,TBDng(s->c)); r=TBDnewnode(s->x,a,b,c); break;
	    case 5: r=TBDnewnode(s->x,s->a,s->b,TBDcn(s->c,t)); break;
	    case 6: a=TBDnnd(s,TBDng(t->a)); b=TBDnnd(s,TBDng(t->b));
                    c=TBDnnd(s,TBDng(t->c)); r=TBDnewnode(t->x,a,b,c); break;
	    case 7: r=TBDnewnode(t->x,t->a,t->b,TBDcn(t->c,s)); break;
	}
	putctb(r0,r);
}
/************************************************************************/
#ifndef TBDoDISa
TBD *TBDds(TBD *s,TBD *t)
{
        return TBDng(TBDcn(TBDng(s),TBDng(t)));
}
#else
TBD *TBDds(TBD *s,TBD *t)
{
        TBD *r,*r0,*a,*b,*c;
	int k;

	checkds(s,t); if (s==t) return s; 
/*
        if (s->x<0||t->x<0) return TBDng(TBDcn(TBDng(s),TBDng(t)));
	getctb(r0,TBDCSTDS,s,t);
*/
	getctbds(r0,TBDCSTND,s,t);

        switch (TBDcase(s->x,t->x)) {
            case 0: a=TBDcn(s->a,t->a); b=TBDcn(s->b,t->b);
                    c=TBDcn(s->c,t->c); r=TBDnewnode(s->x,a,b,c); break;
/*
            case 1: a=TBDcn(TBDcn(s->a,s->c),TBDng(TBDcn(t->a,t->c))); 
            	    b=TBDcn(TBDcn(s->b,s->c),TBDng(TBDcn(t->b,t->c))); c=s->c;
                    r=TBDnewnode(s->x,a,b,TBDtau); break;
            case 2: a=TBDcn(TBDcn(t->a,t->c),TBDng(TBDcn(s->a,s->c)));
            	    b=TBDcn(TBDcn(t->b,t->c),TBDng(TBDcn(s->b,s->c))); c=t->c;
                    r=TBDnewnode(t->x,a,b,TBDtau); break;
*/
            case 1: a=TBDcn(s->a,TBDng(TBDcn(t->a,t->c))); 
            	    b=TBDcn(s->b,TBDng(TBDcn(t->b,t->c))); c=s->c;
                    r=TBDnewnode(s->x,a,b,c); break;
            case 2: a=TBDcn(t->a,TBDng(TBDcn(s->a,s->c)));
            	    b=TBDcn(t->b,TBDng(TBDcn(s->b,s->c))); c=t->c;
                    r=TBDnewnode(t->x,a,b,c); break;
            case 3: a=TBDds(TBDcn(s->a,s->c),TBDcn(t->a,t->c));
                    b=TBDds(TBDcn(s->b,s->c),TBDcn(t->b,t->c)); c=TBDtau;
                    r=TBDnewnode(s->x,a,b,c); break;
            case 4: r=TBDnewnode(s->x,s->a,s->b,TBDcn(s->c,TBDng(t))); break;
            case 5: a=TBDds(s->a,(t)); b=TBDds(s->b,(t)); c=TBDds(s->c,(t));
                    r=TBDnewnode(s->x,a,b,c); break;
            case 6: r=TBDnewnode(t->x,t->a,t->b,TBDcn(t->c,TBDng(s))); break;
            case 7: a=TBDds(t->a,(s)); b=TBDds(t->b,(s)); c=TBDds(t->c,(s));
                    r=TBDnewnode(t->x,a,b,c); break;
        }
	putctbds(r0,r);
}
#endif
/************************************************************************/

/************************************************************************/
TBD *TBDds1(TBD *s,TBD *t)
{
        TBD *r=NULL,*r0,*a,*b,*c;

	checkds(s,t); if (s==t) return s;

	getctb(r0,TBDCSTDS,s,t); 

        a=TBDflatten1(s); b=TBDflatten1(t);
        if (a!=s||b!=t) { r=TBDds(a,b); r0->c=r; return r; }

	switch (TBDcase(s->x,t->x)) {
	    case 0: a=TBDcn(s->a,t->a); b=TBDcn(s->b,t->b); 
		    c=TBDcn(s->c,t->c); r=TBDnewnode(s->x,a,b,c); break;
	    case 1: r=TBDds(s,TBDpn1(t));  break;
	    case 2: r=TBDds(TBDpn1(s),t);  break;
	    case 3: a=TBDds(TBDcn(s->a,s->c),TBDcn(t->a,t->c));
            	    b=TBDds(TBDcn(s->b,s->c),TBDcn(t->b,t->c)); c=TBDtau;
            	    r=TBDnewnode(s->x,a,b,c); break;
	    case 4: r=TBDnewnode(s->x,s->a,s->b,TBDcn(s->c,TBDng(t))); break;
	    case 5: a=TBDds(s->a,(t)); b=TBDds(s->b,(t)); c=TBDds(s->c,(t)); 
                    r=TBDnewnode(s->x,a,b,c); break;
	    case 6: 
	    case 7: r=TBDds(t,s); break;
	}
	r0->c=r; return r;
}
/************************************************************************/
TBD *TBDflatten1(TBD *s)
{
        TBD *r0,*r,*a,*b,*c;
	int k;

	if (istoo(s)||istau(s->c)||abs(s->x)!=abs(s->c->x)) return s;

	a=s->a; b=s->b; c=s->c;
	if (c->x>0) return TBDnewnode(s->x,TBDcn(a,c->a),TBDcn(b,c->b),c->c);
	return TBDnewnode(s->x,TBDcn(a,TBDnnd(c->a,c->c)),
			       TBDcn(b,TBDnnd(c->b,c->c)),
			       TBDtau);
}
/************************************************************************/
TBD *TBDflatten(TBD *s)
{
        TBD *r0,*r,*a,*b,*c;
	int k;

	if (istoo(s)) return s;
        r0=TBDmmgetc(TBDctb,TBDCSTND,NULL,s); 
	if (istoo(r0)||r0->c!=NULL) { return r0; }

	k=abs(s->x);
	a=TBDflatten(s->a); b=TBDflatten(s->b); c=TBDflatten(s->c);
	if (istoo(c)||k!=abs(c->x)) r=TBDnewnode(s->x,a,b,c);
	else if (c->x>0) r=TBDnewnode(s->x,TBDcn(a,c->a),TBDcn(b,c->b),c->c);
	else r=TBDnewnode(s->x,TBDcn(a,TBDnnd(c->a,c->c)),
			       TBDcn(b,TBDnnd(c->b,c->c)),
			       TBDtau);
	r0->c=r; return r;
}
/************************************************************************/
int islit(TBD *s)
{
	if (s->c!=TBDtau) return 0;
	if (s->a==TBDomg&&s->b==TBDtau) return s->x;
	if (s->a==TBDtau&&s->b==TBDomg) return -s->x;
	return 0;
}
/************************************************************************/

/************************************************************************/
GLOBAL int ggtype;
/************************************************************************/
TBD *TBDnewnode1(int x,TBD *a,TBD *b,TBD *c)
{
    if (isomg(a)&&isomg(b)) { if (x>0) return TBDomg; return TBDtau; }
    if (isomg(c)) { if (x>0) return TBDomg; return TBDtau; }

    if (istau(a)&&istau(b)) { if (x>0) return c; return TBDng(c); }
    if (a==b&&istau(c)) { if (x>0) return a; return TBDng(a); }

 if (ggtype&typeTBD) {
    if (isomg(b)&&!istau(a)&&istau(c)) return TBDnewnode1(x,TBDtau,TBDomg,a);
    if (isomg(a)&&!istau(b)&&istau(c)) return TBDnewnode1(x,TBDomg,TBDtau,b);
 }

    if (b==c&&!istau(c)) return TBDnewnode1(x,a,TBDtau,c);
    if (a==c&&!istau(c)) return TBDnewnode1(x,TBDtau,b,c);

    if (a==c&&istau(c)&&x<0) return TBDnewnode1(-x,TBDomg,TBDng(b),c); 
    if (b==c&&istau(c)&&x<0) return TBDnewnode1(-x,TBDng(a),TBDomg,c); 

    if (istau(a)&&isomg(b)&&x<0) return TBDnewnode1(-x,TBDng(c),TBDtau,TBDtau); 
    if (istau(b)&&isomg(a)&&x<0) return TBDnewnode1(-x,TBDtau,TBDng(c),TBDtau); 

    switch (ggtype) {
	case typeBDD: if (c!=TBDtau) { a=TBDcn(a,c); b=TBDcn(b,c); c=TBDtau; }
	case typeTBD: if (x<0) { a=TBDnnd(a,c); b=TBDnnd(b,c); c=TBDtau; x=-x; }
    }
    return TBDmmaddt(TBDttb,TBDmmnewnode(x,a,b,c));
}
/*******************************************************************/

/************************************************************************/
/*
#define TBDoRRDa
/************************************************************************/
#ifdef TBDoRRDb
#define TBDoRRDp
#define TBDoRRDa
#endif
/************************************************************************/
TBD *TBDnewnode(int x,TBD *a,TBD *b,TBD *c)
{
/*
 	if (ggtype!=0) return TBDnewnode1(x,a,b,c);
*/

        if (isomg(a)&&isomg(b)) { if (x>0) return TBDomg; return TBDtau; }
        if (isomg(c)) { if (x>0) return TBDomg; return TBDtau; }

	if (istau(a)&&istau(b)) { if (x>0) return c; return TBDng(c); }
	if (a==b&&istau(c)) { if (x>0) return a; return TBDng(a); }

#if (!(defined TBDoRRDa)&&!(defined TBDoRRDb))
        if (isomg(b)&&!istau(a)&&istau(c)) return TBDnewnode(x,TBDtau,TBDomg,a);
        if (isomg(a)&&!istau(b)&&istau(c)) return TBDnewnode(x,TBDomg,TBDtau,b);
#else
        if (isomg(a)&&istau(b)&&!istau(c)) return TBDnewnode(x,TBDomg,c,TBDtau);
        if (istau(a)&&isomg(b)&&!istau(c)) return TBDnewnode(x,c,TBDomg,TBDtau);
#endif

        if (b==c&&!istau(c)) return TBDnewnode(x,a,TBDtau,c);
        if (a==c&&!istau(c)) return TBDnewnode(x,TBDtau,b,c);

	if (a==c&&istau(c)&&x<0) return TBDnewnode(-x,TBDomg,TBDng(b),c); 
	if (b==c&&istau(c)&&x<0) return TBDnewnode(-x,TBDng(a),TBDomg,c); 

#ifdef TBDoRRd
    if (istau(a)&&isomg(b)&&x<0) return TBDnewnode(-x,TBDng(c),TBDtau,TBDtau); 
    if (istau(b)&&isomg(a)&&x<0) return TBDnewnode(-x,TBDtau,TBDng(c),TBDtau); 
#endif

#if ((defined TBDoRRDp)||(defined TBDoRRDb))
    if (x<0) return TBDnewnode(-x,TBDng(TBDcn(a,c)),TBDng(TBDcn(b,c)),TBDtau); 
#endif
#ifdef TBDoRRDb
    if (!istau(c)) return TBDnewnode(x,(TBDcn(a,c)),(TBDcn(b,c)),TBDtau); 
#endif

        return TBDmmaddt(TBDttb,TBDmmnewnode(x,a,b,c));
}
/************************************************************************/

/************************************************************************/
TBD *TBDrestrict(TBD *s,int x)
{
        TBD *a,*b,*c,*r0,*r;
	int px=abs(x);

        if (istoo(s)||abs(s->x)>px) return s; getctb(r0,TBDCSTPtype,s,(TBD *)x);
        if (s->x==px) { if (x<0) r=TBDcn(s->a,s->c); else r=TBDcn(s->b,s->c); }
	else if (s->x==-px) { 
		if (x<0) r=TBDnnd(s->a,s->c); else r=TBDnnd(s->b,s->c); } 
	else {
                a=TBDrestrict(s->a,x); b=TBDrestrict(s->b,x);
                c=TBDrestrict(s->c,x); r=TBDnewnode(s->x,a,b,c);
        }
	putctb(r0,r);
}
/************************************************************************/

/************************************************************************/
TBD *TBDpos(TBD *s)
{
	int x;
        TBD *r0,*r,*a,*b,*c;

        if (istoo(s)) { return s; }
        r0=TBDmmgetc(TBDctb,TBDCSTPS,NULL,s);
        if (istoo(r0)||r0->c!=NULL) { return r0; }

        a=s->a; b=s->b; c=s->c;
        if ((x=s->x)<0) { 
		x=-x; 
		a=TBDnnd(a,TBDrestrict(c,-x)); 
		b=TBDnnd(b,TBDrestrict(c,x)); c=TBDtau; 
	}
	r=TBDnewnode(x,TBDpos(a),TBDpos(b),TBDpos(c));

        r0->c=r; return r;
}
/*******************************************************************/
TBD *TBDbdd(TBD *s)
{
	int x;
        TBD *r0,*r,*a,*b;

        if (istoo(s)) { return s; }
	if (islit(s)) { return s; }
        r0=TBDmmgetc(TBDctb,TBDCSTBDD,NULL,s);
        if (istoo(r0)||r0->c!=NULL) { return r0; }

	a=TBDcn(s->a,s->c); b=TBDcn(s->b,s->c); 
        if ((x=s->x)<0) { x=-x; a=TBDng(a); b=TBDng(b); } 
	a=TBDrestrict(a,-x); b=TBDrestrict(b,x);
	r=TBDnewnode(x,TBDbdd(a),TBDbdd(b),TBDtau);

        r0->c=r; return r;
}
/*******************************************************************/
TBD *TBDpn1(TBD *s)
{
        TBD *a,*b;
	int x;

        a=TBDnnd(s->a,s->c); b=TBDnnd(s->b,s->c); 
	x=abs(s->x); a=TBDrestrict(a,-x); b=TBDrestrict(b,x);
	return TBDnewnode(-s->x,a,b,TBDtau);
}
/*******************************************************************/
TBD *TBDdspos(TBD *s,TBD *t)
{
        TBD *r,*r0,*a,*b,*c;

	if (istau(s)) return TBDtau; if (isomg(s)) return t;
	if (istau(t)) return TBDtau; if (isomg(t)) return s;
        if (s->x<0||t->x<0) { error("pos"); }

	getctb(r0,TBDCSTDS,s,t);
        if (s->x<t->x) {
            a=TBDdspos(s->a,t); b=TBDdspos(s->b,t);
            c=TBDdspos(s->c,t); r=TBDnewnode(s->x,a,b,c);
        } else if (s->x>t->x) { 
            a=TBDdspos(t->a,s); b=TBDdspos(t->b,s);
            c=TBDdspos(t->c,s); r=TBDnewnode(t->x,a,b,c);
        } else {
/*
            a=TBDdspos(TBDcnpos(s->a,s->c),TBDcnpos(t->a,t->c));
            b=TBDdspos(TBDcnpos(s->b,s->c),TBDcnpos(t->b,t->c));
	    c=TBDtau; r=TBDnewnode(t->x,a,b,c);
*/
            a=TBDcnpos(TBDdspos(s->a,TBDcnpos(t->a,t->c)),TBDdspos(s->c,t->a));
            b=TBDcnpos(TBDdspos(s->b,TBDcnpos(t->b,t->c)),TBDdspos(s->c,t->b));
            c=TBDdspos(s->c,t->c); r=TBDnewnode(t->x,a,b,c);
        }
        putctb(r0,r);
}
/************************************************************************/
TBD *TBDcnpos(TBD *s,TBD *t)
{
        TBD *r,*r0,*a,*b,*c;

	if (isomg(s)) return TBDomg; if (istau(s)) return t;
	if (isomg(t)) return TBDomg; if (istau(t)) return s;
        if (s->x<0||t->x<0) { error("pos"); }

	if (s->x>t->x) { c=s; s=t; t=c; } getctb(r0,TBDCSTND,s,t);
	if (s->x==t->x) {
            	a=TBDcnpos(s->a,t->a); b=TBDcnpos(s->b,t->b);
            	c=TBDcnpos(s->c,t->c); r=TBDnewnode(t->x,a,b,c);
        } else {
		c=s->c;
		while (c->x<t->x&&c!=TBDtau) c=c->c; r=TBDcnpos1(s,c,t);
	}
        putctb(r0,r);
}
/************************************************************************/
TBD *TBDrestrictpos(TBD *s,int x)
{
        TBD *a,*b,*c,*r0,*r;
	int px=abs(x);

        if (istoo(s)||abs(s->x)>px) return s; 
        if (s->x<0) { error("pos"); }

	getctb(r0,TBDCSTPtype,s,(TBD *)x);
        if (s->x==px) { if (x<0) a=s->a; else a=s->b; r=TBDcnpos(a,s->c); } 
	else {
                a=TBDrestrictpos(s->a,x); b=TBDrestrictpos(s->b,x);
                c=TBDrestrictpos(s->c,x); r=TBDnewnode(s->x,a,b,c);
        }
	putctb(r0,r);
}
/************************************************************************/
TBD *TBDcnpos1(TBD *s,TBD *c,TBD *t)
{
	if (s==c) return TBDcnpos(c,t);
	return TBDnewnode(s->x,s->a,s->b,TBDcnpos1(s->c,c,t));
}
/************************************************************************/

/************************************************************************/
TBD *TBDreplace(TBD *s,TBD **t)
{
        TBD *r,*r0,*a,*b,*c;
	int k;

	if (istoo(s)) return s;
	getctb(r0,TBDCSTREPLACE,s,t[0]);
/*
	if (s->x<0) error("TBDreplace");
*/
	a=TBDreplace(s->a,t); b=TBDreplace(s->b,t); c=TBDreplace(s->c,t);
	if (t[s->x]==TBDtau&&t[-s->x]==TBDtau) error("TBDreplace");
	if (s->x<0) {
  		a=TBDds(t[-s->x],a); b=TBDds(t[s->x],b); 
		r=TBDng(TBDcn(TBDcn(a,b),c));
	} else { 
  		a=TBDds(t[s->x],a); b=TBDds(t[-s->x],b); 
		r=TBDcn(TBDcn(a,b),c);
	}
	putctb(r0,r);
}
/************************************************************************/
TBD **TBDgeq(TBD *s,int n)
{
	int i,k;
	TBD *t0;
	TBD **y;
	int n2=2*n;

	y=(TBD **)calloc(4*n+1,sizeof(TBD *))+2*n;
	if (TSTisassign(s,n)==0) { y[0]=s; return NULL; }; 

	for (k=1;k<=n2;k+=2) {
		for (t0=s,i=1;i<=n2;i+=2) if (i!=k) t0=TBDxb(i,t0);
		y[k]=TBDpos(TBDrestrict(t0,k)); 
		y[-k]=TBDpos(TBDrestrict(t0,-k));
		TBDkp(y[k]); TBDkp(y[-k]); TSTiseq(y[k],TBDng(y[-k]));
	}
	TSTisassigneq(s,y,n);

	y[0]=s; /*for TBDmmgetc()*/
	return y;
}
/************************************************************************/
void TSTisassigneq(TBD *s,TBD **y,int n)
{
	int k;
	TBD *t0;

	for (t0=TBDtau,k=1;k<=2*n;k+=2) 
	t0=TBDcn(t0,TBDcn(TBDds(y[k],TBDap(-k)),TBDds(y[-k],TBDap(k))));
	TSTiseq(s,t0);
}
/************************************************************************/
void TSTiseq(TBD *s,TBD *t)
{
	int i;

	i=issubset(s,t)&&issubset(t,s);
	if (i==0) {
/*
		TBDwrite(s,8,stdout);
		TBDwrite(t,8,stdout);
*/
		i=(s==TBDtau)&&(t==TBDomg);
	}
	if (i==0) logs("INFO: TSTiseq failed - i==0");
}
/************************************************************************/
int TSTisassign(TBD *s,int n)
{
	int i;
	TBD *t0;

	for (t0=s,i=1;i<=2*n;i+=2) if (i!=0) t0=TBDxb(i,t0);
	if (t0==TBDtau) return 1;
	logs("INFO: TSTisassign failed - return 0");
	return 0; 
}
/************************************************************************/

/************************************************************************/
int TBDcase(int s0x,int t0x)
{
        int sx=abs(s0x),tx=abs(t0x);
	int a=(s0x>0),b=(t0x>0);

	if (sx==tx) return a*2+b; if (sx<tx) return 4+a; return 6+b; 
}
/************************************************************************/
TBD *TBDsimp(TBD *i2,int n)
{
        int i;
        TBD *a;
        int *p=(int *)calloc(2*n+1,sizeof(int));

        for (i=1;i<=2*n;i+=1) {
                a=TBDrestrict(i2,i); if ((a==TBDomg)) { p[i]=i; continue; }
                a=TBDrestrict(i2,-i); if ((a==TBDomg)) { p[i]=-i; }
        }
        for (i=1;i<=2*n;i+=2) if (p[i]) {
                i2=TBDrestrict(i2,-p[i]);
		i2=TBDcn(i2,TBDap(-p[i])); 
	}
	free(p); return i2;
}
/************************************************************************/
TBD *TBDsimpodd(TBD *i2,int n)
{
        int i;
        TBD *a;
        int *p=(int *)calloc(2*n+1,sizeof(int));

        for (i=1;i<=2*n;i+=2) {
                a=TBDrestrict(i2,i); if ((a==TBDomg)) { p[i]=i; continue; }
                a=TBDrestrict(i2,-i); if ((a==TBDomg)) { p[i]=-i; }
        }
        for (i=1;i<=2*n;i+=2) if (p[i]) {
                i2=TBDrestrict(i2,-p[i]);
		i2=TBDcn(i2,TBDap(-p[i])); 
	}
	free(p); return i2;
}
/************************************************************************/
int TBDsplit(TBD *i2,int n,int z)
{
        int i,x,y;
        int i0=0,x0=0,y0=0,k=0;
        TBD *a,*b;

        for (i=1;i<=2*n;i+=2) {
                a=TBDcn(TBDrestrict(i2,i),TBDap(i)); x=TBDmmnodecnt(a);
                b=TBDcn(TBDrestrict(i2,-i),TBDap(-i)); y=TBDmmnodecnt(b);
		if (a==b) infos("a==b");
                if (x<z&&y<z) {
                    if (x==0) return i;
                    if (y==0) return -i;
                    if (k==0||(x<x0&&y<x0)||(x<y0&&y<y0)) { i0=i; x0=x; y0=y; }
                    if (k++==8) return i0;
                }
        }
        return i0;
}
/************************************************************************/

/************************************************************************/
TBD *TBDone(TBD *t)
{
        TBD *t0,*t1;
        int x=t->x;

        if (isomg(t)||istau(t)) return t;
        if (x>0) {
                t0=TBDcn(t->a,t->c); t1=TBDomg;
                if (TBDmp(t0)) { t1=TBDcn(t->b,t->c); t0=TBDomg; }
        } else {
                t0=TBDng(TBDcn(t->a,t->c)); t1=TBDomg;
                if (TBDmp(t0)) { t1=TBDng(TBDcn(t->b,t->c)); t0=TBDomg; }
                x=-x;
        }
        return TBDnewnode(x,TBDone(t0),TBDone(t1),TBDtau);
}
/********************************************************************/
#define w1(s)	{ fprintf(ff,"%i ",(s)*y); xs[i-1]=s; }
/********************************************************************/
void *TBDwriteOne(TBD *t,int n,int k,FILE *ff)
{       
        int i,x,y,s;
	int *xs=(int *)malloc((n+1)*sizeof(int));

	xs[n]=0;
        for (y=k*n+1,i=1;i<=n;i++,y++) {
                x=2*i-1;
                if (isomg(t)) { error("TBDwriteOne isomg"); }
                else if (istau(t)) { s=1; }
                else if (t->x<x) { error("TBDwriteOne isneg"); }
                else if (t->x>x) { s=1; }
                else {
                     if (t->a==TBDomg&&t->b==TBDtau) { s=1; }
                     else if (t->b==TBDomg&&t->a==TBDtau) { s=-1; }
                     else { error("TBDwriteOne"); }
                     t=t->c;
                }
		xs[i-1]=s; 
/*
		fprintf(ff,"%i ",(s)*y); 
*/
        }
/*
	fprintf(f,"\n");
*/
	return (xs);
}
/********************************************************************/
