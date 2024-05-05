#include "dfc.h"

#define SERVER_IP "127.0.0.1"

int main(int argc, char **argv) {

    
    int sockfd;
    char message[] = "get";
    struct sockaddr_in server_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }
    
    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(10001);
    
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        exit(EXIT_FAILURE);
    }
    
    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    
    // Send data to server
    if (send(sockfd, message, strlen(message), 0) < 0) {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
    
    close(sockfd);
    return 0;
}