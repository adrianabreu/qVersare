#include "socket.h"
#include "tcpserver.h"
#include "client.h"
#include "server.h"
#include "helpsignalsthreads.h"
#include "history.h"

#include <getopt.h>
#include <cstdlib>

std::atomic<bool> endOfLoop(false);

std::queue<Message> historyQueue;
std::mutex mutexSignal;
std::condition_variable conditionSignal;

void handleSignals()
{
    signal(SIGINT, &int_signal_handler);
    signal(SIGTERM, &int_signal_handler);
    signal(SIGHUP, &int_signal_handler);
}

void displayHelp()
{
    std::cout << "Usage mode: " << std::endl
              << "-h --help: Display this message " << std::endl
              << "-s --server: Enter server mode" << std::endl
              << "-c --client IP: Connect to that ip " << std::endl
              << "-p --port PORT: listen/connect on port" << std::endl
              << "-u --user USER: select sender name" << std::endl;
}

void parseArguments(bool help_option,bool server_option,std::string port_option,
                    std::string client_option, std::string userOption, int *aux)
{
    int port = 0; //A port 0 given to the bind function make it to use a random
                  //chose from the OS
    std::string userName;

    if (help_option)
        displayHelp();

    if (!port_option.empty())
        port = stoi(port_option);

    if (!userOption.empty()) {
        userName = userOption;
    } else {
        const char* systemUser = std::getenv("USER");

        if (systemUser != nullptr) {
            userName.assign(systemUser);
        } else {
            userName.assign("Unknown");
        }
    }

    handleSignals();

    if (server_option) {
        TCPServer local = server::setupServer("0.0.0.0",port,*&aux);
        if(*aux == SUCCESS)
            server::startServer(&local, userName);
    } else if (client_option != "") {
        //Server are also clients so servers have priority
        if (port != 0) {
            Socket local = client::setupSocket("0.0.0.0",client_option,port,
                                               &*aux);
            if(*aux == SUCCESS)
                client::startClient(&local, userName);
        } else {
            std::cout << "Please, specify the remote port to connect to"
                      << std::endl;
        }
    }
}

int main(int argc, char* argv[]){

    bool help_option = false;
    bool server_option = false;
    std::string port_option;
    std::string client_option;
    std::string user_option;

    static struct option long_options[] =
    {
        {"help", no_argument, 0, 'h'},
        {"server", no_argument, 0, 's'},
        {"client", required_argument, 0,'c'},
        {"port", required_argument, 0, 'p' },
        {"user", required_argument, 0, 'u'}
    };

    int option_index = 0;
    int c;
    while ((c = getopt_long(argc, argv, "hsc:p:u:",long_options,
                            &option_index)) != -1) {
        switch (c) {
            case 'h':
                help_option = 1;
            break;
            case 's':
                server_option = 1;
            break;
            case 'p':
                port_option = std::string(optarg);
            break;
            case 'c':
                client_option = std::string(optarg);
            break;
            case 'u':
                user_option = std::string(optarg);
            break;
            default:
                std::fprintf(stderr, "getopt devolvió 0%o \n", c);
        }
    }
    int aux = SUCCESS; //Return value (0)
    parseArguments(help_option,server_option,port_option,client_option,
                   user_option,&aux);

    return aux;
}

