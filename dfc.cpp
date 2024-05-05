#include "dfc.h"

#define SERVER_IP "127.0.0.1"
using namespace std;

string command;

int main(int argc, char **argv) {

    
    int sockfd;
    char message[] = "get";
    struct sockaddr_in server_addr;
    vector<server_t> servers;


    if(argc <= 2) {
        cout << argv[0] << " <command> [filename] ... [filename]" << endl;
        return -1;
    }

    command = argv[1];

    ifstream file("dfc.conf");
    if (!file.is_open()) {
        cout << "Error opening file." << endl;
        return -1;
    }


    string line;
    while (getline(file, line)) {
        
        istringstream iss(line);
        string name, ipPort, type;
        if (iss >> type >> name >> ipPort) {
            size_t pos = ipPort.find(':');
            
            if (pos != string::npos) {
                string ip = ipPort.substr(0, pos);
                uint16_t port = stoi(ipPort.substr(pos + 1));
                servers.push_back({name, ip, port, false});
            }
        }
    }

    //test connections to all of the servers. 

    for(int i = 0; i < servers.size(); i++) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            exit(EXIT_FAILURE);
        }
        // Setup server address
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(servers[i].port);
        
        if (inet_pton(AF_INET,  servers[i].ip.c_str(), &server_addr.sin_addr) <= 0) {
            perror("Invalid address / Address not supported");
            close(sockfd);
            continue;
        }
        
        // Connect to server
        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            cout << i << endl;
            //servers.erase(servers.begin() + i);
            close(sockfd);
            continue;
        }
        servers[i].available = true;
        close(sockfd);
            
    }

    if(command == "ls") {
        ls();
    }
    else if(command == "get") {
        for(int i = 2; i < argc; i++) {
            get(argv[i], servers);
        }
    }
    else if(command == "put") {
        for(int i = 2; i < argc; i++) {
            put(argv[i], servers);
        }
    }
    return 0;

    cout << "BREAK HERE" << endl;

    for(int i = 0, j = servers.size(); i < j; i++) {
        if(servers[i].available == false) servers.erase(servers.begin() + i);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }
    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(servers[0].port);
    
    if (inet_pton(AF_INET,  servers[0].ip.c_str(), &server_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        close(sockfd);

    }
    
    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        //cout << i << endl;
        //servers.erase(servers.begin() + i);
        close(sockfd);

    }


    char send_buf[MAXLINE];
    FILE* file_ptr = fopen("test.txt", "r");
    strcpy(send_buf,"put test.txt\n");

    send(sockfd, send_buf, strlen(send_buf), 0);
    bzero(send_buf, sizeof(send_buf));
    int bytes;
    while((bytes = fread(send_buf, sizeof(byte), sizeof(send_buf), file_ptr)) > 0) {
        cout << send_buf << endl;
        cout << bytes << endl;
        send(sockfd, send_buf, bytes, 0);
        bzero(send_buf, sizeof(send_buf));
    }

    return 0;
}

void ls() {
    return;
}

void put(string filename, vector<server_t> &servers) {
    int sockfd1, sockfd2;
    char send_buf[MAXBUF];
    char send_filename[30];
    struct sockaddr_in server_addr, server_addr2;
    int bytes_read;
    FILE* file = fopen(filename, "b");
    if(file == NULL) {
        cout << "Error opening file." << endl;
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    //Amount of bytes to send per chunk.
    long bytes_per_chunk = file_size / servers.size();
    //Move the pointer back to the beginning.
    fseek(file, 0, SEEK_SET);

    cout << "File Size: " << file_size;
    hash<string> hasher;
    int x = hasher(filename) % servers.size();

    for(int i = 0; i < servers.size(); i++) {
        
        // int server2 = (server + 1) % servers.size();
        // 
        int server = (i + x) % servers.size();
        int server2 = (i + x + 1) % servers.size();
        cout << "Chunk " << i << " going to server " << server << endl;
        //Creating the socket
        if ((sockfd1 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            exit(EXIT_FAILURE);
        }
        // Setup server address
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(servers[server].port);
        
        //Setting the ip dest.
        if (inet_pton(AF_INET,  servers[server].ip.c_str(), &server_addr.sin_addr) <= 0) {
            perror("Invalid address / Address not supported");
            close(sockfd1);

        }
        
        // Connect to server
        if (connect(sockfd1, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            close(sockfd1);
        } 

        if ((sockfd2 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            exit(EXIT_FAILURE);
        }
        // Setup server address
        server_addr2.sin_family = AF_INET;
        server_addr2.sin_port = htons(servers[server2].port);
        
        //Setting the ip dest.
        if (inet_pton(AF_INET,  servers[server2].ip.c_str(), &server_addr2.sin_addr) <= 0) {
            perror("Invalid address / Address not supported");
            close(sockfd2);

        }
        
        // Connect to server
        if (connect(sockfd2, (struct sockaddr *)&server_addr2, sizeof(server_addr2)) < 0) {
            perror("Connection failed");
            close(sockfd2);
        } 


        sprintf(send_filename, "%s_%d", filename, i);
        sprintf(send_buf, "put %s\n");
        send(sockfd1, send_buf, strlen(send_buf), 0);
        send(sockfd2, send_buf, strlen(send_buf), 0);
        bzero(send_buf, sizeof(send_buf));

        int curr_chunk_size = bytes_per_chunk;
        int bytes_to_read;

        if(sizeof(send_buf) > curr_chunk_size) {
            bytes_to_read = sizeof(send_buf);
        } else {
            bytes_to_read = curr_chunk_size;
        }
        while((bytes_read = fread(send_buf, sizeof(char), bytes_to_read, file)) > 0) {
            send(sockfd1, send_buf, bytes_read, 0);
            send(sockfd2, send_buf, bytes_read, 0);   

            curr_chunk_size -= bytes_to_read;
            if(curr_chunk_size == 0) break;
            //setting the amount of bytes to read from the file.
            if(sizeof(send_buf) > curr_chunk_size) {
                bytes_to_read = sizeof(send_buf);
            } else {
                bytes_to_read = curr_chunk_size;
            }
        }


        close(sockfd1);
        close(sockfd2);
    }


}

void get(string filename, vector<server_t> &servers) {
    return;
}