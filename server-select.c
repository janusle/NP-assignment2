#include "error.h"
#include "http.h"
#include "confprocess.h"

int 
main( int argc, char **argv )
{
   SAI cliaddr;
   int len, signum; 
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

   sd = (sharedmem*) Malloc( sizeof(sharedmem) );

   /* initialize sd */
   sd->pid = getpid();
   sd->req = 0;
   sd->act = 0; 
    
   isSingle = true;
   isThreaded = false;
   ismultiplexing = true;
   shutflag = false;

   handlereqselect( config, type );
   
   return EXIT_SUCCESS;
}
