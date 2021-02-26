#ifndef main_h
#define main_h
/***********************************************************************/
#include "f2p.h"
#include "p2bp.h"
#include "bp2t.h"
#include "dmc.h"
#include "option.h"
#include "cex.h"
#include "mod.h"
/***********************************************************************/
extern int PAR_tm;
extern time_t t0,t1;
/***********************************************************************/
#define logvds(s) 	fprintf(vdslog,"%s\n",s)
/***********************************************************************/
#define tout		(PAR_tm&&(int)difftime(time(&t1),t0)>PAR_tm)
/***********************************************************************/
#include "wfmain.h"
#endif
