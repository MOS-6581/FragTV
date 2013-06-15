#include "FragServer.h"
#include "serverWindow.h"
#include "MyDebug.h"
#include "TcpListener.h"
#include "SyncHelper.h"
#include "TcpConnectionWorker.h"
#include "DemoScanner.h"
#include "MessageBuilder.h"
#include "ThreadManager.h"
#include "MyThread.h"
#include "FragEnums.h"
#include "Spectator.h"
#include "TcpWorkerManager.h"
#include "SpectatorCommands.h"


#include <QtCore>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QHostAddress>


FragServer::FragServer() 
{
    // Build GUI
    serverWindow = new ServerWindow();
    serverWindow->show();


    // Required for use between threads
    qRegisterMetaType<QList<QByteArray> >("QList<QByteArray>");
    qRegisterMetaType<Spectator*>("Spectator*");
    qRegisterMetaType<TcpConnectionWorker*>("TcpConnectionWorker*");
    qRegisterMetaType<QHostAddress>("QHostAddress");


    myDebug       = new MyDebug()          ; // Custom debug output 
    threadManager = new ThreadManager()    ; // Keep track of threads
    tcpListener   = new TcpListener()      ; // Listen for incoming connections
    workerManager = new TcpWorkerManager() ; 
    syncHelper    = new SyncHelper()       ; // Barcode window
    demoScanner   = new DemoScanner()      ; // Find and read demo files

    workerManager->moveToThread(tcpListener->thread());


    connect(myDebug       , SIGNAL(print(QString))               , SERVERUI->debugEdit             , SLOT(appendPlainText(QString))       );

    connect(serverWindow  , SIGNAL(myQuit())                     , threadManager                   , SLOT(shutdown())                     );
    connect(serverWindow  , SIGNAL(myQuit())                     , this                            , SLOT(beginShutdown())                );
    connect(threadManager , SIGNAL(allThreadsFinished())         , this                            , SLOT(finishShutdown())               );

    connect(workerManager , SIGNAL(clientDisc(QHostAddress))     , tcpListener                     , SLOT(tcpClientDisc(QHostAddress))    );
    connect(tcpListener   , SIGNAL(setConnectedClients(int))     , SERVERUI->connectedClientsSpin  , SLOT(setValue(int))                  );
    
    connect(tcpListener   , SIGNAL(newConnection(Spectator*))    , workerManager                   , SLOT(delegateConnection(Spectator*)) );
   
    connect(demoScanner   , SIGNAL(newDemo(QByteArray))          , workerManager                   , SLOT(resetThrottle())                );
    connect(demoScanner   , SIGNAL(newDemo(QByteArray))          , workerManager                   , SIGNAL(newDemo(QByteArray))          );
    connect(demoScanner   , SIGNAL(appendDemo(QByteArray))       , workerManager                   , SIGNAL(appendDemo(QByteArray))       );
    connect(demoScanner   , SIGNAL(finishDemo(QByteArray))       , workerManager                   , SIGNAL(finishDemo(QByteArray))       );
}
FragServer::~FragServer()
{
}

void FragServer::beginShutdown()
{
}
void FragServer::finishShutdown()
{
    syncHelper->deleteLater();
    myDebug->deleteLater();

    serverWindow->deleteLater();
}

