#include "MyBrowser.h"

#include "MyDebug.h"
#include "StreamVideoWindow.h"
#include "StreamChatWindow.h"
#include "BarcodeReader.h"
#include "MyNetworkCookieJar.h"
#include "ThreadManager.h"
#include "MyThread.h"
#include "Ipc.h"
#include "IpcParser.h"

#include <QObject>
#include <QtCore>
#include <QtGui>
#include <QNetworkAccessManager>
#include <QApplication>

MyBrowser::MyBrowser(QString ipckey, int decodeTimeoutLimit, bool singleProcess) : myDebug(NULL)
{
    ipcClient = new Ipc(this);
    IpcParser* ipcParser = new IpcParser(this);

    connect( ipcClient , SIGNAL(newMessage(QStringList)), 
             ipcParser , SLOT(parseMessage(QStringList)));

    ipcClient->connect("FRAGBROWSER" + ipckey);


    if(!singleProcess)
    {
        // Relay debug output to parent process
        myDebug = new MyDebug(); 

        connect( myDebug , SIGNAL(print(QString)), 
            this         , SLOT(relayDebug(QString)));


        threadManager = new ThreadManager(this)               ; // Keep track of threads
    }


    streamChatWindow  = new StreamChatWindow()                ; // Parse twitch.tv chat and print ingame
    streamVideoWindow = new StreamVideoWindow()               ; // WebKit based browser with NPAPI support for flash player
    barcodeReader     = new BarcodeReader(decodeTimeoutLimit) ; // Barcode scanner & decoder


    // Persistent cookies
    myCookieJar = new MyNetworkCookieJar(this);

    // Shared cookies between video and chat browsers
    streamChatWindow->page()->networkAccessManager()->setCookieJar(myCookieJar);
    streamVideoWindow->page()->networkAccessManager()->setCookieJar(myCookieJar);


    // Terminate with parent process
    connect(ipcClient           , SIGNAL(disconnected())              , GLOBALTHREADMANAGER , SLOT(shutdown())               );
    connect(GLOBALTHREADMANAGER , SIGNAL(allThreadsFinished())        , this                , SLOT(shutdown())               );

    // Browser connections
    connect(ipcParser           , SIGNAL(chatUrl(QString))            , streamChatWindow    , SLOT(safeLoad(QString))        );
    connect(ipcParser           , SIGNAL(videoUrl(QString))           , streamVideoWindow   , SLOT(safeLoad(QString))        );
    connect(ipcParser           , SIGNAL(videoPos(int))               , streamVideoWindow   , SLOT(setWindowPosition(int))   );

    // Playback connections
    connect(streamChatWindow    , SIGNAL(writeChat(QStringList))      , ipcClient           , SLOT(sendMessage(QStringList)) );
    connect(ipcParser           , SIGNAL(decodeTimeout(int))          , barcodeReader       , SLOT(setDecodeTimeout(int))    );
    connect(ipcParser           , SIGNAL(getVideoOffset())            , streamVideoWindow   , SLOT(getOffset())              );
    connect(barcodeReader       , SIGNAL(ipcVideoOffset(QStringList)) , ipcClient           , SLOT(sendMessage(QStringList)) );

    // Send screenshots to decoder and generate video timestamps
    connect( streamVideoWindow , SIGNAL(decodeTimeStamp(qint64, QImage, QImage)), 
             barcodeReader     , SLOT(readTimeStamp(qint64, QImage, QImage)));
}
MyBrowser::~MyBrowser()
{
}

void MyBrowser::shutdown()
{
    streamChatWindow->deleteLater(); 
    streamVideoWindow->deleteLater();

    if(myDebug)
    {
        myDebug->deleteLater();
    }

    this->deleteLater();

    QApplication::quit();
}

void MyBrowser::relayDebug(QString message)
{
    QStringList ipcMessage;
    ipcMessage << "relayDebug";
    ipcMessage << "IPC:: " + message;

    ipcClient->sendMessage(ipcMessage);
}

