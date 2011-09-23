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


static char*
getrptime()
{
  static char str[TIMELEN];
  int tail;
  time_t timer;

  timer = time(NULL);
  strcpy( str, asctime( localtime(&timer)) ); 
 
  /* cut the newline in the end of string */
  tail = strlen( str );
  str[tail-1] = '\0';

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
parseheader( char* buf, int size , char config[][CONFSIZE],
             char info[][TMPLEN] )
{
   char line[ BUFFERLEN ], *tmp ;
   int i, j, len;

   /* if buf size is 0 , then return code 400 bad request */
   if( buf == NULL || size == 0 )
   {
     strcpy( info[STATUS], "400" );
     return 400; 
   }
   /* get the first line */
   tmp = strtok( buf, "\r\n" );

   if( tmp == NULL )
   {
     strcpy( info[STATUS], "400" );
     return 400;
   }

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
   {
     strcpy( info[STATUS], "501" );
     return 501;
   }

   if( strncmp( tmp, "HEAD", strlen("HEAD") ) == 0 )
   {
     strcpy( info[STATUS], "501" );
     return 501;
   }

   if( strncmp( tmp, "TRACE", strlen("TRACE") ) == 0 ) 
   {
     strcpy( info[STATUS], "501" );
     return 501;
   }

   if( strncmp( tmp, "PUT", strlen("PUT") ) == 0 )
   {
     strcpy( info[STATUS], "501" );
     return 501;
   }

   if( strncmp( tmp, "DELETE", strlen("DELETE") ) == 0 )
   {
     strcpy( info[STATUS], "501" );
     return 501;
   }
  
   if( strncmp( tmp, "OPTIONS", strlen("OPTIONS") ) == 0 )
   {
     strcpy( info[STATUS], "501" );
     return 501;
   }
   
   if( strncmp( tmp, "CONNECT", strlen("CONNECT") ) == 0 )
   {
     strcpy( info[STATUS], "501" );
     return 501;
   }

   if( strncmp( tmp, "PATCH", strlen("PATCH") ) == 0 )
   {
     strcpy( info[STATUS], "501" );
     return 501;
   }

   /* request cannot be regnoized , bad request returns */
   if( strncmp( tmp, "GET", strlen("PATCH") ) != 0 )
   {
     strcpy( info[STATUS], "400" );
     return 400; 
   }

   free(tmp);

   /* skip 'method' and space */
   for( i=GETLEN; i<len && isspace( line[i] ); i++ )
   ; 

   if( i >= len )
   {
     strcpy( info[STATUS], "400" );
     return 400; 
   }

   for(j=0; i<len && !isspace( line[i] ); i++, j++)
   {
      info[URL][j] = line[i]; 
   }
   info[URL][j] = '\0';

   /* skip space */
   for( ; i<len && isspace( line[i] ); i++ )
   ; 
  
   if( i >= len )
   {
     strcpy( info[STATUS], "400" );
     return 400; 
   }

   for(j=0; i<len && !isspace( line[i] ); i++, j++ )
   {
     info[VERSION][j] = line[i];
   }
   info[VERSION][j] = '\0';
  
   upperline( info[VERSION], strlen(info[VERSION]) );
  
   /* version is invalid */
   if( strcmp( info[VERSION], "HTTP/1.0" ) != 0 &&
       strcmp( info[VERSION], "HTTP/1.1" ) != 0 )
   {
     strcpy( info[STATUS], "400" );
     return 400; 
   }

   tmp = strtok(NULL, "\r\n");
   while( tmp != NULL )
   {
       upperline( tmp, strlen(tmp) );
       if( strstr( tmp, "HOST:" ) != NULL )
         break; 

       tmp = strtok(NULL, "\r\n");
   }


   if( tmp != NULL )
   {
       /* compare host */
       upperline( config[HOST], strlen(config[HOST]) ); 
       if( strstr( tmp, config[HOST] ) == NULL )
       { 
         strcpy( info[STATUS], "305" );
         return 305; 
       }
   }

   /* HTTP/1.1 must has Host header */
   if( tmp == NULL && 
       strcmp( info[VERSION], "HTTP/1.1" ) == 0 )
   {
       strcpy( info[STATUS], "400" );
       return 400; 
   }
 
   return 0;
}


static long 
getfilesize( int fd )
{
   size_t size;
   struct stat buf;

   if ( fstat( fd, &buf ) < 0 )
     size = -1;

   size = buf.st_size; 
  
   return size;
}


/* get content type */
static char*
gettype( char* url, char config[][CONFSIZE] )
{
   int len, i;
   char tmp[ TMPLEN ];

   len = strlen( url );
   
   for( i=len-1; i>=0 && url[i] != '.'; i-- )
       ;

   /* No extension found */
   if( i<0 )
     return config[DEFAULT];

   strcpy( tmp, &url[i+1] );
   upperline( tmp , strlen(tmp) );
   
   if( strcmp( tmp, "TXT" ) == 0 ){
      return config[TXT];
   }
   else if( strcmp( tmp, "HTM" ) == 0 ){
      return config[HTM];
   }
   else if( strcmp( tmp, "HTML" ) == 0 ){
      return config[HTML];
   }
   else if( strcmp( tmp, "JPG" ) == 0 ){
      return config[JPG];
   }
   else if( strcmp( tmp, "MP3" ) == 0 ){
      return config[MP3];
   }
   else if( strcmp( tmp, "WAV" ) == 0){
      return config[WAV];
   }
   else
   {
      /* default type */
      return config[DEFAULT]; 
   }

}


/* generate header */
static char*
genheader( int code, size_t length, char* type ) 
{
    static char header[ HEADERSIZ ]; 
    char tmp[ TMPLEN ];

    if( code == 404 )
    {
       strcpy( header, "HTTP/1.1 404 Not Found\r\n" );  
    }

    if( code == 403 )
    {
       strcpy( header, "HTTP/1.1 403 Forbidden\r\n" );  
    }

    if( code == 400 )
    {
       strcpy( header, "HTTP/1.1 400 Bad Request\r\n" );  
    }

    if( code == 501 )
    {
       strcpy( header, "HTTP/1.1 501 Not Implemented\r\n" );  
    }

    if( code == 200 )
    {
       strcpy( header, "HTTP/1.1 200 OK\r\n" );  
    }

    strcat( header, "Connection: close\r\n" );


    sprintf( tmp, "Date: %s\r\n", getrptime() );
    strcat( header, tmp );
    
    sprintf( tmp, "Content-Type: %s\r\n", type );
    strcat( header, tmp );

    sprintf( tmp, "Content-Length: %ld\r\n", length );
    strcat( header, tmp);

    strcat( header, "Server: myserver\r\n" );
    
    strcat( header, "\r\n" );
    

    /* for test */
    /*fprintf(stdout, "%s\n", header);*/
    
    return header; 
}


static int 
response( int connfd ,int code, int fp , char info[][TMPLEN], 
          char* type )
{
   int n;
   size_t length = 0, left;
   char buffer[ FILEBUF ];
   char *header; 
   if( code == 404 ) {     
     fp = open( "404.html", O_RDONLY );
     if( fp < 0 )
       return false;

   }

   if( code == 403 )
   {     
     fp = open( "403.html", O_RDONLY );
     if( fp < 0 )
       return false;

   }

   if( code == 400 )
   {     
     fp = open( "400.html", O_RDONLY );
     if( fp < 0 )
       return false;

   }
 
   if( code == 501 )
   {     
     fp = open( "501.html", O_RDONLY );
     if( fp < 0 )
       return false;

   }


   /* get size of file */
   length = getfilesize( fp );
   if( length < 0 )
     return false;

   /* record content-length */
   sprintf( info[CONTENTLEN], "%ld", length );

   /* send header */
   header = genheader( code, length, type );  

   FILE* fd;
   write( connfd, header, strlen(header) ); 
 
   /* send content */
   left = length;
   while( left > 0 ){
      n = read( fp, buffer, FILEBUF );
      if( n == 0 )
        break;
      write( connfd, buffer, n );
    //  write( 2, buffer, n);
      left -= n;    
   }

   close( fp );
}


/* handle request (general version) */
static int
handlereq( int connfd , int logged, int recording, 
           char config[CONFLEN][CONFSIZE] , char info[][TMPLEN] )
{
  int n, code, result;
  char buffer[ BUFFERLEN ], address[ TMPLEN ];
  char *header;
  int fp;  

  /* read request */
  do{
    n = read( connfd, buffer, BUFFERLEN );
  }
  while( n < 0 && errno == EINTR );
  
  if( n < 0 )
      return false;
 
  buffer[n] = '\0';
  /* parse header */
  code = parseheader( buffer, n , config, info );

  if( code == 0 )
  {

     strcpy( address, config[ROOT] );
     strcat( address, info[URL] );
     fp = open( address, O_RDONLY );
           
     /* 403 or 404 */
     if( fp < 0 )
     {
        if( errno == EACCES )
        {
          strcpy(info[STATUS], "403");
          result = response( connfd , 403, INVALID, info, gettype(info[URL], config) );      
        }
        else
        {
          strcpy(info[STATUS], "404");
          result = response( connfd, 404, INVALID, info, gettype(info[URL], config) );
        }
     }
     else
     {
       /* 200 OK */
       strcpy(info[STATUS], "200");
       result = response( connfd, 200, fp, info, gettype(info[URL], config) );
     }
  }
  else
  {
     result = response( connfd, code, INVALID, info, gettype(info[URL], config) );
  }
  

  /*for test*/
  /*printf("%s %s\n\n\n", info[URL], info[VERSION] );*/
  
  /*
  write( 2, buffer, n );
  */
  return result;
}


/* handle request (single process version) */
void
handlereqsgl( int listenfd, int logged, int recording, 
              char config[][CONFSIZE])
{
   int connfd, len, n, err;
   SAI cliaddr;
   char log[ LOGLEN ], cliinfo[ TMPLEN ], addr[ INET_ADDRSTRLEN ], acptime[ TIMELEN ] ,
        clstime[ TIMELEN ], repinfo[ INFOSIZ ] ,info[INFOLEN][TMPLEN] ;
   err = 0;
   len = sizeof( cliaddr );
   connfd = Accept( listenfd, (SA*) &cliaddr, &len );

   /* record accepted time */
   strcpy( acptime, getdatetime() );
 
   /* handle request , if fail to do , return */
   if ( handlereq( connfd , logged, recording, config, info ) == false )
     err = errno; 

   close( connfd );

   /* record closed time */
   strcpy( clstime, gettime() );
   /* record client address and port */
   inet_ntop( AF_INET, &cliaddr.sin_addr, addr, TMPLEN );
   sprintf( cliinfo, "%s %d ", addr, ntohs( cliaddr.sin_port ) );

   /* generate log */ 
   sprintf( log, "%s %s %s %s %s %s %d\n", acptime, clstime, cliinfo, 
            info[URL], info[STATUS], info[CONTENTLEN], err ); 

   /* for test */
   fprintf(stderr, "%s", log );

   if( logged )
   {
       
   }
   
}

