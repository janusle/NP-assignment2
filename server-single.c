#include "error.h"
#include "http.h"

int 
main( int argc, char **argv )
{
   int listenfd;

   /* read config here */


   listenfd = init( INADDR_ANY, 9999 );
   


   return EXIT_SUCCESS;
}
