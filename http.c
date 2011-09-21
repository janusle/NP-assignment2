#include "http.h"

#define SERVERNAME "webserver"

int 
Socket( int family, int type, int protocol )
{
  int listenfd;
  
  if( ( listenfd = socket( family, type, protocol ) ) < 0 )  
  {
    err_quit( SERVERNAME ); 
  }

  return listenfd;
}


void
Bind( int sockfd, const struct sockaddr *myaddr, socklen_t addrlen )
{

  if( bind( sockfd, myaddr, addrlen ) < 0 )
  {
    err_quit( SERVERNAME );
  }

}


void
Listen( int sockfd, int backlog )
{
  if( listen( sockfd, backlog ) < 0 )
  {
    err_quit( SERVERNAME );
  }
}


int 
Accept( int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen )
{
  int connectfd;

  if( (connectfd = accept( sockfd, cliaddr, addrlen )) < 0 )
  {
    err_quit( SERVERNAME );
  }
  
  return connectfd;
}


pid_t 
Fork( void )
{
  pid_t pid;

  if( (pid=fork()) < 0 )
  {
     err_quit( SERVERNAME );
  }

  return pid;
}


void
Close( int sockfd )
{
  if( close( sockfd ) < 0 )
  {
    err_quit( SERVERNAME );
  }

}


int 
init( char *host, char *port , int backlog )
{
   
   int listenfd;
   AR hints, *res;

   bzero( &hints, sizeof(hints) );
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
 
   if( getaddrinfo( host, port, &hints, &res ) != 0 )
   {
      err_quit( PNAME );
   }

   listenfd = Socket( AF_INET, SOCK_STREAM , 0 );
   
   Bind( listenfd , res->ai_addr, res->ai_addrlen );

   Listen( listenfd, backlog );  
 
   return listenfd;
}


static char*
gettime()
{
   static char str[TIMELEN];  
   time_t timer;

   timer = time(NULL);
   strftime( str, TIMELEN, "%d/%m/%Y %H:%M:%S " , localtime(&timer) );
   
   return str;
}

static void
dolog()
{
   

}

static void
handlereq( int connfd )
{
  int n;
  char buffer[ BUFFERLEN ];

  do{
    n = read( connfd, buffer, BUFFERLEN );
  }
  while( n < 0 && errno == EINTR );
  
  if( n < 0 )
      err_quit( PNAME );
  
  write( 2, buffer, n );
}


void
handlereqsgl( int listenfd, int logged )
{
   int connfd, len, n;
   SAI cliaddr;
   char log[ LOGLEN ], buf[ TMPLEN ], addr[ INET_ADDRSTRLEN ]; 
   
   len = sizeof( cliaddr );
   connfd = Accept( listenfd, (SA*) &cliaddr, &len );

   strcpy( log, gettime() );
   inet_ntop( AF_INET, &cliaddr.sin_addr, addr, TMPLEN );

   sprintf( buf, "%s %d ", addr, ntohs( cliaddr.sin_port ) );
   strcat( log, buf );                        

   fprintf(stderr, "%s\n ", log );
   
   handlereq( connfd );

   if( logged )
   {
       
   }
   
   Close( connfd );

}

