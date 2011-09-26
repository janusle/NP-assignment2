#include "error.h"
#include "http.h"
#include "confprocess.h"

int 
main( int argc, char **argv )
{
   SAI cliaddr;
   int listenfd, len, signum; 
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

   signal( atoi(config[SDSIG]) , sig_shutdown );

   listenfd = init( config[ HOST ], config[ PORT ], 3 );

   pidnum = 0;

   for( ; ; ) { 
       handlereqsgl( listenfd, config, type );
   }
   
   return EXIT_SUCCESS;
}
