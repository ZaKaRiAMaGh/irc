
#ifndef SERVER_HPP
#define SERVER_HPP

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <vector>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

#include "CustomException.hpp"

class Server{
    private:
        Server& operator=(const Server& other);
        Server(const Server& other);
        Server();

    public:
        Server(int port, std::string Password);
        ~Server();
        // parametrize constructor


        void                newClientHandler(int cSockerfd);
        int                 ServerSocketFD;
        int                 Port;
        std::string         PassWord;
        std::vector<int>    Clients;
        sockaddr_in         SAddress;

};


int ParsePort(const std::string& av1);
void PasswordParse(std::string av2);

#endif