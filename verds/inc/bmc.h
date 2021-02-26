#ifndef bmc_h
#define bmc_h
/***********************************************************************/
#include "tree.h"
#include "bp2q.h"
#include "bp2s.h"
#include "option.h"
#include "tlf.h"
/***********************************************************************/
typedef struct _vinfo {
        char name[MAXVARLEN];
        int low;
        int high;
        int value;
        int max;
} vinfo;
/***********************************************************************/
#include "wfbmc.h"
#endif
