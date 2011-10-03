#include "http.h"
#define SERVERNAME "webserver"



void 
dolog( char* filename, char *log )
{
    FILE* fp;
    fp = fopen( filename, "a" );
    if( fp == NULL )
     err_quit("Log error");

    fprintf(fp, "%s", log );
    fclose(fp); 
}


void
dolog_withtime( char* filename, char* log )
{
   char st[ LOGLEN ];

   strcpy( st, getdatetime() );
   strcat( st , log );
   dolog( filename, st );

}


void 
printflog_withtime( char* filename, char* format, char* time, int pid )
{
   char st[ LOGLEN ];

   sprintf( st, format, time, pid );
   dolog( filename, st );
}


void
printflogth_withtime( char* filename, char* format, int threadid )
{
   char st[ LOGLEN ];
   sprintf( st, format, threadid );
   dolog( filename, st );
}


int 
Select( int maxfdp1, fd_set *readset, 
        fd_set *writeset, fd_set *exceptset, 
        struct timeval *timeout )
{
   int result;

   while(1){
     result = select( maxfdp1, readset, writeset, exceptset, timeout );
     if( result < 0 ){
      if( errno == EINTR ){
        return -1;
      }
      else
        err_quit("select error");
     }
     return result;
   }
}


void
Write( int fildes, void* buf, size_t nbyte )
{
  size_t left, n, written;
  
  left = nbyte;
  written = 0;

  while( left > 0 ){
    if( (n = write( fildes, buf+written , left )) < 0 ){
        if( errno == EINTR )
          n = 0;
        else if( errno == EPIPE ){
          fprintf(stderr,"PIPE ERROR\n");
          return;
        }
        else
          err_quit("Write error");

    }
    
    /*fprintf(stderr, " n is %ld errno is %d\n", n, errno );*/
    written += n;
    left = nbyte-written; 
  }
  
}


void
Pipe( int fileds[2] )
{
   if( pipe(fileds) < 0 )
    err_quit("Pipe error");
}


pid_t
Wait( int *stat_loc )
{
  pid_t pid;

  do{
    pid = waitpid( -1, NULL, 0 );
    if( pid < 0 )
    {
       if( errno != EINTR )
         err_quit("Wait error"); 
    }

    if( pid > 0 )
      return pid;
  }
  while(1);

}


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
    err_quit( "Socket error" ); 
  }

  return listenfd;
}


void
Bind( int sockfd, const struct sockaddr *myaddr, socklen_t addrlen )
{

  if( bind( sockfd, myaddr, addrlen ) < 0 )
  {
    err_quit( "Bind Error" );
  }

}


void
Listen( int sockfd, int backlog )
{
  if( listen( sockfd, backlog ) < 0 )
  {
    err_quit( "Listen error" );
  }
}


int 
Accept( int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen )
{
  int connectfd;

  while( (connectfd = accept( sockfd, cliaddr, addrlen )) < 0 )
  {
    if( errno == EINTR )
      continue;

    err_quit( "Accept error" );
  }
  
  return connectfd;
}


pid_t 
Fork( void )
{
  pid_t pid;

  if( (pid=fork()) < 0 )
  {
     err_quit( "Fork error" );
  }

  return pid;
}


void
Close( int sockfd )
{
  if( close( sockfd ) < 0 )
  {
    err_quit( "Close error" );
  }

}

static void
sig_pipe( int signum )
{
   fprintf(stderr, "Pipe error, process will quit\n");
   exit(EXIT_FAILURE);
}


int 
init( char *host, char *port , int backlog )
{
   
   int listenfd, opt;
   AR hints, *res;

   bzero( &hints, sizeof(hints) );
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;
 
   if( getaddrinfo( host, port, &hints, &res ) != 0 )
   {
      err_quit( PNAME );
   }

   listenfd = Socket( AF_INET, SOCK_STREAM , 0 );
 
   opt = 1;
   if( setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, 
                   &opt, sizeof(int) ) == -1 )
     err_quit("setsockopt");
   
   Bind( listenfd , res->ai_addr, res->ai_addrlen );

   Listen( listenfd, backlog );  

   signal( SIGPIPE, sig_pipe );

   return listenfd;
}


