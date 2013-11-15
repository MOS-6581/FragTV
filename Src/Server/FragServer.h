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
class Persistence;

class DediServerUI;
class TcpClient;
class MessageParser;
class Playback;

class FragServer : public QObject
{

    Q_OBJECT


public:

    FragServer();
    ~FragServer();


private:

    MyDebug* myDebug;
    ThreadManager* threadManager;
    TcpListener* tcpListener;
    TcpWorkerManager* workerManager;
    DemoScanner* demoScanner;
    Persistence* persistence;

#ifdef QT_GUI_LIB
    ServerWindow* serverWindow;
    SyncHelper* syncHelper; 
#else
    DediServerUI* dediServerUI;
    TcpClient* remoteServer;
    MessageParser* messageParser;
    Playback* demoWriter;
#endif

public slots:

    void beginShutdown();
    void finishShutdown();

};

#endif // FRAGSERVER_H

