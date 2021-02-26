#ifndef tree_h
#define tree_h
/***********************************************************************/
#include "basics.h"
#include "treetype.h"
/***********************************************************************/
#define debug()
/***********************************************************************/
#define tmp0file "logvbs.tmp"
#define tmp1file "logsat.tmp"
#define tmp2file "logsat.cex"
#define vds0file "logvds.tmp"
#define vds2file "logvds.trs"
/***********************************************************************/
#define TRUE			'^'
#define FALSE			'_'
#define TRUEs			"^"
#define FALSEs			"_"
/***********************************************************************/
#define CHANW   		'@'
#define CHANWs  		"@"
#define CHANR   		'?'
#define CHANRs  		"?"
/***********************************************************************/
#define BOR			'M'
#define BORs			"M"
#define DAND			'N'
#define DANDs			"N"
/***********************************************************************/
#define PIDs			"p_id_"
#define PIDkw			"pid"
#define RETs			"ret_x_"
#define RETkw			"ret_"
/***********************************************************************/
#define treestr(a)	treecompose(a,NULL,NULL)
/***********************************************************************/
#define neg(a)          treecompose("!",NULL,a)
#define lor(a,b)        treecompose("|",a,b)
#define land(a,b)       treecompose("&",a,b)
/*******************************************************************/
#define xor(a,b)        treecompose("&", \
                        treecompose("|",treecopy(a),b), \
                        treecompose("|",neg(a),neg(treecopy(b))))
#define xor_dnf(a,b)    treecompose("|", \
                        treecompose("&",treecopy(a),neg(b)), \
                        treecompose("&",neg(a),treecopy(b))) \
/*******************************************************************/
#define leq(a,b)        treecompose("&", \
                        treecompose("|",neg(treecopy(a)),treecopy(b)), \
                        treecompose("|",a,neg(b)))
#define leq_dnf(a,b)    treecompose("|", \
                        treecompose("&",treecopy(a),treecopy(b)), \
                        treecompose("&",neg(a),neg(b))) \
/***********************************************************************/
typedef struct _module { 
	char type; /*MP*/
	tree *parameters;
	tree *prog;
	struct _module *next; 
} module;
/***********************************************************************/
typedef struct _workspace {
        tree *prog;
	tree *spec;
        tree *varconstr;
        tree *deadlockfreecond;
        int booln;
        int puretrans;
	tree *fness;
} workspace;
/***********************************************************************/
typedef struct _dbase {
        char *file;
        char *string;
	module *tsm;
	tree *ts;
	tree *processes;
	int property;
        char propertytype;
} dbase;
/***********************************************************************/
#include "prg.h"
/***********************************************************************/
typedef struct _ggbase {
        char *file;
        char *string;
	module *mm;
	prg *pr;
	prg *pr1;
	prg *prb;
} ggbase;
/***********************************************************************/
#include "wftree.h"
#endif
