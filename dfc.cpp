#include "dfc.h"

#define SERVER_IP "127.0.0.1"
using namespace std;

string command;

int main(int argc, char **argv) {

    
    int sockfd;
    char message[] = "get";
    struct sockaddr_in server_addr;
    vector<server_t> servers;


    if(argc < 2) {
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
        ls(servers);
    }
    else if(command == "get") {
        for(int i = 2; i < argc; i++) {
            get(argv[i], servers);
        }
    }
    else if(command == "put") {
        for(int i = 2; i < argc; i++) {
                for(int i = 0, j = servers.size(); i < j; i++) {
                if(servers[i].available == false) servers.erase(servers.begin() + i);
    }
            put(argv[i], servers);
        }
    }
    return 0;

    cout << "BREAK HERE" << endl;

    

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

void ls(vector<server_t> &servers) {
    vector<dir_list> dirs;
    int sockfd;
    struct sockaddr_in server_addr;
    char recv_buf[MAXLINE];
    char temp_buf[MAXLINE];
    char* extracted_string, *ptr;
    for(int i = 0; i < servers.size(); i++) {
        cout << "going to server " << i << endl;
        //Creating the socket
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            exit(EXIT_FAILURE);
        }
        // Setup server address
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(servers[i].port);
        
        //Setting the ip dest.
        if (inet_pton(AF_INET,  servers[i].ip.c_str(), &server_addr.sin_addr) <= 0) {
            perror("Invalid address / Address not supported");
            close(sockfd);

        }
        
        // Connect to server
        if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            close(sockfd);
        } 

        send(sockfd, "ls t\n", strlen("ls t\n"), 0);
        recv(sockfd, recv_buf, sizeof(recv_buf), 0);
        extracted_string = strtok_r(recv_buf, "\n", &ptr);
        // cout << recv_buf << endl;
        //cout << extracted_string << endl;

   

        string extractedFilename;
        while((extracted_string = strtok_r(NULL, "\n", &ptr)) != NULL) {
            //cout << extracted_string << endl;
            string file = extracted_string;
            string::size_type underscorePos = file.find('_');

            if (underscorePos != std::string::npos) {
                extractedFilename = file.substr(0, underscorePos);
                //cout << "Extracted filename: " << extractedFilename << endl;
            } else {
                cout << "Underscore not found in the filename." << endl;
            }
            
            //add it to the front list.
            if(dirs.size() == 0) {
                dir_list temp_dir;
                temp_dir.name = "FIRST";
                temp_dir.list.push_back("first");
                dirs.push_back(temp_dir);
            }
            for(int j = 0; j < dirs.size(); j++) {
                ///cout << "TEST" << endl;
                if(dirs[j].name == extractedFilename) {
                    
                    break;
                }
                if(j == (dirs.size() - 1)) {
                    
                    // dir_list temp_dir;
                    // temp_dir.name = extractedFilename;
                    dirs.push_back({extractedFilename, {"first"}});
                }            
            }
            for(int j = 0; j < dirs.size(); j++) {
                //cout << "in: " << dirs[j].name << endl;
                if(extractedFilename != dirs[j].name) continue;
                for(int k = 0; k < dirs[j].list.size(); k++) {
                    //cout << "looking at: " << dirs[j].list[k] << endl;
                    //cout << "file: " << file << endl;
                    if((dirs[j].list)[k] == file) {
                        break;
                    }
                    if(k == (dirs[j].list.size() - 1)) {
                        dirs[j].list.push_back(file);
                    }
                }
            }

        }
        

        close(sockfd);

    }
    // cout << "TEST" << endl;
    dirs.erase(dirs.begin());
    for(int i = 0; i < dirs.size(); i++) {
        dirs[i].list.erase(dirs[i].list.begin());
    }
    for(int i = 0; i < dirs.size(); i++) {
        cout << "TITLE ";
        
        cout << dirs[i].name;
        if(dirs[i].list.size() == servers.size()) {
            cout << ": " << "[Complete]" << endl;
        } else {
            cout << ": " << "[Incomplete]" << endl;
        }
        
        for (string element : dirs[i].list) {
        //std::cout << element << std::endl;
    }
    }


    return;
}

