/****************************************/
/*
#define KW28 	"MODULE"
#define KW38 	"MODULE"
#define KW29 	"VVM"
#define KW39 	"MAIN"
/****************************************/
#define KW101	"TRUE"
#define KW102	"FALSE"
#define KW103 	"VAR"
#define KW104 	"INIT"
#define KW105	"TRANS"
#define KW106	"SPEC"
#define KW107 	"PROC"
#define KW108 	"FAIRNESS"
#define KW109 	"INV"
#define KW111 	"ASSUMPTION"
#define KW112 	"GUARANTEE"
#define OP01 	"!="
#define OP02 	"<="
#define OP03 	">="
#define OP04 	":="
#define OP05 	"RETURN"
/****************************************/
#define KW201	"1=1"
#define KW202	"1=0"
#define KW203 	"   "
#define KW204 	"#"
#define KW205	"#"
#define KW206	"#"
#define KW207 	"#"
#define KW208 	"#1=0;"
#define KW209 	"#1=0;"
#define KW211 	"#1=1;"
#define KW212 	"#1=1;"
#define OP11 	"B"
#define OP12 	"{"
#define OP13 	"}"
#define OP14 	"="
#define OP15 	"ret_=1"
/****************************************/
#define kw(a,b) strcpy(cleft[cc],a); strcpy(cright[cc],b); cc++;
/****************************************/
#define kwall() \
	clen[cc]=4; kw(KW101,KW201); \
	clen[cc]=5; kw(KW102,KW202); \
	clen[cc]=3; kw(KW103,KW203); \
	clen[cc]=4; kw(KW104,KW204); \
	clen[cc]=5; kw(KW105,KW205); \
	clen[cc]=4; kw(KW106,KW206); \
	clen[cc]=4; kw(KW107,KW207); \
	clen[cc]=8; kw(KW108,KW208); \
	clen[cc]=3; kw(KW109,KW209); \
	clen[cc]=10; kw(KW111,KW211); \
	clen[cc]=9; kw(KW112,KW212); \
	clen[cc]=2; kw(OP01,OP11); \
	clen[cc]=2; kw(OP02,OP12); \
	clen[cc]=2; kw(OP03,OP13); \
	clen[cc]=2; kw(OP04,OP14); \
	clen[cc]=6; kw(OP05,OP15); \
	clen[cc]=3; kw("'A'","65"); \
	clen[cc]=3; kw("'B'","66"); \
	clen[cc]=3; kw("'C'","67"); \
	clen[cc]=3; kw("'D'","68"); \
	clen[cc]=3; kw("'E'","69"); \
	clen[cc]=3; kw("'F'","70"); \
	clen[cc]=3; kw("'G'","71"); \
	clen[cc]=3; kw("'H'","72"); \
	clen[cc]=3; kw("'I'","73"); \
	clen[cc]=3; kw("'J'","74"); \
	clen[cc]=3; kw("'K'","75"); \
	clen[cc]=3; kw("'L'","76"); \
	clen[cc]=3; kw("'M'","77"); \
	clen[cc]=3; kw("'N'","78"); \
	clen[cc]=3; kw("'O'","79"); \
	clen[cc]=3; kw("'P'","80"); \
	clen[cc]=3; kw("'Q'","81"); \
	clen[cc]=3; kw("'R'","82"); \
	clen[cc]=3; kw("'S'","83"); \
	clen[cc]=3; kw("'T'","84"); \
	clen[cc]=3; kw("'U'","85"); \
	clen[cc]=3; kw("'V'","86"); \
	clen[cc]=3; kw("'W'","87"); \
	clen[cc]=3; kw("'X'","88"); \
	clen[cc]=3; kw("'Y'","89"); \
	clen[cc]=3; kw("'Z'","90"); \
	clen[cc]=3; kw("'a'","97"); \
	clen[cc]=3; kw("'b'","98"); \
	clen[cc]=3; kw("'c'","99"); \
	clen[cc]=3; kw("'d'","100"); \
	clen[cc]=3; kw("'e'","101"); \
	clen[cc]=3; kw("'f'","102"); \
	clen[cc]=3; kw("'g'","103"); \
	clen[cc]=3; kw("'h'","104"); \
	clen[cc]=3; kw("'i'","105"); \
	clen[cc]=3; kw("'j'","106"); \
	clen[cc]=3; kw("'k'","107"); \
	clen[cc]=3; kw("'l'","108"); \
	clen[cc]=3; kw("'m'","109"); \
	clen[cc]=3; kw("'n'","110"); \
	clen[cc]=3; kw("'o'","111"); \
	clen[cc]=3; kw("'p'","112"); \
	clen[cc]=3; kw("'q'","113"); \
	clen[cc]=3; kw("'r'","114"); \
	clen[cc]=3; kw("'s'","115"); \
	clen[cc]=3; kw("'t'","116"); \
	clen[cc]=3; kw("'u'","117"); \
	clen[cc]=3; kw("'v'","118"); \
	clen[cc]=3; kw("'w'","119"); \
	clen[cc]=3; kw("'x'","120"); \
	clen[cc]=3; kw("'y'","121"); \
	clen[cc]=3; kw("'z'","122"); \
	clen[cc]=3; kw("'0'","48"); \
	clen[cc]=3; kw("'1'","49"); \
	clen[cc]=3; kw("'2'","50"); \
	clen[cc]=3; kw("'3'","51"); \
	clen[cc]=3; kw("'4'","52"); \
	clen[cc]=3; kw("'5'","53"); \
	clen[cc]=3; kw("'6'","54"); \
	clen[cc]=3; kw("'7'","55"); \
	clen[cc]=3; kw("'8'","56"); \
	clen[cc]=3; kw("'9'","57"); \
	clen[cc]=3; kw("':'","58"); \
	clen[cc]=3; kw("';'","59"); \
	clen[cc]=3; kw("' '","32"); \
	clen[cc]=4; kw("'\\n'","10"); \
/****************************************/