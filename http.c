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
init( long address, int port )
{
   
   int listenfd;
   SAI servaddr; 

   listenfd = Socket( AF_INET, SOCK_STREAM , 0 );
   
   bzero( &servaddr, sizeof(servaddr) );
   servaddr.sin_family = AF_INET;
   servaddr.sin_addr.s_addr = htonl( address );
   servaddr.sin_port = htons( port );

   Bind( listenfd, (SA *) &servaddr, sizeof( servaddr ) ); 

   return listenfd;
}
