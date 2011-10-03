#include "error.h"
#include "http.h"
#include "confprocess.h"

#define SERVERNAME " server-single start\n"

int 
main( int argc, char **argv )
{
   int listenfd; 
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
   sd->act = 1; /* It's a single process program */
    
   isSingle = true;
   isThreaded = false;
   ismultiplexing = false;
  
   /* logging */
   if( strcmp( config[LOGGING], "yes" )==0 ){
     dolog_withtime( config[LOG], SERVERNAME );
     dolog_withtime( config[LOG], " initialization complete\n" );
   }

   for( ; ; ) { 
       handlereqsgl( listenfd, config, type );
   }
   
   return EXIT_SUCCESS;
}
