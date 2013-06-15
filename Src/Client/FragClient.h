#ifndef FRAGCLIENT_H
#define FRAGCLIENT_H

#include <QObject>
#include <QProcess>

class ClientWindow;
class MyDebug;
class ChannelManager;
class TcpClient;
class BarcodeReader;
class Playback;
class MyWebView;
class MessageParser;
class ThreadManager;
class Ipc;
class IpcParser;


class FragClient : public QObject
{

    Q_OBJECT


public:

    FragClient();
    ~FragClient();


private:

    ClientWindow* mainWindow; 

    MyDebug* myDebug;
    ThreadManager* threadManager;
    ChannelManager* channelManager;
    TcpClient* tcpClient;
    Playback* playback;
    MessageParser* messageParser;


    Ipc* ipcServer;
    IpcParser* ipcParser;
    QProcess browserProc;


public slots:

    void beginShutdown();
    void finishShutdown();

    void executeBrowser();


};

#endif // FRAGCLIENT_H
