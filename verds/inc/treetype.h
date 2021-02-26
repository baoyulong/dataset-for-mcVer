#ifndef treetype_h
#define treetype_h
/***********************************************************************/
#define MAXSTRING		0x04000000
#define MAXVARLEN		0x00000040
/***********************************************************************/
#define AND                     '&'
#define OR                      '|'
#define NEG                     '!'
/***********************************************************************/
typedef struct _tree { 
	char node[MAXVARLEN];
	struct _tree *left; 
	struct _tree *right; 
} tree;
/***********************************************************************/
#endif