char*
getdatetime()
{
   static char str[TIMELEN];  
   time_t timer;

   timer = time(NULL);
   strftime( str, TIMELEN, "%d/%m/%Y %H:%M:%S" , localtime(&timer) );
   
   return str;
}


char*
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
gettype( char* url, char config[][CONFSIZE],
         contenttyp* type[ TYPENUM ] )
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
   
   for( i=0; type[i] != NULL; i++ ){
      if( strcmp( type[i]->ext, tmp ) == 0)
      {
         return type[i]->contype;
      }
   }

   return config[DEFAULT];
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

    sprintf( tmp, "Content-Length: %zu\r\n", length );
    strcat( header, tmp);

    strcat( header, "Server: myserver\r\n" );
    
    strcat( header, "\r\n" );
    
    return header; 
}


static int 
response_select( clientinfo *cli , int code ,
                 int fp , char info[][TMPLEN], 
                 char* type )
{
   size_t length = 0;
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
     {
        
     }
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
   if( cli->siz == 0 ){
      length = getfilesize( fp );
      if( length < 0 )
      {
         return false;
      } 
      /* record content-length */
      sprintf( info[CONTENTLEN], "%zu", length );
      strcpy(cli->info[CONTENTLEN], info[CONTENTLEN]);

      /* header */
      header = genheader( code, length, type );  
      cli->wrbuf = (char*)Malloc(length + strlen(header) );
      strcpy( cli->wrbuf, header );
      read( fp, cli->wrbuf+strlen(header), length );
      cli->siz = length + strlen(header);
      close( fp );
   }

   if( cli->ptr < cli->siz ){ /* haven't finished */
        
        if( cli->siz - cli->ptr > WRITESIZ ){
          Write( cli->fd , cli->wrbuf + cli->ptr, WRITESIZ );
          cli->ptr = cli->ptr + WRITESIZ;
        }
        else{
          Write( cli->fd, cli->wrbuf + cli->ptr, cli->siz-cli->ptr);
          cli->ptr = cli->siz;
        }
   }
   else{ /* finish to write */
        cli->state = FINISH;  
        free( cli->wrbuf );
       }
     

   return true;
}


static int 
response( int connfd ,int code, int fp , char info[][TMPLEN], 
          char* type )
{
   size_t length = 0, left;
   char *buffer;
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
     {
        
     }
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
   if( fp > 0 )
   {
     length = getfilesize( fp );
     if( length < 0 )
       return false;
     
     buffer = (char*)Malloc(length);

     /* record content-length */
     sprintf( info[CONTENTLEN], "%zu", length );

     /* send header */
     header = genheader( code, length, type );  
     Write( connfd, header, strlen(header) ); 
     
     /* send content */
     if( fp > 0 )
     {
       left = length;
       read( fp, buffer, length );
       Write( connfd, buffer, length );
       close( fp );
     }
     
     free(buffer);
   }

   return true;
}


static int
returnstat( int connfd, char info[][TMPLEN], int actconn, 
            long totalreq, char* port, char* sig, char*sdfile )
{
  char buffer[ BUFFERLEN ], *header;
  long len;

  sprintf( buffer, STATPAGE, getrptime(), actconn, totalreq, 
           port, sig, (long long int)sd->pid, sdfile );

  len = strlen(buffer);

  /* record CONTENTLEN */
  sprintf( info[CONTENTLEN], "%ld", len );
  
  /* send header */
  header = genheader( 200, len, "text/html" ); 

  Write( connfd, header, strlen( header ) ); 

  /* send content */
  Write( connfd, buffer, len );

  return true;
}


static int 
settimeout( int fd, int sec )
{
   struct timeval t;
   fd_set rset;

   t.tv_sec = sec;
   t.tv_usec = 0;
   FD_ZERO(&rset);
   FD_SET(fd, &rset);
   
   return select( fd+1, &rset, NULL, NULL, &t );
}



