#include "error.h"
#include "http.h"
#include "confprocess.h"

int 
main( int argc, char **argv )
{
   SAI cliaddr;
   int listenfd;
   int len;
   int i;
   char config[ CONFLEN ][ CONFSIZE ];
   contenttyp* type[ TYPENUM ];

   init_config( "test.config", config, type );
  

   /* for test */
   /* 
   for(i=0; i<CONFLEN; i++ )
   {
      printf("%s\n", config[i]); 
   }

   for(i=0; type[i] != NULL; i++ )
   {
      printf("%s %s\n", type[i]->ext, type[i]->contype );
   }
   */

   /* for test */
   /*
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
   */
   /* read config here */

   
   listenfd = init( config[ HOST ], 
                    config[ PORT ], 3 );

   for( ; ; ) {
     
       handlereqsgl( listenfd, 1 , 1, config, type );

   }
   
   return EXIT_SUCCESS;
}
