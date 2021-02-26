#ifndef cst_h
#define cst_h
/************************************************************************/
#define CLEANSIZE       0x00100000	/*2M*/
#define MCACHESIZE      0x40000000	/*1G*/
#define ICACHESIZE      0x00001000	/*4k*/
/************************************************************************/
#define typeTBD 1
#define typeBDD 2
/***********************************************************************/
#define TBDCSTMARK     	-1
#define TBDCSTCNT      	0x00000008
#define TBDCSTMAX     	0x00000016
/************************************************************************/
#define CSTTBDmp        0x00000001
#define CSTTBD0cm       0x00000002
#define TBDCSTDS        0x00000010
#define TBDCSTND        0x00000100
#define TBDCSTRP        0x00000101
#define TBDCSTSPRP      0x00000102
#define TBDCSTOR        0x00000200
#define TBDCSTLB        0x00000400
#define TBDCSTPtype     0x00000800
#define TBDCSTPS        0x00001000
#define TBDCSTMD        0x00002000
#define TBDCSTBDD       0x00004000
#define TBDCSTRO        0x00008000
#define TBDCSTCNLIT     0x00010000
#define TBDCSTREPLACE   0x00020000
/************************************************************************/
#define CSTmdOPT	0x00000001
#define CSTmdOPTv	0x00000002
#define CSTmcOPTa	0x00000001
#define CSTmcOPTaFWS	0x00000002
/************************************************************************/
/*
#define TBDoNEGp
#define TBDoCONa
#define TBDoDISa
#define TBDoRRa
#define TBDoRRDa
#define TBDoRRDb
/************************************************************************/
#endif
