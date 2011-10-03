#include "libhttp.h" 
extern int errno;
static buf Buffer;
int end;

static char*
replace( const char* st )
{
  long i, j, size;
  char *new;
 
  size = strlen(st);

  new = (char*)malloc( sizeof(char) * strlen(st) * TIMES);
  for(i=0,j=0; i<=size; i++)
  {
    if( isspace(st[i]) ) 
    {
      new[j++] = '%';
      new[j++] = '2';
      new[j++] = '0';
    }
    else
    {
       new[j++] = st[i];
    }
  }
  return new; 

}


static void 
unreadbyte()
{
  Buffer.size++;
  Buffer.current--;
}


char* 
readline( int fd )
{
  static char line[LINELEN], c;
  int i;
  char tmp;

  i = 0;

  do
  {
    c = readbyte(fd);
    if( c == '\r' || c == '\n' )
    {
      tmp = readbyte(fd);
    
      
      /* try looking whether the next char is \r or \n */
      if ( tmp != '\r' && tmp != '\n' )
        unreadbyte();

      if( tmp == c )
        break;
       
      
      if( tmp == EOF )
      {
         line[i] = '\0';
         return line;
      }
      
      break;    
    }

        
    if( c == EOF )
    {
       line[i] = '\0';
       return line;
    }

    line[i++] = c;

  }
  while( i< LINELEN );

  line[i] = '\0'; 
 
  return line;
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

  return select(fd + 1,&rset,NULL,NULL, &t);
}


/* read one byte each time, buffer applied */
char 
readbyte( int fd )
{
  int n; 
  char c;

  /* check if buffer is empty */ 
  if ( Buffer.size > 0 )
  {
    Buffer.size--;
    c = Buffer.pool[Buffer.current];

    Buffer.current++; 
    return c;
  }

  while(true)
  { 
      
      if( settimeout( fd, TIMEOUT ) <= 0 )
      {
        end = true;
        return EOF;
      }
      
      /*
      printf("%d\n", i);
      i++;
      */ 
      n = read( fd, Buffer.pool, BUFFERLEN );
      
      if( n < 0 )
      {
     
         /*perror("error:");
         printf("%d\n", errno);*/
         if( errno == EINTR )
         {
           continue;
         }
         else
         {
           /*err_quit("Read error", false);   */
           return EOF;
         }
      }
      
      break;
  }

  if( n == 0 )
  {
      end = true;
      return EOF;
  }

  Buffer.size = n;
  Buffer.current = 0; 

  Buffer.size--;
  c = Buffer.pool[ Buffer.current++ ];

  return c;
}


int 
Socket( int family, int type, int protocol )
{
   int sockfd;
   sockfd = socket( family, type, protocol );
   if( sockfd < 0 ) 
   {
     err_quit("Socket error");     
   }

   return sockfd;
}


int 
Connect( int sockfd, const struct sockaddr *servaddr,
         socklen_t addrlen)
{
  int n;
  n = connect( sockfd, servaddr, addrlen );
  if ( n < 0 )
  {
    err_quit("Connect error");
  }

  return n;
}


void 
Request( int httpfd, const char *path , const char *host)
{
  if( request( httpfd, path , host ) < 0 )
  {
     err_quit("Fail to send request"); 
  }
}


int 
request( int httpfd, const char *path , const char *host)
{
  int i;
  char req[MAX_REQLEN]; 
  unsigned int sent;

  sprintf( req, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", 
           path, host);

  sent = 0;
  for( i=0; i<strlen(req); i++ )
  {
      send( httpfd, req+i , 1, 0 );
  } 
     

  return 0;
}


/* initialise the buffer */
static void
init_buffer()
{
  Buffer.size = 0;
  Buffer.current = -1;
}


int 
httpconnect( const char *addr , const char *port )
{
  init_buffer();
  end = false;
  if( port == NULL )
    return tcpconnect( addr, WEB_PORT );
  else
    return tcpconnect( addr, port );
}