static int
readreq( int connfd ,char config[CONFLEN][CONFSIZE] ,
         char buffer[ BUFFERLEN ])
{
  int n, siz;
  FILE *fd ;

  siz = 0;
  do{

    if( settimeout( connfd, TIMEOUT ) <= 0 )
      break;

    n = read( connfd, buffer+siz, BUFFERLEN );
 
    if ( n == 0 )
      break;

    if( n < 0 )
    {
       if ( errno == EINTR )
         continue;
       return false;
    }

    siz += n; 
    n = 0;

    /* check if it is the end of http request */
    if( siz >= 4 ) 
    {
       if( buffer[siz-2] ==  '\r' && buffer[siz-1] == '\n' &&
         buffer[siz-4] == '\r' && buffer[siz-3] == '\n' )
         break; 
    } 

  }
  while(1);

  buffer[siz] = '\0';
  
  if( strcmp( config[RECORDING], "yes" ) == 0 )
  {

     fd = fopen( config[RD], "w" );
     fprintf(fd, "%s\n", buffer );
     fclose(fd);
  }

  return true;
}



int repon(clientinfo *cli, 
          char config[CONFLEN][CONFSIZE] , 
          contenttyp* type[ TYPENUM ], char info[][TMPLEN])
{
  int fp, code, result;
  char address[ TMPLEN ];


  /* parse header */
  code = parseheader( cli->buffer, strlen(cli->buffer) , config, info );

  strcpy(cli->info[URL], info[URL]);
  strcpy(cli->info[VERSION], info[VERSION] );

  /* request status */
  if( code == 0 && 
      strcmp( config[STAT], info[URL]+1 ) == 0 )
  {
      strcpy(info[STATUS], "200");
     
      strcpy(cli->info[STATUS], info[STATUS]);

      result = returnstat( cli->fd , info, sd->act, sd->req, config[PORT],
                           config[SDSIG],  config[SDFILE] );

      strcpy( cli->info[CONTENTLEN], info[CONTENTLEN] );
      cli->state = FINISH;
      return result;
  }

  /* shutdown */  
  if( code == 0 &&
      strcmp( config[SDFILE], info[URL] ) == 0)
  {
    kill( sd->pid, atoi(config[SDSIG]) );    

    strcpy( info[STATUS], "200" );
    strcpy( cli->info[STATUS], info[STATUS] );
    strcpy( info[CONTENTLEN], "0" );
    strcpy( cli->info[CONTENTLEN], info[CONTENTLEN] );

    cli->state = FINISH;
    return true;
  }
  

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
          strcpy(cli->info[STATUS], info[STATUS]);
          
          result = response_select( cli, 403, INVALID, info, 
                             gettype(info[URL], config, type) );      

        }
        else
        {
          strcpy(info[STATUS], "404");
          strcpy(cli->info[STATUS], info[STATUS]);

          result = response_select( cli, 404, INVALID, info, 
                             gettype(info[URL], config, type) );

        }
     }
     else
     {
       /* 200 OK */
       strcpy(info[STATUS], "200");
       strcpy(cli->info[STATUS], info[STATUS]);

       result = response_select( cli, 200, fp, info, 
                gettype(info[URL], config, type));

     }
  }
  else
  {

     strcpy(cli->info[STATUS], info[STATUS]);
     result = response_select( cli, code, INVALID, info, 
                        gettype(info[URL], config, type) );
  }


  return result;
}



