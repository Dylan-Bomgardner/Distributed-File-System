#include "dfc.h"

#define SERVER_IP "127.0.0.1"
using namespace std;

int main(int argc, char **argv) {

    
    int sockfd;
    char message[] = "get";
    struct sockaddr_in server_addr;
    vector<server_t> servers;


    if(argc != )

    ifstream file("dfc.conf");
    if (!file.is_open()) {
        cout << "Error opening file." << endl;
        return 1;
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

    cout << "BREAK HERE" << endl;

    for(int i = 0, j = servers.size(); i < j; i++) {
        if(servers[i].available == false) servers.erase(servers.begin() + i);
    }

    char[MAXLINE] send_buf;

    return 0;
}