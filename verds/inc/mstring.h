#ifndef mstring_h
#define mstring_h
/***********************************************************************/
#define stringnew(b,n) \
	{b=(char *)malloc((n+1)*sizeof(char));} \
/***********************************************************************/
#define stringcpy(b,a) if (a!=NULL) { \
	b=(char *)malloc((strlen(a)+1)*sizeof(char)); strcpy(b,a);} \
/***********************************************************************/
#define stringpar(b,a) if (a!=NULL) { \
	b=(char *)malloc((strlen(a)+7)*sizeof(char)); \
	sprintf(b,"(%s)",a);} \
/***********************************************************************/
#define stringneg(b,a) if (a!=NULL) { \
	b=(char *)malloc((strlen(a)+7)*sizeof(char)); \
	sprintf(b,"!(%s)",a);} \
/***********************************************************************/
#define stringcat(b,a1,a2) if (a1!=NULL&&a2!=NULL) { \
	b=(char *)malloc((strlen(a1)+strlen(a2)+6)*sizeof(char)); \
	sprintf(b,"%s%s",a1,a2);} \
/***********************************************************************/
#define stringconj(b,a1,a2) if (a1!=NULL&&a2!=NULL) { \
	b=(char *)malloc((strlen(a1)+strlen(a2)+6)*sizeof(char)); \
	sprintf(b,"(%s)%c(%s)",a1,'&',a2); }\
/***********************************************************************/
#define stringdisj(b,a1,a2) if (a1!=NULL&&a2!=NULL) { \
	b=(char *)malloc((strlen(a1)+strlen(a2)+6)*sizeof(char)); \
	sprintf(b,"(%s)%c(%s)",a1,'|',a2); }\
/***********************************************************************/
#define stringimp(b,a1,a2) if (a1!=NULL&&a2!=NULL) { \
	b=(char *)malloc((strlen(a1)+strlen(a2)+7)*sizeof(char)); \
	sprintf(b,"!(%s)|(%s)",a1,a2);} \
/***********************************************************************/
#define stringdisj(b,a1,a2) if (a1!=NULL&&a2!=NULL) { \
	b=(char *)malloc((strlen(a1)+strlen(a2)+6)*sizeof(char)); \
	sprintf(b,"(%s)%c(%s)",a1,'|',a2); }\
/***********************************************************************/
#endif