/* handle request (general version) */
int
handlereq( int connfd ,
           char config[CONFLEN][CONFSIZE] , 
           contenttyp* type[ TYPENUM ], char info[][TMPLEN] )
{
  int n, code, result, siz;
  char buffer[ BUFFERLEN ], address[ TMPLEN ];
  int fp;  
  FILE *fd ;


  /* read request */
  siz = 0;
  do{

    if( settimeout( connfd, TIMEOUT ) <= 0 )
      break;

    n = read( connfd, buffer+siz, BUFFERLEN );
 
    if ( n == 0 )
      break;

    if( n < 0 )
    {
       if ( errno == EINTR )
         continue;
       return false;
    }

    siz += n; 
    n = 0;

    /* check if it is the end of http request */
    if( siz >= 4 ) 
    {
       if( buffer[siz-2] ==  '\r' && buffer[siz-1] == '\n' &&
         buffer[siz-4] == '\r' && buffer[siz-3] == '\n' )
         break; 
    } 

  }
  while(1);
  
 
  buffer[siz] = '\0';
  
  if( strcmp( config[RECORDING], "yes" ) == 0 )
  {

     fd = fopen( config[RD], "w" );
     fprintf(fd, "%s\n", buffer );
     fclose(fd);
  }


  /* parse header */
  code = parseheader( buffer, siz , config, info );

  /* request status */
  if( code == 0 && 
      strcmp( config[STAT], info[URL]+1 ) == 0 )
  {
      strcpy(info[STATUS], "200");
     
      result = returnstat( connfd , info, sd->act, sd->req, config[PORT],
                           config[SDSIG],  config[SDFILE] );
      return result;
  }

  /* shutdown */
  
  if( code == 0 &&
      strcmp( config[SDFILE], info[URL] ) == 0)
  {
     
    kill( sd->pid, atoi(config[SDSIG]) );    

    strcpy( info[STATUS], "200" );
    strcpy( info[CONTENTLEN], "0" );

    return true;
  }
  

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
          result = response( connfd , 403, INVALID, info, 
                             gettype(info[URL], config, type) );      
        }
        else
        {
          strcpy(info[STATUS], "404");
          result = response( connfd, 404, INVALID, info, 
                             gettype(info[URL], config, type) );
        }
     }
     else
     {
       /* 200 OK */
       strcpy(info[STATUS], "200");
       result = response( connfd, 200, fp, info, 
                gettype(info[URL], config, type));
     }
  }
  else
  {
     result = response( connfd, code, INVALID, info, 
                        gettype(info[URL], config, type) );
  }
  

  return result;
}


/* signal handler, waits child */
static void
sig_child( int signum )
{

   while( (waitpid( -1, NULL, WNOHANG) > 0 ) ){
      //pidnum--;  
      (sd->act)--;
   }; 
}


static void
sig_user1( int signum )
{}


void
sig_shutdown( int signum )
{ 

    fprintf( stderr, "Shutdown signal was caught\n" );

    if( strcmp( config[LOGGING], "yes") == 0 ){
       dolog_withtime( config[LOG], " shutdown signal was caught\n");
    }

    shutdown( listenfd , SHUT_RDWR );
    shutdown( connfd , SHUT_RD );

    /* if server is multiplexed */
    if( ismultiplexing ){
       shutflag = true; 
       return;
    }

    /* if server is single */
    if( isSingle && !isThreaded )
    {
      fprintf(stderr,"Server will exit\n");

       /* logging */
       if( strcmp( config[LOGGING], "yes") == 0 ){
         dolog_withtime( config[LOG], " terminating server\n");
       }
      exit(0);
    }
   
    /* if server is threaded */
    if( isSingle && isThreaded )
    {
       fprintf(stderr,"Waiting for all threads\n");
       
       /* logging */
       if( strcmp( config[LOGGING], "yes") == 0 ){
          dolog_withtime( config[LOG], " waiting for all threads\n");
       }

       while( sd->act > 0 ){
       }
       fprintf(stderr, "All threads returned.\n");
      
       /* logging */
       if( strcmp( config[LOGGING], "yes") == 0 ){
          dolog_withtime( config[LOG], " all threads returned.\n");
       }
       exit(0);
    }
    
    /* if server is forked */
    if( sd->act > 0 ){ 
      
      fprintf(stderr,"Waiting for all children\n");

      /* logging */
      if( strcmp( config[LOGGING], "yes") == 0 ){
          dolog_withtime( config[LOG], " waiting for all children.\n");
      }

      while( sd->act > 0  )
      {
        /* wait sd->act becoming 0 */
      }; 

      fprintf(stderr, "All child processes returned.\n");

      /* logging */
      if( strcmp( config[LOGGING], "yes") == 0 ){
          dolog_withtime( config[LOG], " all child processes returned.\n");
      }
    }

    fprintf(stderr, "Server will exit \n");

    /* logging */
    if( strcmp( config[LOGGING], "yes") == 0 ){
          dolog_withtime( config[LOG], " terminating server.\n");
    }

    exit(0);
}



