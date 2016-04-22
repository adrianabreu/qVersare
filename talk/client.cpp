#include "client.h"
#include "helpsignalsthreads.h"
#define ERR_SOCKET 3



/*===================================================
 * Thread's domain
 * ==================================================
 */
void client::getandSendMessage(Socket *local,std::atomic<bool>& endOfLoop,
                               const std::string& userName)
{
    //We should ead until read quit or eof
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
            local->sendTo(message);

            //add to history
            std::unique_lock<std::mutex> lockSignal(mutexSignal);
            historyQueue.push(message);
            lockSignal.unlock();
            conditionSignal.notify_one();
        }
    }

}

void client::sendThread(Socket& local,std::atomic<bool>& endOfLoop,
                        const std::string& userName)
{
    try {
        getandSendMessage(&local,endOfLoop,userName);
    } catch (std::system_error& e) {
        std::cerr << program_invocation_name << ": " << e.what()
        << std::endl;
        endOfLoop = true;
    }

}
//=======================================================================

void client::receiveAndShowMessage(Socket *socket)
{
    Message message;
    while(1) {
        socket->receiveFrom(message);

        //add to history
        std::unique_lock<std::mutex> lockSignal(mutexSignal);
        historyQueue.push(message);
        lockSignal.unlock();
        conditionSignal.notify_one();

        /* In order to increase the security we add a \0 to limit
         * the lecture to the size of the buffer, maybe the message
         * its not delimited by default.
         */
        message.username[15]= '\0';
        message.text[1023] = '\0';
        message.time[25] = '\0';
        std::cout << message.time << " " << message.username << " sent: '"
                  << message.text << "'" << std::endl;
    }

}

void client::recvThread(Socket& local, std::atomic<bool>& endOfLoop)
{
    try {
        client::receiveAndShowMessage(&local);
    } catch (std::system_error& e) {
        std::cout << "Connection was over" << std::endl;
        endOfLoop = true;
    }

}

/*===================================================
 * End of thread's domain
 * ==================================================
 */


Socket client::setupSocket(const std::string& ipLocal,
                           const std::string& ipRemote, int port, int* aux)
{
    Socket local;
    try {
         local = Socket(ipLocal,ipRemote,port);

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


void client::startClient(Socket *local,const std::string& userName)
{
    //We have to block the signals on the children
    try {
        setSigMask(SIG_BLOCK);
    } catch (std::system_error& e) {
        std::cerr << program_invocation_name << ": " << e.what()
        << std::endl;
    }

    std::thread createHistory(&queueThread, std::ref(userName));

    std::thread sender(&sendThread,std::ref(*local),
                      std::ref(endOfLoop),std::ref(userName));

    std::thread receiver(&recvThread,std::ref(*local),
                      std::ref(endOfLoop));

    //Now we unblock the signals on the main thread
    try {
        setSigMask(SIG_UNBLOCK);
    } catch (std::system_error& e) {
        std::cerr << program_invocation_name << ": " << e.what()
        << std::endl;
    }

    while(!endOfLoop)
        usleep(25000);
    //We must finish both threads gracefully!
    requestCancellation(sender);
    requestCancellation(receiver);
    requestCancellation(createHistory);
}
