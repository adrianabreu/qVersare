#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>

#include <cstdio>
#include <cerrno>
#include <cstring>
#include <system_error>

// Messages structure
struct Message {
    char username[16];
    char text[1024];
    char time[26]; //Www Mmm dd hh:mm:ss yyyy
};

//Helper function for building sockaddr_in struct
sockaddr_in makeIpAddress(const std::string& ip_address, int port);

class Socket
{

protected:
    int fd_; //Socket
    void normalSocket(const sockaddr_in& address); //Helper function

public:
    Socket(); //Default constructor with invalid fd
    Socket(int tempfd); //Constructor for server using the fd from accept
    Socket(const std::string& localIpaddress,const std::string& remoteIpaddress,
           int remotePort); //Normal constructor based on ip
    Socket(const sockaddr_in& local, const sockaddr_in& remote);
    Socket(const Socket&);

    ~Socket();

    int getFd() const;
    void setFd(int newFd);

    void sendTo(const Message& message);
    void receiveFrom(Message& message);

    Socket& operator=(Socket&& older);
};

#endif // SOCKET_H
