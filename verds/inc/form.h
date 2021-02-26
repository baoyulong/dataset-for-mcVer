#ifndef form_h
#define form_h
/*******************************************************************/
#include "tree.h"
/*******************************************************************/
#define MAXOPRLEN 1
/*******************************************************************/
typedef struct _bof {
	char node[MAXOPRLEN];
	struct _bof *left;
	struct _bof *right;
} bof;
/*******************************************************************/
typedef struct _tlf {
	char node[MAXOPRLEN];
	struct _bof *left;
	struct _bof *right;
} tlf;
/*******************************************************************/
#endif
