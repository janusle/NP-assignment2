#ifndef HTTP_H
#define HTTP_H

#include<netdb.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<strings.h>
#include<signal.h>
#include<stdlib.h>
#include<time.h>
#include<ctype.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/wait.h>
#include<pthread.h>
#include "error.h"

#define PNAME "webserver"
/* some sizes and lengths */
#define TIMELEN 50
#define LOGLEN 200
#define TMPLEN 300
#define BUFFERLEN 2048
#define INFOSIZ 2048
#define CONFLEN 11
#define INFOLEN 4
#define CONFSIZE 50

#define INVALID -1
/* config index */
#define PORT 0
#define HOST 1
#define ROOT 2
#define SDSIG 3
#define SDFILE 4
#define STAT 5
#define LOG 6
#define RD 7
#define DEFAULT 8
#define LOGGING 9
#define RECORDING 10

/* some lengths */
#define TYPENUM 200
#define TYPELEN 15
#define FILEBUF 500000
#define HEADERSIZ 1000
#define METHODLEN 300
#define HOSTLEN 5
#define GETLEN 3

/* client information */
#define URL 0
#define VERSION 1
#define CONTENTLEN 2
#define STATUS 3

/* status page */
#define STATPAGE "<h2>Server status page</h2>\
<br/>\
My WebServer <br/>\
Status at %s <br/><br/>\
Active connections: %d <br/>\
Total requests: %ld <br/>\
Listening port: %s <br/><br/>\
To shutdown, do 'kill -%s %lld' or click\
<a href=%s>here</a>"


#define MAXCLINET 100
#define TIMEOUT 3

/* FSM state for select server */
#define READREQ 0
#define RESPONSE 1
#define FINISH 2
#define WRITESIZ 1000

typedef struct sockaddr SA;
typedef struct sockaddr_in SAI;
typedef struct addrinfo AR;
typedef void sigfun(int signum);

/* for storing content type */
typedef struct{
    char ext[ TYPELEN ];
    char contype[ TYPELEN ];
} contenttyp;

/* shared memory for all children */
typedef struct{
  pid_t pid; /* parent's pid */
  int act; /* active connection */
  long req; /* total requests */
} sharedmem;

/* server infomation */
typedef struct{
  int connfd;
  char (*config)[CONFSIZE];
  contenttyp (*type)[TYPENUM];
  char info[INFOLEN][TMPLEN];
  char acptime[ TIMELEN ];
  SAI cliaddr;
} servinfo; 

/* client information */
typedef struct{
  int fd;
  int state;
  int siz;
  int ptr;
  char info [INFOLEN][TMPLEN];
  char buffer[ BUFFERLEN ];
  char log[ LOGLEN ];
  char *wrbuf;
} clientinfo;



int listenfd;
/* pointer to shared memory */
sharedmem *sd;
/* is server single, threaded or multiplexing */
int isSingle;
int isThreaded;
int ismultiplexing;
/* shutdown flag for select server */
int shutflag;
/* mutex lock for threaded server */
pthread_mutex_t act_mutex;
/* config file */
char config[ CONFLEN ][ CONFSIZE ];


/*wrapper function*/

void Pipe( int fileds[2] );

int Socket( int family, int type, int protocol );

void Bind( int sockfd, const SA *myaddr, socklen_t addrlen );

void Listen( int sockfd, int backlog );

int Accept( int sockfd, SA *cliaddr, socklen_t *addrlen );

pid_t Fork( void );

void Close( int sockfd );

void *Malloc( size_t size );

pid_t Wait( int *stat_loc );


/* initializing functions */
int init( char *host, char *port, int backlog );

/* handlers */
void handlereqsgl(int listenfd,
                  char config[][CONFSIZE], contenttyp* type[TYPENUM]);

int handlereq( int connfd, char config[][CONFSIZE], 
                contenttyp* type[ TYPENUM ], char info [][TMPLEN] );

void handlereqfork( int listenfd, char config[][CONFSIZE], contenttyp* type[TYPENUM] );

void handlereqthread( int listenfd, char config[][CONFSIZE], contenttyp* type[TYPENUM] );

void handlereqselect( char config[][CONFSIZE], contenttyp* type[TYPENUM] );

/* signal */
sigfun* signal( int signum, sigfun *fun );

void sig_shutdown( int signum );

/* log related functions*/
void dolog( char* filename, char *log );

void dolog_withtime( char* filename, char*log );

void printflog_withtime( char* filename, char* format, char* time, int pid );

/* time related functions*/
char* getdatetime();

char* gettime();

/* replace all '%20' in the address to space */
void cleanadd( char* address );

#endif
