#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "Server.hpp" 

class client{
    private:
        std::string Name;
        std::string Pass;
        int         isRegistered;

        client(const client& other);
        client& operator=(const client& other);

    public:
        client(); // default constructor;
        // client() //parameterize
        ~client();

        // getters :
        std::string& getName() const;
        std::string& getPass() const;
        int          getisRegistered() const;

        // setters :
        void         setName(std::string newName);
        void         setPass(std::string newPass);
        void         setRegistered();

};

#endif