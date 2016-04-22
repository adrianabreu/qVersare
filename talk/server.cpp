#include "server.h"
#include <mutex>
#include <thread>
#include <list>
#include <map>

std::map<std::thread::id,Socket> hashSockets; //List of sockets clients,
std::mutex hashSocketsMutex;

std::list<std::thread> listThreads; //Threads created for listen to clients
std::mutex listThreadsMutex;

TCPServer server::setupServer(const std::string& ipLocal, int port, int* aux)
{
    TCPServer local;

    try {
         local = TCPServer(ipLocal,port);
    }catch (std::system_error& e) {
        if (errno == ENOBUFS || errno == ENOMEM) {
            std::cerr << program_invocation_name
                      << ": Not enough memory for creating Socket "
                      << std::endl;
        } else {
            std::cerr << program_invocation_name << ": " << e.what()
            << std::endl;
        }

        *aux = ERR_SOCKET; //Program won't finish with 0
    }
    return local;
}

void server::startServer(TCPServer *local,const std::string& userName)
{
    //We have to block the signals on the children
    try {
        setSigMask(SIG_BLOCK);
    } catch (std::system_error& e) {
        std::cerr << program_invocation_name << ": " << e.what()
        << std::endl;
    }

    //History
    std::thread createHistory(&queueThread, std::ref(userName));

    //We will launch a thread for listen for connections
    std::thread listener(&server::listenThread, std::ref(local));

    //We will now launch a thread for getting user input
    std::thread sender(&server::sendThread, std::ref(endOfLoop),
                      std::ref(userName));

    //Now we unblock the signals on the main thread
    try {
        setSigMask(SIG_UNBLOCK);
    } catch (std::system_error& e) {
        std::cerr << program_invocation_name << ": " << e.what()
        << std::endl;
    }

    while(!endOfLoop)
        usleep(25000);

    //We must close all the sockets and finish all the threads
    requestCancellation(listener);
    requestCancellation(sender);
    requestCancellation(createHistory);
    server::clearListThreads();
}


void server::getandSendMessage(std::atomic<bool>& endOfLoop,
                               const std::string& userName)
{
    Message message;
    std::string message_text;

    while (! endOfLoop) {
        std::getline(std::cin,message_text);

        if(message_text == "/quit" || std::cin.eof())
            endOfLoop = true;

        if (!endOfLoop) {
            userName.copy(message.username,sizeof(message.username) - 1 ,0);
            message.username[userName.length()] = '\0';
            message_text.copy(message.text, sizeof(message.text) - 1, 0);
            message.text[message_text.length()] = '\0';
            std::time_t result = std::time(nullptr);
            std::strftime(message.time, sizeof(message.time), "%D %T",
                          std::localtime(&result));

            //add to history
            std::unique_lock<std::mutex> lockSignal(mutexSignal);
            historyQueue.push(message);
            lockSignal.unlock();
            conditionSignal.notify_one();

            std::unique_lock<std::mutex> lock(hashSocketsMutex);
            server::sendAll(message,std::this_thread::get_id());
            lock.unlock();

        }
    }
}

void server::sendThread(std::atomic<bool>& endOfLoop,
                         const std::string& userName)
{
    try {
        getandSendMessage(endOfLoop,userName);
    } catch (std::system_error& e) {
        std::cerr << program_invocation_name << ": " << e.what()
        << std::endl;
        endOfLoop = true;
    }

}


void server::listenThread(TCPServer *local)
{
    while(!endOfLoop) {
        try {
            sockaddr_in sinRemote = {};
            int tempfd = local->handleConnections(&sinRemote);
            auto mythread = std::thread(&threadReceive,std::ref(tempfd));
            local->pushThread(mythread);

        } catch (std::system_error& e) {
            std::cerr << program_invocation_name << ": " << e.what()
            << std::endl;
        }

    }
}

void server::threadReceive(int& tempfd)
{
    try {
        //lock resource - create socket - unlock resource - read socket!
        std::unique_lock<std::mutex> lock(hashSocketsMutex);
        hashSockets[std::this_thread::get_id()] = Socket(tempfd);
        lock.unlock();
        server::receiveAndShowMessage();
    } catch (std::system_error& e) {
        //Probably connection over
        std::unique_lock<std::mutex> lock(hashSocketsMutex);
        hashSockets.erase(std::this_thread::get_id());
        lock.unlock();

        std::unique_lock<std::mutex> lock2(listThreadsMutex);
        auto this_iterator = listThreads.begin();
        for (auto it1 = listThreads.begin();
             it1 != listThreads.end(); ++it1) {
            if (it1->get_id() == std::this_thread::get_id()) {
                auto auxThread = std::move(*it1);
                auxThread.detach();
                this_iterator = it1;
            }
        }
        listThreads.erase(this_iterator);
        lock2.unlock();
    }
}


void server::receiveAndShowMessage()
{
    Message message;

    std::unique_lock<std::mutex> lock(hashSocketsMutex);
    Socket socket = hashSockets[std::this_thread::get_id()];
    lock.unlock();

    while(1) {
            socket.receiveFrom(message);
            message.username[15]= '\0';
            message.text[1023] = '\0';
            message.time[25] = '\0';

            //add to history
            std::unique_lock<std::mutex> lockSignal(mutexSignal);
            historyQueue.push(message);
            lockSignal.unlock();
            conditionSignal.notify_one();

            std::unique_lock<std::mutex> lock(hashSocketsMutex);
            std::cout << message.time << " " << message.username << " sent: '"
                      << message.text << "'" << std::endl;
            server::sendAll(message,std::this_thread::get_id());
            lock.unlock();
            //%d %T
    }

}

void server::sendAll(const Message& message, std::thread::id senderId)
{
    //The resources have been locked before, so we can send now
    for(auto &it1 : hashSockets) {
        if (it1.first != senderId) {
            try {
                it1.second.sendTo(message);
            } catch (std::system_error& e) {
                std::cerr << program_invocation_name << ": " << e.what()
                << std::endl;
            }
        }
    }

}

void server::clearListThreads()
{
    try {
        std::unique_lock<std::mutex> lock(listThreadsMutex);
        //All the threads we have created must be destroyed
        for(auto &it1 : listThreads)
            requestCancellation(it1);
        lock.unlock();
    } catch(std::system_error &e) {

    }
}
