#ifndef SERVER_H
#define SERVER_H
#include "tcpserver.h"


/*
 * These are helper functions for setting up the
 * server and launch the new threads
 * also it controls the sendall, all servers have
 * 3 + n thredas
 * main + listen + input + n * clientsthreads
 */

namespace server {

    TCPServer setupServer(const std::string& ipAdress, int port, int* aux);
    void startServer(TCPServer *local,const std::string& userName);

    //The send thread gets the message from input and send it
    void getandSendMessage(std::atomic<bool>& endOfLoop,
                           const std::string& userName);
    void sendThread(std::atomic<bool>& endOfLoop,
                     const std::string& userName);

    // Launches a thread that will loop on accepting new connections
    void listenThread(TCPServer* local);

    void threadReceive(int& tempfd); //function for each children thread
    void receiveAndShowMessage();

    // Messages should be sent to everyone except the sender
    void sendAll(const Message& message, std::thread::id senderId);

    void clearListThreads(); //For cancelling all threads
}
#endif // SERVER_H
