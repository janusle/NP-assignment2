#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<sys/errno.h>

extern int errno;

/* print error msg to stderr, if u equals true then print usage as well*/
void err_quit(char *errmsg);

void usage_get();

/* print usage to stderr */
void usage();
