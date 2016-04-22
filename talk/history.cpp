#include "history.h"

History::History(const std::string username)
{
    size_ = HISTORYSIZE;
    std::string path = std::getenv("HOME");
    path += "/.talk/";
    DIR* dir = opendir(path.c_str());
    if (dir) {
        closedir(dir);
    } else {
        if (ENOENT == errno) {
            int createDir = mkdir(path.c_str(), 0777);
            if (createDir == -1)
                throw std::system_error(errno, std::system_category(),
                                        "No se pudo crear el directorio");
        } else {
            throw std::system_error(errno, std::system_category(),
                                    "No se pudo abrir el directorio");
        }
    }

    path += username;
    path += ".log";

    int fd = open(path.c_str(), (O_RDWR | O_CREAT), 06666);
    if (fd == -1)
        throw std::system_error(errno, std::system_category(),
                                "Imposible crear el fichero");
    auto truncateFile = ftruncate(fd, size_);
    if (truncateFile == -1) {
        throw std::system_error(errno, std::system_category(),
                                "Imposible Truncar el fichero");
    }
    fileAddress_ = static_cast<char*>(mmap(nullptr, size_,
                                           (PROT_READ | PROT_WRITE),
                                           MAP_SHARED, fd,
                                           0));
    if (fileAddress_ == MAP_FAILED) {
        throw std::system_error(errno, std::system_category(),
                                "Fallo al Mapear");
    }
    char* auxiliar = fileAddress_;
    lastPosition_ = static_cast<char*>(memchr(auxiliar, '`', HISTORYSIZE));
    if (lastPosition_ == nullptr)
        lastPosition_ = fileAddress_;
}

History::~History()
{
    munmap(fileAddress_, HISTORYSIZE);
    lastPosition_ = nullptr;
}

void History::addToHistory(const Message &message)
{
    if (lastPosition_ != fileAddress_)
        if(*(lastPosition_-1) == '`')
            lastPosition_ --;

    std::string line = message.time;
    line += " ";
    line += message.username;
    line += " said:";
    line += message.text;
    line += '\n';

    if (lastPosition_ + line.length() >= (fileAddress_ + HISTORYSIZE) - 2)
        lastPosition_ = fileAddress_;

    int num_bytes = line.copy(lastPosition_, line.length(), 0);
    lastPosition_ += num_bytes;
    *lastPosition_ = '`';

}
