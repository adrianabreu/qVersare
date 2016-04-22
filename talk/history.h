#ifndef HISTORY_H
#define HISTORY_H
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <cstdlib>
#include <dirent.h>
#include <stdio.h>

#include "socket.h"

#define HISTORYSIZE 1048576

class History
{
public:
    History(const std::string username);
    ~History();
    void addToHistory(const Message& message);
private:
    int size_;
    char* fileAddress_;
    char* lastPosition_;
};

#endif // HISTORY_H
