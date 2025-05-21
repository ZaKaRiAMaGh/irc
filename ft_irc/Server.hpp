
#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <vector>
#include <set>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <signal.h>

#include "CustomException.hpp"
#include "Client.hpp"
#include <poll.h>

class Server{
    private:
        Server& operator=(const Server& other);
        Server(const Server& other);
        Server();
        void                    ServerPrepa();
    public:
        Server(int port, std::string Password);
        ~Server();
        // parametrize constructor

        static bool             Signal;
        int                     ServerSocketFD;
        int                     Port;
        std::string             PassWord;
        std::vector<Client>     Clients;
        std::vector<pollfd>     PollFDs;
        sockaddr_in             SAddress;
        
        // void                    newClientHandler(int cSockerfd);
        void                    newClientHandler(int cSockerfd, sockaddr_in Client__Address);
        void                    sendReply(int cSockfd, std::string message);
        void                    handleNewClient();
        void                    ServerStarts();
        static void                    Signals_handler(int signum);
};


int ParsePort(const std::string& av1);
void PasswordParse(std::string av2);

#endif