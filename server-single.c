#include "error.h"
#include "http.h"

int 
main( int argc, char **argv )
{
   SAI cliaddr;
   int listenfd;
   int len;
   char config[ CONFLEN ][ CONFSIZE ];

   /* for test */
   strcpy( config[ ROOT ], "htdocs" );
   strcpy( config[ HOST ], "localhost" );
   strcpy( config[ LOG ] , "web.log" );
   strcpy( config[ RD ] , "lastrequest.txt" );

   /* read config here */


   listenfd = init( INADDR_ANY, "11256", 3 );

   for( ; ; ) {
     
       handlereqsgl( listenfd, 1 , 1, config );

   }

   return EXIT_SUCCESS;
}
