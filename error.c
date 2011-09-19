#include "error.h"

void 
err_quit(char *errmsg)
{

  if( errno != 0 )
   perror( errmsg );

  exit(-1);
}

void 
usage()
{
  fprintf(stderr, "Usage:  configfile\n"); 
}
