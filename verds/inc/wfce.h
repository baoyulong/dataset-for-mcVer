vinfo *getvarinfo(char *fn);
int *getstpinfo(char *fn);
char **gettrsinfo(char *fn);
void file2states(char *fn,vinfo *vv,int *stp,char **trs);
void vinfowrite(vinfo *vv,FILE *ff);
int main(int argc, char *argv[]);