int
tcpconnect( const char *addr, const char *port )
{
  /*struct hostent *h;*/
  int sockfd;
  /*struct sockaddr_in sa;*/
  struct addrinfo hints, *res; 

  bzero(&hints, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = 0;

  if( getaddrinfo( addr, port , &hints, &res) != 0 )
  {
     err_quit("Address is invalid");
  }
  

  sockfd = Socket( AF_INET, SOCK_STREAM, 0 );
  Connect( sockfd , res->ai_addr, res->ai_addrlen );
    
  return sockfd; 
}


static void 
printerr( const char *line )
{
  printf("%s\n", line+OFFSET); 
}


void
wget( const char* host, const char* port, const char* path , const char* saveto)
{
  int httpfd, i;
  char *line, c; 
  long size;
  FILE *fp, *fd;

  /*
  char tmp[1001];  
  int n;
  */

  httpfd = httpconnect( host , port );
  Request( httpfd, path , host );
  fd = fopen(RECVDATA, "a");


  line = readline( httpfd );
  fprintf(fd, "%s\n", line);

  if( strstr( line, OK ) == NULL  )
  {
    printerr( line );     
    exit(EXIT_FAILURE);
  }

  /* skip header */
  do
  {
    line = readline( httpfd );
    fprintf(fd, "%s\n", line);

    if( line[0] == '\0' )
     break;
  }while(1);

  /* begin to read content and save to file*/
  size = 0;
  fp = fopen(saveto, "w");

  /* for test */
  /* 
  while(true)
  {
    if( ( n = read( httpfd, tmp, 1000)) == 0  )
      break;
    tmp[1001] = '\0';
    fputs( tmp, fp); 
    size += n;
  }
  */ 

  i = 0; 
  c = readbyte( httpfd ); 
  while( end != true )
  {
    /* sleep if read 1KB */
    
    if( i == 1000 ){
       i = 0;
       sleep(1); 
    }
    
    size++;
    fputc(c, fp); 
    fputc(c, fd);
    c = readbyte( httpfd ); 
    i++;
  }
  
  fprintf(stdout, "%ld byte(s) fetched\n", size);
  fprintf(fd, "\n"); 

  fclose(fd);
  fclose(fp);
  close(httpfd);
}


char**
gethostandport( char* url )
{
   /* hostandport is an array, the first item is used to store the host 
    * the second item is used to store port, the third item is path*/
   static char *hostandport[LEN];
   char* tmp, *url_;
   url_ = strdup(url);

   if( strstr( url, "/" ) == NULL )
   {
      if( strstr( url, ":" ) == NULL )
      {
         hostandport[0] = (char*)malloc( sizeof(char) * HOSTLEN );
         strcpy( hostandport[0], url); 
         hostandport[1] = (char*)malloc( sizeof(char) * PORTLEN );
         strcpy( hostandport[1], WEB_PORT);
      }
      else
      {
         tmp = strtok(url, ":");
         hostandport[0] = (char*)malloc( sizeof(char) * HOSTLEN ) ;
         strcpy( hostandport[0], tmp);

         hostandport[1] = (char*)malloc( sizeof(char) * PORTLEN );
         strcpy( hostandport[1], strtok(NULL, ":"));
      }
      hostandport[2] = (char*)malloc( sizeof(char) * LEN );
      strcpy( hostandport[2] , "/" ); 
   }
   else
   {
     tmp = strtok(url, "/");
     if( strstr( tmp, ":" ) == NULL )
     { 
        hostandport[0] = (char*)malloc( sizeof(char) * HOSTLEN );
        strcpy( hostandport[0], tmp ); 
        hostandport[1] = (char*)malloc( sizeof(char) * PORTLEN );
        strcpy( hostandport[1], WEB_PORT);
     }
     else
     { 
        hostandport[0] = (char*)malloc( sizeof(char) * HOSTLEN );
        strcpy( hostandport[0] , strtok(tmp, ":") ); 
        hostandport[1] = (char*)malloc( sizeof(char) * PORTLEN );
        strcpy( hostandport[1], strtok(NULL, ":") );
     }

     hostandport[2] = (char*)malloc( sizeof(char) * HOSTLEN);
     strcpy( hostandport[2], strstr(url_, "/") ); 
   }
   
   
   hostandport[0] = replace(hostandport[0]);
   hostandport[2] = replace(hostandport[2]);
   
   free(url_);
   return hostandport;
}
