#include <arpa/inet.h>
#include <dirent.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* for fgets        */
#include <string>
#include <strings.h>    /* for bzero, bcopy */
#include <sys/socket.h> /* for socket use   */
#include <sys/stat.h>
#include <unistd.h> /* for read, write  */
#include <vector>

using namespace std;

#define MAXLINE 8192 /* max text line length */
#define MAXBUF 8192  /* max I/O buffer size */
#define LISTENQ 1024 /* second argument to listen() */

struct server_t {
  string name;
  string ip;
  uint16_t port;
  bool available;
};

struct dir_list {
  string name;
  vector<string> list;
};

void ls(vector<server_t> &servers);

void get(string filename, vector<server_t> &servers);

void put(string filename, vector<server_t> &servers);