
#include "Server.hpp"

Server::Server(int port, std::string Pass): Port(port), PassWord(Pass){
    std::memset(&SAddress, 0, sizeof(SAddress));
    SAddress.sin_family = AF_INET;
    SAddress.sin_port = htons(Port);
    SAddress.sin_addr.s_addr = INADDR_ANY;

        // socket creation
    ServerSocketFD = socket(PF_INET, SOCK_STREAM, 0);
    if (ServerSocketFD == -1)
        throw CustomException("Failed to create socket\n");
    std:: cout << Port << ' ' << PassWord << std::endl;

        // linking socket to internet !
    if (bind(ServerSocketFD, (struct sockaddr*)&SAddress, sizeof(SAddress)) < 0)
        throw CustomException("Bind failed\n");

    // Listen to network and  waiting for expected connections
    if (listen(ServerSocketFD, 128) < 0)
        throw CustomException("Listen failed\n");

    // Accept connections if server wants to !
    while (1){
        int clientfd;
        sockaddr_in CAddress;
    
        
        socklen_t addr_size = sizeof(SAddress);
        try {
            clientfd = accept(ServerSocketFD, (struct sockaddr*)&CAddress, &addr_size);
            if (clientfd < 0) 
                throw CustomException("Accept failed\n");
            
        }
        catch(const CustomException& e){
            std::cout << "Server Running Warnings : " << e.msg();
            continue;
        }

        std::cout << "Client connected!\n";
        std::cout << "New client connected: " << inet_ntoa(CAddress.sin_addr) << "\n";

        // Optional: send welcome message
        const char* msg = "Welcome to IRC Server!\n";
        send(clientfd, msg, strlen(msg), 0);

        // close(clientfd);
        // close(ServerSocketfd);
    }
}

Server::~Server(){
    // need to close all sockets && connections between server and clients !
}

void Server::newClientHandler(int cSocketfd){
    
}