sigfun*
signal( int signum, sigfun *fun)
{
   struct sigaction act, oact;

   act.sa_handler = fun;
   sigemptyset( &act.sa_mask );
   act.sa_flags = 0;
   
   if ( sigaction( signum, &act, &oact) < 0 )
     return SIG_ERR;
   return oact.sa_handler;
}


void
handlereqselect( char config[][CONFSIZE], 
              contenttyp* type[TYPENUM] )
{
   /* len is size of cliadd, err is error code, maxi is max of client
    * maxfd is max of descriptor */
   int sock, connfd, len, ready, err, maxi, maxfd, i;
   clientinfo client[FD_SETSIZE];
   fd_set rset, wset, allset;
   SAI cliaddr;
   char log[ LOGLEN ], cliinfo[ TMPLEN ], 
        addr[ INET_ADDRSTRLEN ], acptime[ TIMELEN ] ,
        clstime[ TIMELEN ], repinfo[ INFOSIZ ] ,info[INFOLEN][TMPLEN] ;
   FILE *fp;

   err = 0;
   len = sizeof( cliaddr );

   /* initializing */
   for( i=0; i<FD_SETSIZE; i++ )
       client[i].fd = -1;
   maxi = -1;
   maxfd = listenfd;
   FD_ZERO( &allset );
   FD_SET( listenfd, &allset );

   /* main loop starts here */
   for( ; ; ){

      /* check if shutdown signal was caught and active connection is zero */
      if( shutflag == true && 
          sd->act == 0 )
         return;

      rset = allset;
      wset = allset;

      ready = Select(maxfd+1, &rset, &wset, NULL, NULL ); 

      if( ready == -1 ){
          if( shutflag == true && sd->act > 0 )
            continue;
          else
            return;
      }
        

      if( shutflag == false && FD_ISSET( listenfd, &rset ) ){ 
         
         /* new connection */
         connfd = Accept( listenfd, (SA*) &cliaddr, &len );

         sd->req++;
         sd->act++;

         strcpy( acptime, getdatetime() );
         /* save new connection */
         for( i=0; i<FD_SETSIZE; i++ )
            if( client[i].fd < 0 ){
               client[i].fd = connfd;
               client[i].state = READREQ;
               client[i].ptr = 0;
               client[i].siz = 0;
               
               /* record accepted time */
               strcpy( client[i].log, getdatetime() );
               break;
            }

         FD_SET( connfd, &allset );
         
         if( connfd > maxfd )
           maxfd = connfd;
        
         if( i > maxi )
           maxi = i;
         
         if( --ready <= 0 )
           continue; /* no more descriptors */
      }
     

      /* process read set */
      for( i=0; i <=maxi; i++ ){
        if( (sock = client[i].fd) <0 )
          continue;
        
        if( client[i].state != READREQ )
          continue;

        if( FD_ISSET( sock, &rset) ){
      
          /* handle request , if fail to do , return */
          /*   
          if ( handlereq( sock, config, type, info) == false ) 
            err = errno; 
          */
          if( readreq( sock, config, client[i].buffer ) == false ){
              fprintf(stderr, "read error");
              continue;
          }
          
          client[i].state = RESPONSE;
          ready--;
        }
      }

      if( ready <= 0 )
        continue;

      /* process write set */
      for( i=0; i<=maxi; i++ ){
         if( (sock = client[i].fd) < 0 )
            continue;
        
         if( client[i].state == READREQ )
            continue;
         

         if( FD_ISSET( sock, &wset) ){
            
            if( client[i].siz == 0 ){

              if( repon(&client[i], config, type, info ) == false ){
                 fprintf(stderr, "write error\n");
                 client[i].state = FINISH;
                 continue;
              }
            }
            else{

              if ( response_select( &client[i], 200, -1, info, NULL ) == false ){
                 client[i].state = FINISH;
                 continue;
              }

            }

            /* write is not finished */
            if( client[i].state != FINISH )
              continue;

            /* close connection and clear fd set */
            close( sock );
            FD_CLR( sock, &allset ); 
            client[i].fd = -1;

            /* record closed time */
            strcpy( clstime, gettime() );
            //strcat( client[i].log, gettime() );

            /* record client address and port */
            inet_ntop( AF_INET, &cliaddr.sin_addr, addr, TMPLEN );
            sprintf( cliinfo, "%s %d ", addr, ntohs( cliaddr.sin_port ) );

            /* generate log */ 
            
            sprintf( log, "%s %s %s %s %s %s %d\n", acptime, clstime, cliinfo, 
                   client[i].info[URL], client[i].info[STATUS], 
                   client[i].info[CONTENTLEN], err ); 
            
            
            /* for test */
            fprintf(stderr, "%s", log );

            if( strcmp( config[LOGGING], "yes" ) == 0 )
            {
              fp = fopen( config[LOG] , "a");
              fprintf( fp, "%s", log );
              fclose(fp);
            }

            sd->act--;

            if( --ready <= 0 )
              break; /* no more descriptors */
         } 
      }
   } 
}



