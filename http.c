#include "http.h"

#define SERVERNAME "webserver"

void *
Malloc( size_t size )
{
  void* space;

  space = malloc( size );

  if( space == NULL )
    err_quit("PNAME");

  return space;
}


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
getdatetime()
{
   static char str[TIMELEN];  
   time_t timer;

   timer = time(NULL);
   strftime( str, TIMELEN, "%d/%m/%Y %H:%M:%S" , localtime(&timer) );
   
   return str;
}


static char*
gettime()
{
   static char str[TIMELEN];  
   time_t timer;

   timer = time(NULL);
   strftime( str, TIMELEN, "%H:%M:%S" , localtime(&timer) );
   
   return str;
}



static void
upperline( char* st, int len )
{
  int i;
  for( i=0; i<len; i++ )
  {
    st[i] = toupper(st[i]);
  }

}



static void
lowerline( char* st, int len )
{
  int i;
  for( i=0; i<len; i++ )
  {
    st[i] = tolower(st[i]);
  }

}



static int
parseheader( char* buf, int size , char info[][TMPLEN] )
{
   char line[ BUFFERLEN ], *tmp ;
   int i, j, len;

   /* if buf size is 0 , then return code 400 bad request */
   if( buf == NULL || size == 0 )
     return 400; 

   /* get the first line */
   tmp = strtok( buf, "\r\n" );

   if( tmp == NULL )
     return 400;

   strcpy( line, tmp );
   len = strlen( line ); 

   /* get http method */
   tmp = (char*)Malloc( METHODLEN * sizeof( char ) ); 
      
   for( i=0, j=0; i<len && !isspace(line[i]); i++, j++ )
     tmp[j] = line[i];
   tmp[j] = '\0';

   upperline( tmp, strlen(tmp) );
   /* request does not be implemented */ 
   if( strncmp( tmp, "POST", strlen("POST") ) == 0 )
     return 501;
  
   if( strncmp( tmp, "HEAD", strlen("HEAD") ) == 0 )
     return 501;

   if( strncmp( tmp, "TRACE", strlen("TRACE") ) == 0 )
     return 501;
  
   if( strncmp( tmp, "PUT", strlen("PUT") ) == 0 )
     return 501;

   if( strncmp( tmp, "DELETE", strlen("DELETE") ) == 0 )
     return 501;
  
   if( strncmp( tmp, "OPTIONS", strlen("OPTIONS") ) == 0 )
     return 501;
   
   if( strncmp( tmp, "CONNECT", strlen("CONNECT") ) == 0 )
     return 501;

   if( strncmp( tmp, "PATCH", strlen("PATCH") ) == 0 )
     return 501;

   /* request cannot be regnoized , bad request returns */
   if( strncmp( tmp, "GET", strlen("PATCH") ) != 0 )
     return 400;
 
   
   /* skip 'method' and space */
   for( i=GETLEN; i<len && isspace( line[i] ); i++ )
   ; 

   if( i >= len )
     return 400;

   for(j=0; i<len && !isspace( line[i] ); i++, j++)
   {
      info[URL][j] = line[i]; 
   }
   info[URL][j] = '\0';

   /* skip space */
   for( ; i<len && isspace( line[i] ); i++ )
   ; 
  
   if( i >= len )
     return 400;

   for(j=0; i<len && !isspace( line[i] ); i++, j++ )
   {
     info[VERSION][j] = line[i];
   }
   info[VERSION][j] = '\0';
  
   upperline( info[VERSION], strlen(info[VERSION]) );
   /* version is invalid */
   if( strcmp( info[VERSION], "HTTP/1.0" ) != 0 &&
       strcmp( info[VERSION], "HTTP/1.1" ) != 0 )
     return 400; 
   
   return 0;
}


static int
handlereq( int connfd , int logged, int recording, 
           char config[CONFLEN][CONFSIZE] , char* log )
{
  int n, code;
  char buffer[ BUFFERLEN ], info[INFOLEN][TMPLEN] ;
  

  /* read request */
  do{
    n = read( connfd, buffer, BUFFERLEN );
  }
  while( n < 0 && errno == EINTR );
  
  if( n < 0 )
      return false;
 
  buffer[n] = '\0';
  code = parseheader( buffer, n , info );

  printf("%s %s\n\n\n", info[URL], info[VERSION] );

  write( 2, buffer, n );
}


/* handle request (single process version) */
void
handlereqsgl( int listenfd, int logged, int recording, 
              char config[][CONFSIZE])
{
   int connfd, len, n, err;
   SAI cliaddr;
   char log[ LOGLEN ], cliinfo[ TMPLEN ], addr[ INET_ADDRSTRLEN ], acptime[ TIMELEN ] ,
        clstime[ TIMELEN ], repinfo[ INFOSIZ ]; 

   err = 0;
   len = sizeof( cliaddr );
   connfd = Accept( listenfd, (SA*) &cliaddr, &len );

   /* record accepted time */
   strcpy( acptime, getdatetime() );
 
   /* handle request , if fail to do , return */
   if ( handlereq( connfd , logged, recording, config, log ) == false )
     err = errno; 

   Close( connfd );

   /* record closed time */
   strcpy( clstime, gettime() );
   /* record client address and port */
   inet_ntop( AF_INET, &cliaddr.sin_addr, addr, TMPLEN );
   sprintf( cliinfo, "%s %d ", addr, ntohs( cliaddr.sin_port ) );

   /* generate log */ 
   sprintf( log, "%s %s %s %d", acptime, clstime, cliinfo, err ); 

   /* for test */
   fprintf(stderr, "%s\n ", log );

   if( logged )
   {
       
   }
   
}

