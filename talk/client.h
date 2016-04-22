#ifndef CLIENT_H
#define CLIENT_H
#include "socket.h"
#include "helpsignalsthreads.h"

extern std::queue<Message> historyQueue;
extern std::mutex mutexSignal;
extern std::condition_variable conditionSignal;

/*
 * These are helper functions for the client setup
 * and threads launched
 */
namespace client {
    //If the newadd socket is not created correctly, aux will indicate it
    Socket setupSocket(const std::string& sinLocal,const std::string& sinRemote,
                       int port, int *aux);
    void startClient(Socket *local, const std::string& userName);


    //The send thread gets the message from input and send it
    void getandSendMessage(Socket *local,std::atomic<bool>& endOfLoop,
                           const std::string& userName);
    void sendThread(Socket& local,std::atomic<bool>& endOfLoop,
                    const std::string& userName);

    //The recv thread gets the message from socket and displays it
    void receiveAndShowMessage(Socket *socket);
    void recvThread(Socket& local,std::atomic<bool>& endOfLoop);
}

#endif // CLIENT_H
