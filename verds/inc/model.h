#ifndef model_h
#define model_h
/***********************************************************************/
typedef struct _model { int n; int m; int l; int *mstring; } model;
/***********************************************************************/
typedef struct _mstruct {
        int **Clause;
        int *CL; 
        int *LiteralMatrix;
        int *LS;
        int *LL;
} mstruct;
/***********************************************************************/
typedef struct _mdata {
        char *InactiveMatrix;
        int *InactiveList;
        int *Literals;
        int *Current;
} mdata;
/***********************************************************************/
#endif
