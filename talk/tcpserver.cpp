#include "tcpserver.h"

TCPServer::TCPServer() {}

TCPServer::TCPServer(const TCPServer& older) : Socket()
{
    //If you do not call socket, a warning is launched
    fd_ = older.getFd();
}

TCPServer::~TCPServer()
{
    //We have to close our socket
    close(fd_);
}

TCPServer::TCPServer(const std::string& ip_address, int port)
{
    sockaddr_in address = makeIpAddress(ip_address, port);
    normalSocket(address);
    int result = listen(fd_, 5);

    if (result < 0) {
        throw std::system_error(errno, std::system_category(),
                                "listen error");
    } else {
        socklen_t add_len = sizeof(address);
        result = getsockname(fd_,reinterpret_cast<sockaddr*>(&address),
                &add_len);
        if (result == 0)
            std::cout << "Entering server mode... Listening on " <<
                      ntohs(address.sin_port) << std::endl;
        else
            throw std::system_error(errno, std::system_category(),
                                    "couldn't get listen port info");
    }
}

int TCPServer::handleConnections(sockaddr_in *remote)
{
    int aux = -1; // no valid fd
    socklen_t ssize = sizeof(*remote);
    int tempfd = accept(fd_,reinterpret_cast<sockaddr*>(remote),&ssize);

    if (tempfd < 0) {
        throw std::system_error(errno, std::system_category(),
                                " Error on accept");
    } else {
        aux = tempfd;
    }
    return aux; //We return the new socket
}

void TCPServer::pushThread(std::thread& mythread)
{
    //We have to lock the resource for if a thread it's deleting itself
    //Calling functions will handle exceptions from mutex lock
    std::unique_lock<std::mutex> lock(listThreadsMutex);
    listThreads.push_back(std::move(mythread));
    lock.unlock();

}

TCPServer& TCPServer::operator =(TCPServer&& older)
{
    fd_= older.getFd();
    older.setFd(-1);

    return *this;
}
