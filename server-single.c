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
   strcpy( config[ ROOT ], "." );
   strcpy( config[ HOST ], "localhost" );
   strcpy( config[ LOG ] , "web.log" );
   strcpy( config[ RD ] , "lastrequest.txt" );
   strcpy( config[ TXT ], "text/plain" );
   strcpy( config[ HTM ], "text/html" );
   strcpy( config[ HTML ], "text/html" );
   strcpy( config[ JPG ], "image/jpeg" );
   strcpy( config[ MP3 ], "audio/mpeg" );
   strcpy( config[ WAV ], "audio/vnd.wave" );
   strcpy( config[ DEFAULT ], "text/plain" );
   /* read config here */


   listenfd = init( "localhost", "40311", 3 );

   for( ; ; ) {
     
       handlereqsgl( listenfd, 1 , 1, config );

   }

   return EXIT_SUCCESS;
}
