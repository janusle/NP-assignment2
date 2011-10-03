#include<sys/socket.h>
#include<sys/types.h>
#include<sys/uio.h>
#include<sys/select.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<strings.h>
#include<netdb.h>
#include<stdlib.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#include "error.h"


#define WEB_PORT "80"
#define MAX_REQLEN 300
#define BUFFERLEN 3000
#define LINELEN 200
#define OK "200"
#define OFFSET 9
#define TIMEOUT 900
#define NUMARG 3
#define HOSTLEN 200
#define LEN 3
#define PORTLEN 10
#define SENDDATA "gethttp-tx"
#define RECVDATA "gethttp-rx"
#define TIMES 3

typedef struct buffer
{
  char pool[BUFFERLEN];
  int size; 
  int current;
} buf;


/* wrapper functions */
int Socket( int family, int type, int protocol );
int Connect( int sockfd, const struct sockaddr *servaddr,
             socklen_t addrlen);

int tcpconnect( const char *addr, const char *port );
int httpconnect( const char *addr , const char *port);

int request( int httpfd, const char *path, const char *host );
void Request( int httpfd, const char *path , const char *host );


char readbyte( int fd );
char* readline( int fd);

void wget( const char* host, const char * port , const char* path
           ,const char* saveto );

char** gethostandport( char* url );
