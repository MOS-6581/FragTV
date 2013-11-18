#include "FragServer.h"

#include "serverWindow.h"
#ifdef QT_GUI_LIB
#include "SyncHelper.h"
#else
#include "TcpClient.h"
#include "MessageParser.h"
#include "Playback.h"
#endif

#include "MyDebug.h"
#include "TcpListener.h"
#include "TcpConnectionWorker.h"
#include "DemoScanner.h"
#include "MessageBuilder.h"
#include "ThreadManager.h"
#include "MyThread.h"
#include "FragEnums.h"
#include "Spectator.h"
#include "TcpWorkerManager.h"
#include "SpectatorCommands.h"
#include "Persistence.h"

#include <QtCore>
#ifdef QT_GUI_LIB
#include <QFileDialog>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QMessageBox>
#endif
#include <QHostAddress>


FragServer::FragServer() 
{
    persistence   = new Persistence();

    // Build GUI
#ifdef QT_GUI_LIB
    persistence->processCommandline(QApplication::arguments());
    serverWindow = new ServerWindow(persistence);
    serverWindow->show();
#else
    persistence->processCommandline(QCoreApplication::arguments());

    dediServerUI = new DediServerUI(persistence);
    remoteServer = new TcpClient();
    messageParser = new MessageParser(this)  ; // TCP message parser
    demoWriter    = new Playback(this)       ; // WolfcamQL stuff (unused in server) and demo writing
#endif

    // Required for use between threads
    qRegisterMetaType<QList<QByteArray> >("QList<QByteArray>");
    qRegisterMetaType<Spectator*>("Spectator*");
    qRegisterMetaType<TcpConnectionWorker*>("TcpConnectionWorker*");
    qRegisterMetaType<QHostAddress>("QHostAddress");


    myDebug       = new MyDebug()          ; // Custom debug output 
    threadManager = new ThreadManager()    ; // Keep track of threads
    tcpListener   = new TcpListener()      ; // Listen for incoming connections
    workerManager = new TcpWorkerManager() ; 
#ifdef QT_GUI_LIB
    syncHelper    = new SyncHelper()       ; // Barcode window
#endif
    demoScanner   = new DemoScanner()      ; // Find and read demo files

    workerManager->moveToThread(tcpListener->thread());

#ifdef QT_GUI_LIB
    connect(myDebug       , SIGNAL(print(QString))               , SERVERUI->debugEdit             , SLOT(appendPlainText(QString))       );
    connect(serverWindow  , SIGNAL(myQuit())                     , threadManager                   , SLOT(shutdown())                     );
    connect(serverWindow  , SIGNAL(myQuit())                     , this                            , SLOT(beginShutdown())                );
    connect(tcpListener   , SIGNAL(setConnectedClients(int))     , SERVERUI->connectedClientsSpin  , SLOT(setValue(int))                  );
#else
    connect(remoteServer  , SIGNAL(remoteServerConnect())        , remoteServer                    , SLOT(tcpConnect())                   );
    connect(remoteServer  , SIGNAL(newMessage(bool, QByteArray)) , messageParser                   , SLOT(readMessage(bool, QByteArray))  );

    connect(messageParser , SIGNAL(demoNew(QString, QByteArray, qint64)) , demoWriter              , SLOT(demoNew(QString, QByteArray, qint64)) );
    connect(messageParser , SIGNAL(demoAppend(QString, QByteArray))      , demoWriter              , SLOT(demoAppend(QString, QByteArray))      );
    connect(messageParser , SIGNAL(demoFinish())                         , demoWriter              , SLOT(demoFinish())                         );

#endif

    connect(threadManager , SIGNAL(allThreadsFinished())         , this                            , SLOT(finishShutdown())               );
    connect(workerManager , SIGNAL(clientDisc(QHostAddress))     , tcpListener                     , SLOT(tcpClientDisc(QHostAddress))    );

    connect(tcpListener   , SIGNAL(newConnection(Spectator*))    , workerManager                   , SLOT(delegateConnection(Spectator*)) );
   
    connect(demoScanner   , SIGNAL(newDemo(QByteArray))          , workerManager                   , SLOT(resetThrottle())                );
    connect(demoScanner   , SIGNAL(newDemo(QByteArray))          , workerManager                   , SIGNAL(newDemo(QByteArray))          );
    connect(demoScanner   , SIGNAL(appendDemo(QByteArray))       , workerManager                   , SIGNAL(appendDemo(QByteArray))       );
    connect(demoScanner   , SIGNAL(finishDemo(QByteArray))       , workerManager                   , SIGNAL(finishDemo(QByteArray))       );

#ifndef QT_GUI_LIB
    workerManager->connectRemoteSettings(messageParser);
    remoteServer->autoConnect();
#endif

}
FragServer::~FragServer()
{
/*
Fixme... things should be deleted! Disabled atm as I have no idea if there are any issues with these destructors
    delete demoScanner;

#ifdef QT_GUI_LIB
    delete syncHelper;
#endif
    delete workerManager;
    delete tcpListener;
    delete threadManager;

    delete persistence;
    delete myDebug;

#ifndef QT_GUI_LIB
    delete remoteServer;
    delete dediServer;
#endif
    */
}

void FragServer::beginShutdown()
{
}
void FragServer::finishShutdown()
{
#ifdef QT_GUI_LIB
    syncHelper->deleteLater();
#endif
    myDebug->deleteLater();

#ifdef QT_GUI_LIB
    serverWindow->deleteLater();
#endif
}

