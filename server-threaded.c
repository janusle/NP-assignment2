#include "error.h"
#include "http.h"
#include "confprocess.h"

#define SERVERNAME " server-threaded\n"

int 
main( int argc, char **argv )
{
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
 
   listenfd = init( config[ HOST ], config[ PORT ], 3 );
   
   isSingle = true; 
   isThreaded = true;
   ismultiplexing = false;

   pthread_mutex_init(&act_mutex, NULL);

   /* set shared memroy */
   sd = (sharedmem*)Malloc( sizeof(sharedmem) );
   sd->pid = getpid();
   sd->act = 0;
   sd->req = 0;

   /* logging */
   if( strcmp( config[LOGGING], "yes" ) == 0 ){
      dolog_withtime( config[LOG], SERVERNAME );
      dolog_withtime( config[LOG], " initialization complete\n");
   }

   handlereqthread( listenfd, config, type );
   
   return EXIT_SUCCESS;
}
