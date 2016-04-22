#include "socket.h"
#include "helpsignalsthreads.h"
#include <ctime>

sockaddr_in makeIpAddress(const std::string& ip_address, int port){
    sockaddr_in tmp{};

    tmp.sin_family = AF_INET;
    const char * c = ip_address.c_str();
    inet_aton(c,&tmp.sin_addr);
    tmp.sin_port = htons(port);

    return tmp;
}

void Socket::normalSocket(const sockaddr_in& address)
{
    // Create local socket
    fd_ = socket(AF_INET, SOCK_STREAM, 0);

    if ( fd_ < 0 )
        throw std::system_error(errno, std::system_category(),
                                "it wasn't possible to create the socket");

    //Bind the socket to a known direction
    int result = bind(fd_, reinterpret_cast<const sockaddr*>(&address),
                      sizeof(address));

    if ( result < 0 )
        throw std::system_error(errno, std::system_category(),"bind error: ");

}

Socket::Socket()
{
    //We have to initialize the file descriptor to a non valid value
    fd_ = -1;
}

Socket::Socket(int tempfd)
{
    fd_ = tempfd;
}

Socket::Socket(const std::string& localIpAddress,
               const std::string& remoteIpAddress, int remotePort)
{
    Socket::normalSocket(makeIpAddress(localIpAddress, 0));
    sockaddr_in remote = makeIpAddress(remoteIpAddress, remotePort);
    int result = connect(fd_,reinterpret_cast<const sockaddr*>(&remote),
                         sizeof(remote));

    if (result < 0) {
        std::cout << "Couldn't connect to remote host " << remoteIpAddress
                  << std::endl;
    } else {
        std::cout << "Connected to " << inet_ntoa(remote.sin_addr)
                  << std::endl;
    }
}


Socket::Socket(const Socket& older)
{
    fd_=older.getFd();
}

Socket::~Socket()
{
    //We just need to close the file descriptor for end connection
    close(fd_);
}

int Socket::getFd() const
{
    return fd_;
}

void Socket::setFd(int newFd)
{
    fd_ = newFd;
}

void Socket::sendTo(const Message& message)
{
    int result = write(fd_, static_cast<const void*>(&message),
                       sizeof(message));

    if ( result < 0 ) {
        std::cout << "Your partner has diconnected" << std::endl;
        throw std::system_error(errno, std::system_category(), "write error: ");
    }
}

void Socket::receiveFrom(Message& message)
{
    //Has to be int for errors
    int result = 0;
    while(result < sizeof(message)) { //For dealing with incompleted messages
        result = read(fd_,static_cast<void*>(&message), sizeof(message));

        if (result < 0) {
            throw std::system_error(errno, std::system_category(),
                                    "read error: ");
        } else if (result == 0) {
            //If read = 0, socket was closed
            throw std::system_error(errno, std::system_category(),
                                    "Connection was over: ");
        }
    }
}

Socket& Socket::operator=(Socket&& older)
{
    fd_=older.getFd();
    older.setFd(-1);

    return *this;
}