/* handle request (single process version) */
void
handlereqsgl( int listenfd, char config[][CONFSIZE], 
              contenttyp* type[TYPENUM] )
{
   int connfd, len, n, err;
   SAI cliaddr;
   char log[ LOGLEN ], cliinfo[ TMPLEN ], 
        addr[ INET_ADDRSTRLEN ], acptime[ TIMELEN ] ,
        clstime[ TIMELEN ], repinfo[ INFOSIZ ] ,info[INFOLEN][TMPLEN] ;
   FILE *fp;

   err = 0;
   len = sizeof( cliaddr );
 
   connfd = Accept( listenfd, (SA*) &cliaddr, &len );

   sd->req++;

   /* record accepted time */
   strcpy( acptime, getdatetime() );
 
   /* handle request , if fail to do , return */
   if ( handlereq( connfd , config, type, info) == false ) 
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

   fprintf(stderr, "%s", log );

   if( strcmp( config[LOGGING], "yes" ) == 0 )
   {
     fp = fopen( config[LOG] , "a");
     fprintf( fp, "%s", log );
     fclose(fp);
   }
   
}



void*
handlereq_th( void* data )
{
  servinfo* si;
  char info[INFOLEN][TMPLEN], log[ LOGLEN ], 
       cliinfo[ TMPLEN ], addr[ INET_ADDRSTRLEN ], 
       clstime[ TIMELEN ], repinfo[ INFOSIZ ]; 
  SAI cliaddr;
  int err;
  FILE *fp;

  pthread_detach( pthread_self());


  /* increment active connection and total request */
  
  pthread_mutex_lock(&act_mutex); 
  sd->act += 1;
  sd->req += 1; 
  pthread_mutex_unlock(&act_mutex);
  

  sprintf( log, " thread %ld start\n", (long)pthread_self() );

  fprintf(stderr, log );
  
  /* logging */
  if( strcmp( config[LOGGING], "yes" ) == 0 ){
     dolog_withtime( config[LOG], log ); 
  }

  si = (servinfo*) data;

  if ( handlereq( si->connfd, si->config,
                si->type, si->info ) == false )
  {
     err = errno;
  } 

  close( si->connfd );
  
  /* record closed time */
  strcpy( clstime, gettime() );

  /* record client address and port */
  inet_ntop( AF_INET, &(si->cliaddr.sin_addr), addr, INET_ADDRSTRLEN );
  sprintf( cliinfo, "%s %d ", addr, ntohs( si->cliaddr.sin_port ) );

  /* generate log */ 
  sprintf( log, "%s %s %s %s %s %s %d\n", si->acptime, clstime, cliinfo, 
           si->info[URL], si->info[STATUS], si->info[CONTENTLEN], err ); 

  fprintf(stderr, "%s", log );

  if( strcmp( si->config[LOGGING], "yes" ) == 0 )
  {
     fp = fopen( si->config[LOG] , "a");
     fprintf( fp, "%s", log );
     fclose(fp);
  }
       
  sprintf( log, " thread %ld is done\n", (long)pthread_self() );

  fprintf(stderr, log );
  
  /* logging */
  if( strcmp( config[LOGGING], "yes" ) == 0 ){
     dolog_withtime( config[LOG], log ); 
  }

  
  free(si);
  
  /* decrement active connection */

  pthread_mutex_lock(&act_mutex); 
  sd->act -= 1;
  pthread_mutex_unlock(&act_mutex);
  

  return NULL;
}



