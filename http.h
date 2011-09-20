#ifndef HTTP_H
#define HTTP_H

#include<netdb.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<strings.h>

#include "error.h"

#define PNAME "single-server"

typedef struct sockaddr SA;
typedef struct sockaddr_in SAI;
typedef struct addrinfo AR;

/*wrapper function*/

int Socket( int family, int type, int protocol );

void Bind( int sockfd, const SA *myaddr, socklen_t addrlen );

void Listen( int sockfd, int backlog );

int Accept( int sockfd, SA *cliaddr, socklen_t *addrlen );

pid_t Fork( void );

void Close( int sockfd );

/* initializing functions */
int init( char *host, char *port, int backlog );


#endif
