#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(qintptr fd, QObject *parent = 0);

    void start();

    /*
     * Para tu caso se me ocurre que conectes disconnect al quit() de QThread,
     *  eso detiene el hilo.
     * CUando se detenga mandará una señal finished que puedes conectar al
     * deleteLater de Client.
     * Aunque finished indica que el hilo está muerto, realmente aun no.
     * Lo último que hace antes de morir de manera efectiva es hacer todos
     * los delete programados con deleteLater.
     * Así que puedes conectar finished al deleteLater de Client. Y ya está.
     * Al mientras el hilo muere hacer el delete de cliente,
     * que a su vez destruye el objeto QThread, si es miembro de Client.*/

signals:
    void forwardMessage(QString message, int fd);
    void disconnectedClient(int fd);

public slots:
    void disconnected();
    void readyRead();
    void newMessage(QString message, int fd);
    //void deleteLater();

private:
    int my_socket_fd; //for remove from qmap
    QTcpSocket my_socket;
    QThread my_thread;
};

#endif // CLIENT_H
