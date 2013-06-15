#ifndef IPC_H
#define IPC_H

#include <QObject>
#include <QtCore>
#include <QLocalSocket>
#include <QLocalServer>


class Ipc : public QObject
{

    Q_OBJECT


public:

    Ipc(QObject* parent);
    ~Ipc();

    bool listen(QString key);
    void connect(QString key);


public slots:

    void sendMessage(QStringList message);


private slots:

    void acceptConnection();

    void readMessage();

    void printError();


private:
    
    QLocalServer *localServer;
    QLocalSocket *localSocket;

    quint32 blockSize;


signals:

    void disconnected();

    void newMessage(QStringList message);


};

#endif // IPC_H
