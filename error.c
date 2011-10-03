#include "error.h"

void 
err_quit(char *errmsg)
{

  if( errno != 0 )
   perror( errmsg );

  exit(EXIT_FAILURE);
}

void
usage_get()
{
  fprintf(stderr, "Usage: gethttp URL filename\n");
}

void 
usage()
{
  fprintf(stderr, "Usage: server configfile\n"); 
}
