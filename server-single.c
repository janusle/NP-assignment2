#include "error.h"
#include "http.h"
#include "confprocess.h"

int 
main( int argc, char **argv )
{
   SAI cliaddr;
   int listenfd, len, logging, recording;
   char config[ CONFLEN ][ CONFSIZE ];
   contenttyp* type[ TYPENUM ];

   if( argc < 2 )
   {
     fprintf(stderr, "Too few arguments\n");
     fprintf(stderr, "Usage: server-single configfile\n");
     exit(EXIT_FAILURE);
   }

   /* read config here */
   init_config( argv[1], config, type );
   if( strcmp( config[LOGGING], "yes" ) == 0 )
     logging = 1;
   else
     logging = 0;

   if( strcmp( config[RECORDING], "yes" ) == 0 )
     recording = 1;
   else
     recording = 0;

   listenfd = init( config[ HOST ], config[ PORT ], 3 );

   for( ; ; ) { 
       handlereqsgl( listenfd, config, type );
   }
   
   return EXIT_SUCCESS;
}