void put(string filename, vector<server_t> &servers) {
    int sockfd1, sockfd2;
    char send_buf[MAXBUF];
    char send_filename[30];

    struct sockaddr_in server_addr, server_addr2;
    int bytes_read;
    cout << filename << endl;
    FILE* file = fopen(filename.c_str(), "r");
    if(file == NULL) {
        cout << "Error opening file." << endl;
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    //Amount of bytes to send per chunk.
    long bytes_per_chunk = file_size / servers.size();
    //cout << "NUM SERVERS: " << servers.size();
    //Move the pointer back to the beginning.
    fseek(file, 0, SEEK_SET);

    //cout << "File Size: " << file_size << endl;
    //cout << "Bytes per Chunk: " << bytes_per_chunk << endl;
    hash<string> hasher;
    int x = hasher(filename) % servers.size();

    for(int i = 0; i < servers.size(); i++) {
        
        // int server2 = (server + 1) % servers.size();
        // 
        int server = (i + x) % servers.size();
        int server2 = (i + x + 1) % servers.size();
        //cout << "Chunk " << i << " going to server " << server << endl;
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

        
        sprintf(send_filename, "%s_%d", filename.c_str(), i);
        sprintf(send_buf, "put %s\n", send_filename);
        send(sockfd1, send_buf, strlen(send_buf), 0);
        send(sockfd2, send_buf, strlen(send_buf), 0);
        bzero(send_buf, sizeof(send_buf));

        int curr_chunk_size = bytes_per_chunk;
        int bytes_to_read;

        if(sizeof(send_buf) < curr_chunk_size) {
            bytes_to_read = sizeof(send_buf);
        } else {
            bytes_to_read = curr_chunk_size;
        }

        //cout << "BYTES BEING READ: " << bytes_to_read << endl;
        while((bytes_read = fread(send_buf, sizeof(char), bytes_to_read, file)) > 0) {
            send(sockfd1, send_buf, bytes_read, 0);
            send(sockfd2, send_buf, bytes_read, 0);   
            //cout << "BYTES READ AND WRITTEN: " << bytes_read << endl;

            curr_chunk_size -= bytes_to_read;
           //cout << "CHUNK LEFT: " << curr_chunk_size << endl;
            //cout << "FILE POINTER: " << file->_offset << endl;
            if(curr_chunk_size <= 0) break;
            
            //setting the amount of bytes to read from the file.
            if(sizeof(send_buf) < curr_chunk_size) {
                bytes_to_read = sizeof(send_buf);
            } else {
                if(i == servers.size() - 1) bytes_to_read = curr_chunk_size + (file_size % servers.size());
                else bytes_to_read = curr_chunk_size;
            }
        }


        close(sockfd1);
        close(sockfd2);
    }
    fclose(file);  
    cout << "[SUCCESS] File Written." << endl;

}

void get(string filename, vector<server_t> &servers) {

    mkdir("./cache", 0777);

    char send_buf[MAXLINE];
    char desired_file[40];
    int sockfd;
    int rec, read_bytes;
    struct sockaddr_in server_addr;

    FILE* file_dest = fopen(filename.c_str(), "w");
    for(int i = 0; i < servers.size(); i++) {
        sprintf(desired_file, "./cache/%s_%d", filename.c_str(), i);
        sprintf(send_buf, "get %s_%d\n", filename.c_str(), i);
        //cout << "DESIRED FILE: " << send_buf << endl;

        
        for(int j = 0; j < servers.size(); j++) {
            //Creating the socket
            //cout << "ITERATION: " << j << endl;;
            //cout << servers.size() << endl;
            if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
                perror("Socket creation error");
                exit(EXIT_FAILURE);
            }
            // Setup server address
            server_addr.sin_family = AF_INET;
            server_addr.sin_port = htons(servers[j].port);
            //cout << servers[j].port << endl;
            //Setting the ip dest.
            if (inet_pton(AF_INET,  servers[j].ip.c_str(), &server_addr.sin_addr) <= 0) {
                perror("Invalid address / Address not supported");
                close(sockfd);

            }
            
            // Connect to server
            if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
                perror("Connection failed");
                continue;
                close(sockfd);
            } 
            sprintf(send_buf, "get %s_%d\n", filename.c_str(), i);
            send(sockfd, send_buf, strlen(send_buf), 0);
            bzero(send_buf, sizeof(send_buf));
            //cout << "HERE1" << endl;
            rec = recv(sockfd, send_buf, sizeof(send_buf), 0);
            //bzero(send_buf, sizeof(send_buf));
            //cout << "HERE" << endl;
            //cout << send_buf << endl;
            if(!strncmp(send_buf, "yes", 3)) {
                //cout << "YES" << endl;
                bzero(send_buf, sizeof(send_buf));
                FILE* file = fopen(desired_file, "w");
                while((rec = recv(sockfd, send_buf, sizeof(send_buf), 0)) > 0) {
                    
                    //cout << "WRITING" << endl;
                    fwrite(send_buf, sizeof(char), rec, file);
                    bzero(send_buf, sizeof(send_buf));
                }
                fclose(file);
                break;
            }

            close(sockfd);

        }
        
        DIR *dir = opendir("./cache");
        int count = 0;
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL) {
            if (entry->d_type == DT_REG) { // Check if the entry is a regular file
                count++;
            }
        }
        //cout << count << endl;
        if(count == 0) {
            cout << "File cannot be completed." << endl;
            return;
        }
        //cout << "TEST" << endl;
        FILE* temp_file = fopen(desired_file, "r");
        bzero(send_buf, sizeof(send_buf));
        while((read_bytes = fread(send_buf, sizeof(char), sizeof(send_buf), temp_file)) > 0) {
            fwrite(send_buf, sizeof(char), read_bytes, file_dest);
        }
        fclose(temp_file);
    }
    fclose(file_dest);
    for (const auto& entry : std::filesystem::directory_iterator("./cache")) {
        
            std::filesystem::remove(entry.path()); 
    }
    return;
}