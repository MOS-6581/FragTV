#ifndef FRAGSERVER_H
#define FRAGSERVER_H

#include <QObject>
#include <QtCore>

class ServerWindow;
class MyDebug;
class TcpListener;
class SyncHelper;
class DemoScanner;
class ThreadManager;
class TcpWorkerManager;


class FragServer : public QObject
{

    Q_OBJECT


public:

    FragServer();
    ~FragServer();


private:

    ServerWindow* serverWindow;

    MyDebug* myDebug;
    ThreadManager* threadManager;
    TcpListener* tcpListener;
    TcpWorkerManager* workerManager;
    SyncHelper* syncHelper; 
    DemoScanner* demoScanner;


public slots:

    void beginShutdown();
    void finishShutdown();


};

#endif // FRAGSERVER_H

