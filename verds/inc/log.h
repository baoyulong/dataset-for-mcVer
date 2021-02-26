#ifndef log_h
#define log_h
/************************************************************************/
#define error(s)  	{fprintf(stderr,"%s error\n",s); exit(0);}
/***********************************************************************/
#define iinfos(s)       {}
#define iinfoi(s)       {}
#define iinfof(s)       {}
/***********************************************************************/
#define infos(s)        {fprintf(stdout,"%s\n",s);}
#define infoi(s)        {fprintf(stdout,"%i\n",s);}
#define infof(s)        {fprintf(stdout,"%f\n",s);}
/***********************************************************************/
#define infot() \
        printf("TIME %f,%i\n",(float)utiltime(1),(int)difftime(time(&t1),t0))\
/************************************************************************/
extern FILE *logi;
extern FILE *logw;
/***********************************************************************/
extern time_t t0,t1;
/***********************************************************************/
#define loginit() {\
	if (logi==NULL) logi=fopen("logtbdi.tmp","w"); \
	if (logw==NULL) logw=fopen("logtbdw.tmp","w"); \
}\
/***********************************************************************/
#define loginfo(s,i)   	{fprintf(logi,"%s:%i\n",s,i);}
#define logwarning(s,i) {fprintf(logw,"%s:%i\n",s,i);}
#define logtime(s) 	{fprintf(logi,"%s:%i\n",s,(int)difftime(time(&t1),t0));}
#define logexit(s)  	{loginfo(s,0);fclose(logi);fclose(logw);exit(0);}
/***********************************************************************/
#endif
