#ifndef tbd_h
#define tbd_h
/***********************************************************************/
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
/***********************************************************************/
#include "typetbd.h"
#include "cst.h"
/***********************************************************************/
/*
int TPavar(char **v1,int n);
int **TPatrs(tree *t);
TBD **TPafair(tree *t);
TBD *TPainit(tree *t);
TBD *TPfml(tree *p);
/***********************************************************************/
TBD *TBDbw(TBD *w);
TBD *TBDfw(TBD *w);
TBD *TBDcn(TBD *s,TBD *t);
TBD *TBDds(TBD *s,TBD *t);
TBD *TBDng(TBD *s);
TBD *TBDap(int i);
/***********************************************************************/
#define TBDrc(w) TBDmmmarkset(w,TBDCSTMARK)
/***********************************************************************/
#endif 
