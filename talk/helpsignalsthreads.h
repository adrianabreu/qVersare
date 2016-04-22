#ifndef HELPSIGNALSTHREADS_H
#define HELPSIGNALSTHREADS_H

#include <thread>
#include <pthread.h>
#include <csignal>
#include <atomic>
#include <system_error>
#include <mutex>
#include <condition_variable>
#include <queue>

#include "socket.h"
#include "history.h"

#define SUCCESS 0
#define ERR_SOCKET 3

extern std::atomic<bool> endOfLoop;

extern std::queue<Message> historyQueue;
extern std::mutex mutexSignal;
extern std::condition_variable conditionSignal;

/*
 * These are helping functions common to both
 * server and client side.
 */

//Signal help
void int_signal_handler(int signum);
void setSigMask(int sigAction);

//This function will shutdown the threads
void requestCancellation(std::thread& oneThread);

void queueThread(const std::string& username);

#endif // HELPSIGNALSTHREADS_H
