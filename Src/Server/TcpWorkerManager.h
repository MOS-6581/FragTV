#ifndef TCPWORKERMANAGER_H
#define TCPWORKERMANAGER_H

#include <QObject>
#include <QtCore>
#include <QHostAddress>

class TcpConnectionWorker;
class Spectator;
class SpectatorCommands;
class NetStats;
class MessageParser;


class TcpWorkerManager : public QObject
{

    Q_OBJECT

public:

    TcpWorkerManager(QObject *parent = 0);
    ~TcpWorkerManager();

#ifndef QT_GUI_LIB
    void connectRemoteSettings(MessageParser *messageParser);
#endif

private:

    QList<TcpConnectionWorker*> allWorkers;
    QList<TcpConnectionWorker*> throttledWorkersQueue;

    int maxThrottle;
    QTimer* serveThrottledTimer;
    int throttledInProgress;

    SpectatorCommands* spectatorCommands;

    NetStats* netStats;

public slots:

    void createTcpWorker();
    void createTcpWorkerDone();

    void delegateConnection(Spectator* spectator);
    TcpConnectionWorker* getAvailableWorker();

    void serveThrottled();
    void serveThrottledFinished();
    void resetThrottle();
    

signals:

    void newDemo(QByteArray);
    void appendDemo(QByteArray);
    void finishDemo(QByteArray);

    void clientDisc(QHostAddress);


};


#endif // TCPWORKERMANAGER_H
