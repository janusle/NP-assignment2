#include "error.h"
#include "libhttp.h"

int 
main(int argc, char *argv[])
{
  char **hostandport;
  /* check argument */
  if( argc < NUMARG ){
   usage_get();
   fprintf(stderr, "missing URL or filename\n");
   return EXIT_FAILURE;
  }
 
  hostandport = gethostandport( argv[1] );
  

  wget( hostandport[0] ,hostandport[1], hostandport[2], argv[2]);
 
  return 0;
}
