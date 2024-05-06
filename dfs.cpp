/* 
 * echoservert.c - A concurrent echo server using threads
 */

#include "nethelp.h"
#include <iostream>
//#include "signal.h"

void echo(int connfd);
void *thread(void *vargp);

using namespace std;

char dir[30];
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

    //read the config file. 


    int status = mkdir(argv[1], 0777);
    if(status == 0) {
        cout << "Directory: " << argv[1] << " created." << endl;
    }
    else {
        cout << "Directory: " << argv[1] << " already created." << endl;
    }
    strcpy(dir, argv[1]);

    port = atoi(argv[2]);
    listenfd = open_listenfd(port);
    
    while (1) {
        //printf("test\n");
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
    char command[20];
    size_t n;
    char* extracted_string, *ptr;
    char filename[30];
    char file_dest[30];

    pthread_detach(pthread_self()); 
    
    //echo(connfd);
    if((n = readline(connfd, buf, MAXLINE)) != 0);

    //send ack to the client.
    //write(connfd, send_buf, strlen(send_buf));
    
    extracted_string = strtok_r(buf, " ", &ptr);
    if(extracted_string == NULL) {
        printf("No message in the socket\n");

        free(vargp);
        close(connfd);
        return 0;
    }

    strcpy(command, extracted_string);

    if(!strncmp(command, "get", 3)) {
        cout << "command is get" << endl;
        extracted_string = strtok_r(NULL, "\n", &ptr);
        if(extracted_string == NULL) {
            cout << "BAD THINGY" << endl;
            free(vargp);
            close(connfd);
            return 0;
        }
        strcpy(filename, extracted_string);
        cout << "FILENAME: " << filename << endl;
        FILE* file;
        char rec_buf[MAXLINE];
        int bytes_received;
        
        bzero(send_buf, sizeof(send_buf));
        
        sprintf(file_dest, "%s/%s", dir, filename);
        file = fopen(file_dest, "r");
        if(file == NULL) {
            cout << "NO" << endl;
            strcpy(send_buf, "no");
            send(connfd, send_buf, sizeof(send_buf), 0);
            free(vargp);
            close(connfd);
            return 0;
        }
        
        strcpy(send_buf, "yes");
        cout << "YES" << endl;
        if(send(connfd, send_buf, sizeof(send_buf), 0) == -1) cout << "FAILED" << endl;
        int bytes_read;
        while((bytes_read = fread(send_buf, sizeof(char), sizeof(send_buf), file)) > 0) {
            cout << "READ: " << bytes_read << endl;
            send(connfd, send_buf, bytes_read, 0);
        }

    }
    else if(!strncmp(command, "ls", 2)) {
        cout << "command is ls" << endl;
        extracted_string = strtok_r(NULL, "\n", &ptr);
        if(extracted_string == NULL) {
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
        //cout << "HERE" << endl;
        sprintf(dir_buf, "first\n");
        while ((entry = readdir(directory)) != NULL) {
            if(!strncmp(entry->d_name, ".", 1)) continue;
            printf("%s\n", entry->d_name);
            
            sprintf(dir_buf, "%s%s\n", dir_buf, entry->d_name);
        }
        send(connfd, dir_buf, sizeof(dir_buf), 0);
        bzero(dir_buf, sizeof(dir_buf));
    }
    else if(!strncmp(command, "put", 3)) {
        cout << "command is put" << endl;
        extracted_string = strtok_r(NULL, "\n", &ptr);
        if(extracted_string == NULL) {
            free(vargp);
            close(connfd);
            return 0;
        }
        strcpy(filename, extracted_string);
        cout << "FILENAME: " << filename << endl;
        FILE* file;
        char rec_buf[MAXLINE];
        int bytes_received;
        
        sprintf(file_dest, "%s/%s", dir, filename);
        file = fopen(file_dest, "w");

        //put revieved stuff into a file. 
        while ((bytes_received = recv(connfd, rec_buf, sizeof(rec_buf), 0)) > 0) {
            // cout << bytes_received << endl;
            // cout << rec_buf << endl;
            fwrite(rec_buf, sizeof(char), bytes_received, file);
            bzero(rec_buf, sizeof(rec_buf));
         }
        fclose(file);
    }
    else {
        cout << "command not available" << endl;
    }
    

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

//     char http_code[30] = "200 OK";
//     size_t n; 
//     char http_request[BUFSIZ];
//     char buf[MAXLINE], host_buf[MAXLINE], first_buf[MAXLINE]; 
//     char send_buf[MAXLINE];
//     char* extracted_string, * ptr;
//     char http_type[80], index[80], http_version[80];
//     char* cache_location = "./cache/";
//     char content_type[20];
//     char content_length[20];
//     char file_location[100];
//     char file_extension[10];
//     FILE *file_ptr;
//     struct stat st;
//     int bytes_read;
//     char hostname[100];

//     int counter = 0;
//     int prev_n = 0;
//     n = recv(connfd, http_request, MAXLINE, 0);
//    // printf("buf %s\n", buf);
//     //memcpy(http_request + prev_n, buf, n);
//     //printf("HTTP REQUEST: %s", http_request);
    

//     //printf("%s", http_request);
    

//     //printf("server received %d bytes\n", n);
    
//     //return;
//     strcpy(first_buf, http_request);
//     strcpy(buf, http_request);
//     extracted_string = strtok_r(buf, " ", &ptr);
       
//     if(extracted_string == NULL) {
//         printf("test1\n");
//         bzero(http_code, sizeof(http_code));
//         strcpy(http_code, "400 Bad Request");
//         strcpy(content_length, "Content-Length: 0");
//         sprintf(send_buf, "%s %s\r\n%s\r\n", "HTTP/1.1", http_code, content_length);
//         // printf("THING: %s\n", send_buf);
//         write(connfd, send_buf, strlen(send_buf));
//         return;
//     }
//     strcpy(http_type, extracted_string);
//     extracted_string = strtok_r(NULL, " ", &ptr);
//     if(extracted_string == NULL) {
//         printf("test2\n");
//         bzero(http_code, sizeof(http_code));
//         strcpy(http_code, "400 Bad Request");
//         strcpy(content_length, "Content-Length: 0");
//         sprintf(send_buf, "%s %s\r\n%s\r\n", "HTTP/1.1", http_code, content_length);
//         // printf("THING: %s\n", send_buf);
//         write(connfd, send_buf, strlen(send_buf));
//         return;
//     }
//     strcpy(index, extracted_string);
//     extracted_string = strtok_r(NULL, "\r", &ptr);
//     if(extracted_string == NULL) {
//         printf("test3\n");
//         bzero(http_code, sizeof(http_code));
//         strcpy(http_code, "400 Bad Request");
//         strcpy(content_length, "Content-Length: 0");
//         sprintf(send_buf, "%s %s\r\n%s\r\n", "HTTP/1.1", http_code, content_length);
//         // printf("THING: %s\n", send_buf);
//         write(connfd, send_buf, strlen(send_buf));
//         return;
//     }
//     strcpy(http_version, extracted_string);

//     printf("http_type: %s, index: %s, ver: %s\n", http_type, index, http_version);
    
//     //Error Handling for not GET
//     if(strncmp(http_type, "GET", 3)) {
//         printf("test HERE %s\n", http_version);
//         bzero(http_code, sizeof(http_code));
//         strcpy(http_code, "405 Method Not Allowed");
//         return;
//     }

//     //Error Handling for a version other than 1.0 or 1.1
//     if(strncmp(http_version, "HTTP/1.1", 8) && strncmp(http_version, "HTTP/1.0", 8)) {
//         printf("test HERE %s\n", http_version);
//         bzero(http_code, sizeof(http_code));
//         strcpy(http_code, "505 HTTP Version Not Supported");
//         return;
//     }

//     // Test if the requested URL exists
//     //bzero(buf, sizeof(buf));
//     struct hostent *host_info;
//     //readline(connfd, host_buf, MAXLINE);
    
//     extracted_string = strtok_r(first_buf, "\n", &ptr);
//     extracted_string = strtok_r(NULL, "\n", &ptr);
//     strcpy(host_buf, extracted_string);
//     printf("HOST BUF%s\n", host_buf);
//     extracted_string = strtok_r(host_buf, " ", &ptr);
//     if(extracted_string != NULL) {
//         extracted_string = strtok_r(NULL, ":", &ptr);
//         if(extracted_string != NULL) {
//             strcpy(hostname, extracted_string);
//             printf("%s\n", hostname);
//             remove_trailing_whitespace(hostname);
//             remove_crlf(hostname);
//             host_info = gethostbyname(hostname);
//             printf("%s\n", hostname);
//             if(host_info == NULL) {
//                 printf("cannot get host name\n");
//                 bzero(http_code, sizeof(http_code));
//                 strcpy(http_code, "404 Not Found");
//                 strcpy(content_length, "Content-Length: 0");
//                 sprintf(send_buf, "%s %s\r\n%s\r\n", "HTTP/1.1", http_code, content_length);
//             // printf("THING: %s\n", send_buf);
//                 write(connfd, send_buf, strlen(send_buf));  
//                 return;
//             }
//             else {
//                 printf("IP: %s\n", inet_ntoa(*(struct in_addr*)host_info->h_addr_list[0]));
//             }                                                                                                                                                   
//         }                                                                             
//     }
    
//     //Check if the host name or ip is in the block list.
//     file_ptr = fopen("blocklist", "r");
//     char line[MAXLINE];
//     char ip_addr[20];
//     sprintf(ip_addr, "%s", inet_ntoa(*(struct in_addr*)host_info->h_addr_list[0]));
//     if(file_ptr == NULL) {
//         printf("Error opening the blocklist file.\n");
//         return;
//     }
//     //printf("TEST TEST TEST\n");
//     while(fgets(line, sizeof(line), file_ptr) != NULL) {
//         // printf("LINE: %s\n",line);
        
//         // printf("Hostname: %s\n", hostname);
//         // printf("STRLEN: %d", strlen(line));
//         if(!strncmp(line, hostname, strlen(line) - 1)) {
//             printf("Hostname %s found in the blockfile\n", hostname);
//             bzero(http_code, sizeof(http_code));
//             strcpy(http_code, "403 Forbidden");
//             strcpy(content_length, "Content-Length: 0");
//             sprintf(send_buf, "%s %s\r\n%s\r\n", http_version, http_code, content_length);                
//             write(connfd, send_buf, strlen(send_buf));
//             return;
//         }
//         if(!strncmp(line, ip_addr, strlen(line) - 1)) {
//             printf("IP %s found in the blockfile\n", ip_addr);
//             bzero(http_code, sizeof(http_code));
//             strcpy(http_code, "403 Forbidden");
//             strcpy(content_length, "Content-Length: 0");
//             sprintf(send_buf, "%s %s\r\n%s\r\n", http_version, http_code, content_length);                
//             write(connfd, send_buf, strlen(send_buf));
//         return;
//         }
//         bzero(line, sizeof(line));
//     }
//     fclose(file_ptr);

//     //create a socket that will connect.
//     //test to see if the name is in the cache
//     //cache["netsys.cs.colorado.edu"] = 0;

//     //calcualte the hash of index
//     size_t index_hash_num = hash<string>{}(index);
//     printf("INDEX HASH%d\n", index_hash_num);
//     char index_hash[100];
//     sprintf(index_hash, "%d", index_hash_num);
//     printf("test\n");

//     time_t curr_time = std::time(nullptr);
//     double diff;
//     int bytes_readd;
//     sprintf(file_location, "%s%s", cache_location, index_hash);
//     printf("FILE LOCATION %s\n", file_location);
//     bzero(buf, sizeof(buf));
//     char* result = strchr(index, '?');
//     if(cache.count(index_hash) > 0 && result == NULL) {
//         if((diff = difftime( curr_time, *cache[index_hash])) < timeout) {
//             cout << "DIFFERENCE IN TIME: " << diff << endl;
//             printf("FILE IN CACHE GOOD\n");
//             file_ptr = fopen(file_location, "r");
//             while((bytes_readd = fread(buf, sizeof(char), sizeof(buf), file_ptr)) > 0) {
//                 write(connfd, buf, bytes_readd);
//             }
//             return;
//         }
//         else {
//             if(filesystem::exists(file_location)) {
//                 printf("DELETING CACHE FILE\n");
//                 filesystem::remove(file_location);
//             }
//             delete cache[index_hash];
//             cache.erase(index_hash);
//             cout << "EXISTS IN CACHE TIMER RAN OUT" << endl;
//         }
//     }
//     else {
//         cout << "NOT IN CACHE" << endl;
//     }
//     time_t* new_timer = new time_t;
//     //set the timer to the current time.
//     *new_timer = time(nullptr);
//     cache[index_hash] = new_timer;
//     //receive the data from the server
//     int sockfd;
//     struct sockaddr_in server_addr;
//     sockfd = socket(AF_INET, SOCK_STREAM, 0);
//     if (sockfd < 0) {
        
//         perror("Error creating socket");
//         exit(EXIT_FAILURE);
//     }

//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(80);
//     inet_pton(AF_INET, inet_ntoa(*(struct in_addr*)host_info->h_addr_list[0]), &server_addr.sin_addr);

//     if (connect(sockfd, (const sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
//         printf("Error connecting to server\n");
//         close(sockfd);
//         return;
//     } 
//     //printf("Connected to server at %s:%d\n", inet_ntoa(*(struct in_addr*)host_info->h_addr_list[0]), 80);

//     //printf("BUF: %s", first_buf);

//     //send the data to the client.
//     // strcpy(content_length, "Content-Length: 0");
//     bzero(send_buf, sizeof(send_buf));
//     sprintf(send_buf, "%s\r\n", first_buf);
//     if(send(sockfd, http_request, strlen(http_request), 0) < 0) {
//         printf("Error sending to server\n");
//         close(sockfd);
//         return;
//     }

    
//     printf("SENT\n");
//     int bytes_received;
//     char buffer[MAXLINE];
//     file_ptr = fopen(file_location, "w");


//     while ((bytes_received = recv(sockfd, buffer, sizeof(buffer), 0)) > 0) {
//     // Null-terminate received data to use it as a string
//     //printf("test\n");
//     fwrite(buffer, sizeof(byte), bytes_received, file_ptr);
//     // Print received data
//     //printf("RECEIVED DATA:%s\n", buffer);
//     write(connfd, buffer, bytes_received);
//     bzero(buffer, sizeof(buffer));
// }
//     //printf("INDEX: %s\n", index);
    
//     printf("DONE\n");
//     fclose(file_ptr);
//     return;

}