/* handle request( multithread version ) */
void
handlereqthread( int listenfd, char config[][CONFSIZE],
                 contenttyp* type[TYPENUM] )
{
    int len;
    pthread_t tid;
    servinfo *data = NULL;
    char acptime[ TIMELEN ];

    for( ; ; ){ 
        
        /* create a new servinfo */
        data = (servinfo*)Malloc( sizeof(servinfo) );
        data->config = config;
        data->type = type;

        len = sizeof( data->cliaddr );       

        connfd = Accept( listenfd, (SA*) &(data->cliaddr), &len ); 

        /* record accepted time */
        strcpy( data->acptime, getdatetime() );
        
        data->connfd = connfd; 
        
        if (pthread_create( &tid, NULL , handlereq_th, (void*)data)> 0){
           perror("Thread error");
        }
        
    }

}



/* handle request (forked version) */
void
handlereqfork( int listenfd, char config[][CONFSIZE], 
              contenttyp* type[TYPENUM] )
{
   int len, n, err, shmid;
   SAI cliaddr;
   pid_t pid;
   char log[ LOGLEN ], cliinfo[ TMPLEN ], 
        addr[ INET_ADDRSTRLEN ], acptime[ TIMELEN ] ,
        clstime[ TIMELEN ], repinfo[ INFOSIZ ] ,info[INFOLEN][TMPLEN],
        tmp[ TMPLEN ];
   sigset_t new, old , zero;

   FILE *fp;

   err = 0;
   len = sizeof( cliaddr );

   /* All opertions about signal */
   signal( SIGCHLD, sig_child );
   signal( SIGUSR1, sig_user1 );
   sigemptyset(&zero);
   sigemptyset(&new);
   sigaddset(&new, SIGUSR1 );
   if ( sigprocmask(SIG_BLOCK, &new, &old) < 0 )
     err_quit("signal error");

   /* Share the memory */ 
   shmid = shmget( IPC_PRIVATE, sizeof(sharedmem), IPC_CREAT | 0600 );
   if( shmid < 0 ){
      err_quit("Shared memory error");
   }
   sd = (sharedmem*) shmat( shmid, NULL, 0 );
   if( sd == (void*)-1 ){
     err_quit("Attach memory error");
   }
   /* set value to shared memory */
   sd->pid = getpid();
   sd->req = 0;
   sd->act = 0;


   for( ; ; ){
     
     connfd = Accept( listenfd, (SA*) &cliaddr, &len );

     (sd->req)++; /* total reqs increment by 1 */

     /* record accepted time */
     strcpy( acptime, getdatetime() );
    
     if( (pid = Fork()) == 0 ) /* child */
     {
        sigsuspend(&zero);
        

        if ( sigprocmask(SIG_SETMASK, &old, NULL ) < 0 )
           err_quit("signal error");

        fprintf(stderr, "%s child %d start\n", getdatetime(), getpid() );

        /* logging */
        if( strcmp( config[LOGGING], "yes") == 0 ){
          printflog_withtime( config[LOG], "%s child %d start\n", 
                              getdatetime(), getpid() );
        }

        /* handle request , if fail to do , set errno:w */
        if ( handlereq( connfd , config, type, info) == false ) 
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

        fprintf(stderr, "%s", log );

        if( strcmp( config[LOGGING], "yes" ) == 0 )
        {
          fp = fopen( config[LOG] , "a");
          fprintf( fp, "%s", log );
          fclose(fp);
        }
      
        if( strcmp( config[LOGGING], "yes") == 0 ){
          printflog_withtime( config[LOG], "%s child %d is done\n", getdatetime(), getpid() );
        }
        
        
        sprintf(log, "%s Child %d is done\n", getdatetime(), getpid() );
        fprintf(stderr, log );
        exit(0);
     }
    
     /* some important operations */
     close( connfd ); /* parent closes connected fd */

     /*pidnum++;*/
     (sd->act)++;
     

     kill( pid, SIGUSR1 ); /* tell child to start */
   }

}

