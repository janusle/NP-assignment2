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


void
handlereqsgl( int listenfd,  )
{
   int connfd, len;
   SAI cliaddr;
   
   len = sizeof( cliaddr );
   connfd = Accept( listenfd, (SA*) &cliaddr, &len );

   

}
