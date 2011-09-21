#include "error.h"
#include "http.h"

int 
main( int argc, char **argv )
{
   SAI cliaddr;
   int listenfd;
   int len;
   /* read config here */


   listenfd = init( INADDR_ANY, "11256", 3 );

   for( ; ; ) {
     
       handlereqsgl( listenfd, 1 );

   }

   return EXIT_SUCCESS;
}
