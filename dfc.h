#include <stdio.h>
#include <stdlib.h>
#include <string.h>      /* for fgets        */
#include <strings.h>     /* for bzero, bcopy */
#include <unistd.h>      /* for read, write  */
#include <sys/socket.h>  /* for socket use   */
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/stat.h>

#define MAXLINE  8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */