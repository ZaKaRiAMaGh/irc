
#include "Server.hpp"

bool Server::Signal = false;

Server::Server(int port, std::string Pass): ServerSocketFD(-1), Port(port), PassWord(Pass){
}

void Server::Signals_handler(int signum){
    (void)signum;
	std::cout << std::endl << "Signal Received!" << std::endl;
	Server::Signal = true;
}

void    Server::ServerPrepa(){
    std::memset(&SAddress, 0, sizeof(SAddress));
    SAddress.sin_family = AF_INET;
    SAddress.sin_port = htons(Port);
    SAddress.sin_addr.s_addr = INADDR_ANY;

    ServerSocketFD = socket(PF_INET, SOCK_STREAM, 0);
    if (ServerSocketFD == -1)
        throw CustomException("Failed to create socket\n");
    std:: cout << Port << ' ' << PassWord << std::endl;
    if (bind(ServerSocketFD, (struct sockaddr*)&SAddress, sizeof(SAddress)) < 0)
        throw CustomException("Bind failed\n");

    if (listen(ServerSocketFD, 128) < 0)
        throw CustomException("Listen failed\n");

    pollfd server_pollfd;
    std::memset(&server_pollfd, 0, sizeof(server_pollfd));
    server_pollfd.fd = ServerSocketFD;
    server_pollfd.events = POLLIN;
    PollFDs.push_back(server_pollfd);
}

void    Server::ServerStarts(){
    try{
        ServerPrepa();
    }
    catch(const CustomException& e){
        close(ServerSocketFD);
        std::cout << "Exception Cought in ServerPrepa :" << e.msg();
    }
    
    // Accept connections if server wants to !
    while (!Signal){
        int     status = poll(PollFDs.data(), PollFDs.size(), -1);
        if (status < 0){
            std::cerr << "Poll Failed\n";
            break ;
        }
        for (size_t i = 0; i < PollFDs.size(); ++i) {
            if (PollFDs[i].revents & POLLIN) {
                if (PollFDs[i].fd == ServerSocketFD) {
                    handleNewClient();
                }// else {
                //     handleClientData(PollFDs[i].fd);
                // }
            }
        }
    }
}


void Server::handleNewClient(){
    sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(ServerSocketFD, (sockaddr*)&client_addr, &addr_len);
    if (client_fd < 0) {
        // perror("accept");
        return;
    }
    // Add client socket to poll
    pollfd client_pollfd;
    client_pollfd.fd = client_fd;
    client_pollfd.events = POLLIN;
    PollFDs.push_back(client_pollfd);

    std::cout << "New client connected: " << client_fd << std::endl;
    try{
        newClientHandler(client_fd, client_addr);
    }
    catch(const CustomException& e){
        std::cout << "Catched exeption from handleNewClient() : " << e.msg();
    }
}

Server::~Server(){
    // need to close all sockets && connections between server and clients !
}
#include <sstream> 
std::vector<std::string> split(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
        if (!token.empty() && token[0] == ':') {
            // Everything else is part of realname
            std::string rest;
            std::getline(iss, rest);
            tokens.back() += rest; // append the rest
            break;
        }
    }
    return tokens;
}


void    Server::sendReply(int cSocketfd, std::string message){
    send(cSocketfd, message.c_str(), message.length(), 0);
    close(cSocketfd);
}

// void Server::newClientHandler(int cSocketfd){
void Server::newClientHandler(int cSocketfd, sockaddr_in Client__Address){
    char ClientRecivedData[255];
    bzero(ClientRecivedData, 255);

    ssize_t bytesRead = recv(cSocketfd, ClientRecivedData, 254, 0);
    if (bytesRead <= 0)
        close(cSocketfd);
    std::string receivedPass(ClientRecivedData, bytesRead);
    if (receivedPass.length() > 128){
        sendReply(cSocketfd, "ERROR : Hey Password by rules is not contains alot of chars 128 max\n");
        throw CustomException("Client did write alot of chars.\n");
    }

    if (std::strncmp(ClientRecivedData, "PASS ", 5) != 0){
        sendReply(cSocketfd, "ERROR : First command must be PASS <your_password>\n");
        throw CustomException("Client did not send PASS as first command.\n");
    }
    else {
        receivedPass = receivedPass.substr(5);
    }
    for (size_t i = 0; i < receivedPass.length(); ) {
        if (receivedPass[i] == '\n' || receivedPass[i] == '\r')
            receivedPass.erase(i, 1);
        else
            ++i;
    }

    if (receivedPass != this->PassWord) {
        sendReply(cSocketfd, "ERROR : Wrong password. U're Disconnecting ...\n");
        throw CustomException("Client provided wrong password. Disconnected.\n");
    }


    /// Nick checking here !
    bzero(ClientRecivedData, 255);
    bytesRead = recv(cSocketfd, ClientRecivedData, 254, 0);
    if (bytesRead <= 0)
        close(cSocketfd);
    std::string receivedNick(ClientRecivedData, bytesRead);
    if (std::strncmp(ClientRecivedData, "NICK ", 5) == 0)
        receivedNick = receivedNick.substr(5);
    else {
        sendReply(cSocketfd, "ERROR : Second command after PASS must be NICK <your_nickname>\n");
        throw CustomException("Client did not send NICK as Second command.\n");
    }
    for (size_t i = 0; i < receivedNick.length(); ) {
        if (receivedNick[i] == '\n' || receivedNick[i] == '\r')
            receivedNick.erase(i, 1);
        else
            ++i;
    }
    if (receivedNick.empty() || receivedNick.length() > 30) {
        sendReply(cSocketfd, "ERROR :Invalid nickname length\n");
        throw CustomException("Client sent invalid nickname\n");
    }
    for (size_t i = 0; i < receivedNick.length(); i++ ) {
        if (!std::isalpha(receivedNick[i]) && receivedNick[i] != '-' && receivedNick[i] != '_'){
            sendReply(cSocketfd,  "ERROR :Invalid nickname with alpha and - and _ chars accepted only\n");
            throw CustomException("Client sent invalid nickname (chars invalid)\n");
        }
    }

    // Check uniqueness
    for (size_t i = 0; i < Clients.size(); i++) {
        if (Clients[i].Name == receivedNick) {
            sendReply(cSocketfd, "ERROR :Nickname already in use\n");
            throw CustomException("Duplicate nickname\n");
        }
    }

    // USER cmd parse :
    bzero(ClientRecivedData, 255);
    bytesRead = recv(cSocketfd, ClientRecivedData, 254, 0);
    if (bytesRead <= 0)
        close(cSocketfd);
    std::string receivedUSER(ClientRecivedData, bytesRead);
    if (std::strncmp(ClientRecivedData, "USER ", 5) == 0)
        receivedUSER = receivedUSER.substr(5);
    else {
        sendReply(cSocketfd, "ERROR : third command after NICK must be USER <username> * * : <realname>\n");
        throw CustomException("Client did not send USER as third command.\n");
    }
    if (receivedUSER.empty()){
        sendReply(cSocketfd, "ERROR :Invalid username length\n");
        throw CustomException("Client sent invalid USER\n");
    }
    std::vector<std::string> splited = split(receivedUSER);
    if (splited.size() < 4){
        sendReply(cSocketfd, "ERROR :Invalid username args\n");
        throw CustomException("Client sent invalid USER args\n");
    }
    

    Client first;
    Clients.push_back(first);
    Client& newClient = Clients.back();
    newClient.ClientSocketfd = cSocketfd;
    newClient.ClientAddress = Client__Address;
    newClient.Name = receivedNick;
    newClient.Pass = receivedPass;
    newClient.realName = splited[3];
    newClient.UserName = splited[0];
    newClient.hasPass = true;
    std::cout << "Client :: " << newClient.Name
                << " with UserName : " << newClient.UserName 
                << " with Socketfd : " << newClient.ClientSocketfd
                << " with realName : " << newClient.realName
                << " with Pass : " << newClient.Pass
                << " \n";
    // multi clients left
}