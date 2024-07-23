/*
 * echoservert.c - A concurrent echo server using threads
 */

#include "nethelp.h"
#include <iostream>
// #include "signal.h"

void *thread(void *vargp);

using namespace std;

char dir[30];
int main(int argc, char **argv) {
  int listenfd, *connfdp, port, clientlen = sizeof(struct sockaddr_in);
  struct sockaddr_in clientaddr;
  pthread_t tid;

  // signal(SIGPIPE, SIG_IGN);
  if (argc != 3) {
    fprintf(stderr, "usage: %s ./dfs<n> <port>\n", argv[0]);
    exit(0);
  }

  // read the config file.

  int status = mkdir(argv[1], 0777);
  if (status == 0) {
    cout << "Directory: " << argv[1] << " created." << endl;
  } else {
    cout << "Directory: " << argv[1] << " already created." << endl;
  }
  strcpy(dir, argv[1]);

  port = atoi(argv[2]);
  listenfd = open_listenfd(port);

  while (1) {
    // printf("test\n");
    connfdp = (int *)malloc(sizeof(int));
    *connfdp =
        accept(listenfd, (sockaddr *)&clientaddr, (socklen_t *)&clientlen);
    pthread_create(&tid, NULL, thread, connfdp);
  }
}

/* thread routine */
void *thread(void *vargp) {
  int connfd = *((int *)vargp);
  char buf[MAXLINE];
  char send_buf[MAXLINE];
  char command[20];
  size_t n;
  char *extracted_string, *ptr;
  char filename[30];
  char file_dest[30];

  pthread_detach(pthread_self());

  // echo(connfd);
  if ((n = readline(connfd, buf, MAXLINE)) != 0)
    ;

  // send ack to the client.
  // write(connfd, send_buf, strlen(send_buf));

  extracted_string = strtok_r(buf, " ", &ptr);
  if (extracted_string == NULL) {
    printf("No message in the socket\n");

    free(vargp);
    close(connfd);
    return 0;
  }

  strcpy(command, extracted_string);

  if (!strncmp(command, "get", 3)) {
    cout << "command is get" << endl;
    extracted_string = strtok_r(NULL, "\n", &ptr);
    if (extracted_string == NULL) {
      cout << "BAD THINGY" << endl;
      free(vargp);
      close(connfd);
      return 0;
    }
    strcpy(filename, extracted_string);
    cout << "FILENAME: " << filename << endl;
    FILE *file;
    char rec_buf[MAXLINE];
    int bytes_received;

    bzero(send_buf, sizeof(send_buf));

    sprintf(file_dest, "%s/%s", dir, filename);
    file = fopen(file_dest, "r");
    if (file == NULL) {
      cout << "NO" << endl;
      strcpy(send_buf, "no");
      send(connfd, send_buf, sizeof(send_buf), 0);
      free(vargp);
      close(connfd);
      return 0;
    }

    strcpy(send_buf, "yes");
    cout << "YES" << endl;
    if (send(connfd, send_buf, sizeof(send_buf), 0) == -1)
      cout << "FAILED" << endl;
    int bytes_read;
    while ((bytes_read =
                fread(send_buf, sizeof(char), sizeof(send_buf), file)) > 0) {
      cout << "READ: " << bytes_read << endl;
      send(connfd, send_buf, bytes_read, 0);
    }

  } else if (!strncmp(command, "ls", 2)) {
    cout << "command is ls" << endl;
    extracted_string = strtok_r(NULL, "\n", &ptr);
    if (extracted_string == NULL) {
      // cout << "BAD THINGY" << endl;
      free(vargp);
      close(connfd);
      return 0;
    }
    strcpy(filename, extracted_string);

    char dir_buf[MAXLINE];
    DIR *directory = opendir(dir);
    if (dir == NULL) {
      cout << "Cannot open the directory." << endl;
      return 0;
    }
    struct dirent *entry;
    // cout << "HERE" << endl;
    sprintf(dir_buf, "first\n");
    while ((entry = readdir(directory)) != NULL) {
      if (!strncmp(entry->d_name, ".", 1))
        continue;
      printf("%s\n", entry->d_name);

      sprintf(dir_buf, "%s%s\n", dir_buf, entry->d_name);
    }
    send(connfd, dir_buf, sizeof(dir_buf), 0);
    bzero(dir_buf, sizeof(dir_buf));
  } else if (!strncmp(command, "put", 3)) {
    cout << "command is put" << endl;
    extracted_string = strtok_r(NULL, "\n", &ptr);
    if (extracted_string == NULL) {
      free(vargp);
      close(connfd);
      return 0;
    }
    strcpy(filename, extracted_string);
    cout << "FILENAME: " << filename << endl;
    FILE *file;
    char rec_buf[MAXLINE];
    int bytes_received;

    sprintf(file_dest, "%s/%s", dir, filename);
    file = fopen(file_dest, "w");

    // put revieved stuff into a file.
    while ((bytes_received = recv(connfd, rec_buf, sizeof(rec_buf), 0)) > 0) {
      // cout << bytes_received << endl;
      // cout << rec_buf << endl;
      fwrite(rec_buf, sizeof(char), bytes_received, file);
      bzero(rec_buf, sizeof(rec_buf));
    }
    fclose(file);
  } else {
    cout << "command not available" << endl;
  }

  // free dynamic arg and the close the socket.
  free(vargp);
  close(connfd);
  return 0;
}
