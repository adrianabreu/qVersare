#ifndef TCPSERVER_H
#pragma once
#define TCPSERVER_H
#include "socket.h"
#include "helpsignalsthreads.h"
#include <list>
#include <mutex>

/*We have to use the vector threads as global for making each
 * thread able of deleting itself when his job is done.
 */
extern std::list<std::thread> listThreads; //Threads created for listen to clients
extern std::mutex listThreadsMutex;

//History
extern std::queue<Message> historyQueue;
extern std::mutex mutexSignal;
extern std::condition_variable conditionSignal;

/*
 * This class does the basic server stuff,
 * listen on a single port, and handle new connections
 */

class TCPServer : public Socket
{

public:
    TCPServer();
    TCPServer(const std::string& address, int port);
    TCPServer(const TCPServer& older);

    ~TCPServer();

    //This keeps on an accept block and returns the new fd from accept
    int handleConnections(sockaddr_in *remote);
    void pushThread(std::thread& mythread);

    TCPServer& operator=(TCPServer&& older);
};

#endif // TCPSERVER_H
