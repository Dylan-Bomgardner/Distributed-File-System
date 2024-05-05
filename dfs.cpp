/* 
 * echoservert.c - A concurrent echo server using threads
 */

#include "nethelp.h"
//#include "signal.h"

void echo(int connfd);
void *thread(void *vargp);

int main(int argc, char **argv) 
{
    int listenfd, *connfdp, port, clientlen=sizeof(struct sockaddr_in);
    struct sockaddr_in clientaddr;
    pthread_t tid; 

    // signal(SIGPIPE, SIG_IGN);
    if (argc != 3) {
        fprintf(stderr, "usage: %s ./dfs<n> <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    listenfd = open_listenfd(port);
    
    while (1) {
    //    / printf("test\n");
        connfdp = (int*) malloc(sizeof(int));
        *connfdp = accept(listenfd, (sockaddr*) &clientaddr, (socklen_t*) &clientlen);
        pthread_create(&tid, NULL, thread, connfdp);
        
    }
}

/* thread routine */
void * thread(void * vargp) 
{  
    int connfd = *((int *)vargp);
    char buf[MAXLINE]; 
    char send_buf[MAXLINE];
    size_t n;
 s
    pthread_detach(pthread_self()); 
    
    //echo(connfd);
    if((n = readline(connfd, buf, MAXLINE)) != 0);

    //send ack to the client.
    write(connfd, send_buf, strlen(send_buf));


    //free dynamic arg and the close the socket.
    free(vargp);
    close(connfd);
    return 0;
}

/*
 * echo - read and echo text lines until client closes connection
 */
void echo(int connfd) 
{

    char http_code[30] = "200 OK";
    size_t n; 
    char buf[MAXLINE]; 
    char send_buf[MAXLINE];
    char* extracted_string, * ptr;
    char http_type[80], index[80], http_version[80];
    char* document_root = "./www";
    char content_type[20];
    char content_length[20];
    char file_location[100];
    char file_extension[10];
    FILE *file_ptr;
    struct stat st;
    int bytes_read;

    int counter = 0;
    if((n = readline(connfd, buf, MAXLINE)) != 0) {
        printf("server received %d bytes\n", n);
        extracted_string = strtok_r(buf, " ", &ptr);
        
        if(extracted_string == NULL) {
            printf("test1\n");
            bzero(http_code, sizeof(http_code));
            strcpy(http_code, "400 Bad Request");
            strcpy(content_length, "Content-Length: 0");
            sprintf(send_buf, "%s %s\r\n%s\r\n", "HTTP/1.1", http_code, content_length);
           // printf("THING: %s\n", send_buf);
            write(connfd, send_buf, strlen(send_buf));
            return;
        }
        strcpy(http_type, extracted_string);
        extracted_string = strtok_r(NULL, " ", &ptr);
        if(extracted_string == NULL) {
            printf("test2\n");
            bzero(http_code, sizeof(http_code));
            strcpy(http_code, "400 Bad Request");
            strcpy(content_length, "Content-Length: 0");
            sprintf(send_buf, "%s %s\r\n%s\r\n", "HTTP/1.1", http_code, content_length);
           // printf("THING: %s\n", send_buf);
            write(connfd, send_buf, strlen(send_buf));
            return;
        }
        strcpy(index, extracted_string);
        extracted_string = strtok_r(NULL, "\r", &ptr);
        if(extracted_string == NULL) {
            printf("test3\n");
            bzero(http_code, sizeof(http_code));
            strcpy(http_code, "400 Bad Request");
            strcpy(content_length, "Content-Length: 0");
            sprintf(send_buf, "%s %s\r\n%s\r\n", "HTTP/1.1", http_code, content_length);
           // printf("THING: %s\n", send_buf);
            write(connfd, send_buf, strlen(send_buf));
            return;
        }
        strcpy(http_version, extracted_string);

        printf("http_type: %s, index: %s, ver: %s\n", http_type, index, http_version);
        
        //Error Handling for not GET
        if(strncmp(http_type, "GET", 3)) {
            bzero(http_code, sizeof(http_code));
            strcpy(http_code, "405 Method Not Allowed");
        }

        //Error Handling for a version other than 1.0 or 1.1
        if(strncmp(http_version, "HTTP/1.1", 8) && strncmp(http_version, "HTTP/1.0", 8)) {
            printf("test HERE %s\n", http_version);
            bzero(http_code, sizeof(http_code));
            strcpy(http_code, "505 HTTP Version Not Supported");
        }
        //printf("test\n");
        //Send error back in case there is an error so far
        //printf("CODE: %s\n", http_version);
        if(strncmp(http_code, "200", 3)) {
            printf("test\n");
            strcpy(content_length, "Content-Length: 0");
            sprintf(send_buf, "%s %s\r\n%s\r\n", http_version, http_code, content_length);
            printf("THING: %s\n", send_buf);
            write(connfd, send_buf, strlen(send_buf));
            return;
        }

        //get the file location and add it to the variable..
        if(!strcmp(index, "/")) {
            bzero(index, sizeof(index));
            strcpy(index, "/index.html");
        }
        sprintf(file_location, "%s%s", document_root, index);
        printf("%s\n", file_location);
        if(access(file_location, F_OK) != -1) {
            //printf("FILE EXISTS\n");
        }
        else {
            printf("file does not exist\n");
            bzero(http_code, sizeof(http_code));
            strcpy(http_code, "404 Not Found");
            strcpy(content_length, "Content-Length: 0");
            sprintf(send_buf, "%s %s\r\n%s\r\n", http_version, http_code, content_length);                
            write(connfd, send_buf, strlen(send_buf));
            return;
        }

        file_ptr = fopen(file_location, "r");
        if(file_ptr == NULL) {
            printf("Permission issue opening the file.\n");
            bzero(http_code, sizeof(http_code));
            strcpy(http_code, "403 Forbidden");
            strcpy(content_length, "Content-Length: 0");
            sprintf(send_buf, "%s %s\r\n%s\r\n", http_version, http_code, content_length);                
            write(connfd, send_buf, strlen(send_buf));
            return;
        }

        //get length of the file.
        if(stat(file_location, &st) == 0) {
            printf("Content-Length: %lld\n", (long long)st.st_size);
            sprintf(content_length, "Content-Length: %lld", (long long)st.st_size);
        } 
        else {
            printf("Error here.\n");
        }
        //get the file extension
        const char* file_substrings = strrchr(file_location, '.');
        strcpy(file_extension, file_substrings + 1);

        printf("file extension: %s\n", file_extension);
        if(!strcmp(file_extension, "html")) strcpy(content_type, "Content-Type: text/html");
        else if(!strcmp(file_extension, "txt")) strcpy(content_type, "Content-Type: text/plain");
        else if(!strcmp(file_extension, "png")) strcpy(content_type, "Content-Type: image/png");
        else if(!strcmp(file_extension, "gif")) strcpy(content_type, "Content-Type: image/gif");
        else if(!strcmp(file_extension, "jpg")) strcpy(content_type, "Content-Type: image/jpg");
        else if(!strcmp(file_extension, "ico")) strcpy(content_type, "Content-Type: image/x-icon");
        else if(!strcmp(file_extension, "css")) strcpy(content_type, "Content-Type: text/css");
        else if(!strcmp(file_extension, "js")) strcpy(content_type, "Content-Type: application/javascript");
        else {
            printf("File Extension not supported.\n");
            bzero(http_code, sizeof(http_code));
            strcpy(http_code, "403 Forbidden");
            strcpy(content_length, "Content-Length: 0");
            sprintf(send_buf, "%s %s\r\n%s\r\n", http_version, http_code, content_length);                
            write(connfd, send_buf, strlen(send_buf));
            return;
        }
        //Construct HTTP Header

        sprintf(send_buf, "%s %s\r\n%s\r\n%s\r\n\r\n", http_version, http_code, content_type, content_length);
        write(connfd, send_buf, strlen(send_buf));
        
        //Now start sending the contents of the file.
        char line[MAXLINE];
        while ((bytes_read = fread(line, 1, MAXLINE, file_ptr)) > 0) {
            bzero(send_buf, sizeof(send_buf));
            memcpy(send_buf, line, bytes_read);
            write(connfd, send_buf, bytes_read);
        }
        fclose(file_ptr);
        return;
        }
}
