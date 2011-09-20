#include "error.h"
#include "http.h"

int 
main( int argc, char **argv )
{
   SAI cliaddr;
   int listenfd;
   int len;
   /* read config here */


   listenfd = init( INADDR_ANY, "9999", 33 );

   for( ; ; ) {
     
       handlereqsgl( listenfd );

   }

   return EXIT_SUCCESS;
}
