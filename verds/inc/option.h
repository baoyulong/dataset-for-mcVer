#ifndef option_h
#define option_h
/********************************************************************/
typedef struct _OPTcex { 
	char var[64]; int varlow; int varhigh;
} OPTcex;
/********************************************************************/
typedef struct _OPTc { 
	char *filec; char *filesp; char *filefsp; char vv;
} OPTc;
/********************************************************************/
typedef struct _OPTb { 
	int  lbound; int  bound; char *qbfsolver; char *satsolver;
	char xbv;
} OPTb;
/********************************************************************/
typedef struct _OPT { 
	char bs; /*b,f,bs,QBF,SAT*/
	char ck; int ag; char Om; char Ovord; int Oalg; char string[16];
	OPTc opc; OPTb opb; OPTcex opcex;
	char *symfile;
} OPT;
/********************************************************************/
#define op001t "Prints the command line help"
#define op002t "Prints the list of the properties in the model"
#define op003t "Checks the i-th property (default: -ck 1)"
/********************************************************************/
#define help00() \
fprintf(stdout,"-help (-h) (-?)   : %s\n",op001t);\
fprintf(stdout,"-p                : %s\n",op002t);\
fprintf(stdout,"-ck i             : %s\n",op003t);\
/********************************************************************/
#define op101t "Checks whether it is a total transition relation"
#define op102t "Checks whether it is a deadlock freee transition system"
#define op103t "Uses the variable ordering from the file varfile"
/********************************************************************/
#define help10() \
fprintf(stdout,"-ck tt            : %s\n",op101t);\
fprintf(stdout,"-ck df            : %s\n",op102t);\
fprintf(stdout,"-v varfile        : %s\n",op103t);\
/********************************************************************/
#define op201t "Uses qbf-based/sat-based bounded correctness checking"
#define op202t "Uses qbf-based bounded correctness checking"
#define op203t "Uses sat-based bounded correctness checking (ACTL formulas)"
#define op204t "Sets the qbfsolver variable"
#define op205t "Sets the satsolver variable"
/********************************************************************/
#define help20() \
fprintf(stdout,"-bs               : %s\n",op201t);\
fprintf(stdout,"-QBF              : %s\n",op202t);\
fprintf(stdout,"-SAT              : %s\n",op203t);\
fprintf(stdout,"-qbfsolver solver : %s\n",op204t);\
fprintf(stdout,"-satsolver solver : %s\n",op205t);\
/********************************************************************/
#define op211t "Sets the bound variable"
#define op212t "Applies verification only"
/********************************************************************/
#define help21() \
fprintf(stdout,"-bound k          : %s\n",op211t);\
fprintf(stdout,"-verify           : %s\n",op212t);\
/********************************************************************/
#define op301t "Enables optimization"
#define op302t "Ignores Fairness Specifications"
#define op303t "Enables cone of influence reduction"
/********************************************************************/
#define help30() \
fprintf(stdout,"-O                : %s\n",op301t);\
fprintf(stdout,"-ignore fairness  : %s\n",op302t);\
/********************************************************************/
#define help() help00() help10() help20() help21() help30() /*help90()
/********************************************************************/
#define help90() \
fprintf(stdout,"-------------------------\n");\
fprintf(stdout,"-O                : Om+Oa\n");\
fprintf(stdout,"-Om               : model c\n");\
fprintf(stdout,"-Ov               : model var\n");\
fprintf(stdout,"-Oa               : alg forward\n");\
fprintf(stdout,"-OaFWS            : alg forward split\n");\
fprintf(stdout,"-Overify          : alg verify\n");\
/********************************************************************/
#endif
