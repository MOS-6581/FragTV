#ifndef MYBROWSER_H
#define MYBROWSER_H

#include <QObject>
#include <QtCore>

class MyDebug;
class BarcodeReader;
class MyNetworkCookieJar;
class StreamChatWindow;
class StreamVideoWindow;
class ThreadManager;
class Ipc;


class MyBrowser : public QObject
{

    Q_OBJECT


public:

    MyBrowser(QString ipckey, int decodeTimeoutLimit, bool singleProcess);
    ~MyBrowser();


private:

    MyDebug* myDebug;
    ThreadManager* threadManager;
    BarcodeReader* barcodeReader;

    MyNetworkCookieJar* myCookieJar;
    StreamVideoWindow* streamVideoWindow;
    StreamChatWindow* streamChatWindow;

    Ipc* ipcClient;


public slots:

    void shutdown();

    void relayDebug(QString message);


};

#endif // MYBROWSER_H
