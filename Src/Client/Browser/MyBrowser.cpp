#include "MyBrowser.h"

#include "MyDebug.h"
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


    barcodeReader     = new BarcodeReader(decodeTimeoutLimit) ; // Barcode scanner & decoder


    // Persistent cookies
    myCookieJar = new MyNetworkCookieJar(this);

    // Terminate with parent process
    connect(ipcClient           , SIGNAL(disconnected())              , GLOBALTHREADMANAGER , SLOT(shutdown())               );
    connect(GLOBALTHREADMANAGER , SIGNAL(allThreadsFinished())        , this                , SLOT(shutdown())               );

    // Playback connections
    connect(ipcParser           , SIGNAL(decodeTimeout(int))          , barcodeReader       , SLOT(setDecodeTimeout(int))    );
    connect(barcodeReader       , SIGNAL(ipcVideoOffset(QStringList)) , ipcClient           , SLOT(sendMessage(QStringList)) );

             barcodeReader     , SLOT(readTimeStamp(qint64, QImage, QImage));
}
MyBrowser::~MyBrowser()
{
}

void MyBrowser::shutdown()
{
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

