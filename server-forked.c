#include "error.h"
#include "http.h"
#include "confprocess.h"

int 
main( int argc, char **argv )
{
   SAI cliaddr;
   int listenfd, len, i; 
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

   signal( atoi( config[SDSIG]), sig_shutdown );

   /* init childs */
   /*
   for( i=0; i<MAXCLIENT; i++ ){
     children.pids[i] = 0; 
   }
   children.count = 0;
   */
   pidnum = 0; 

   listenfd = init( config[ HOST ], config[ PORT ], 3 );

   handlereqfork( listenfd, config, type );
   
   return EXIT_SUCCESS;
}
