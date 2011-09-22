#include "error.h"

void 
err_quit(char *errmsg)
{

  if( errno != 0 )
   perror( errmsg );

  exit(EXIT_FAILURE);
}

void 
usage()
{
  fprintf(stderr, "Usage:  configfile\n"); 
}
