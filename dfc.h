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
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <functional>
#include <dirent.h>

using namespace std;

#define MAXLINE  8192  /* max text line length */
#define MAXBUF   8192  /* max I/O buffer size */
#define LISTENQ  1024  /* second argument to listen() */

struct server_t {
    string name;
    string ip;
    uint16_t port;
    bool available;
};

void ls();

void get(string filename, vector<server_t> &servers);

void put(string filename, vector<server_t> &servers);