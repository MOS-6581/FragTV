#include "FragClient.h"
#include "TcpClient.h"
#include "clientWindow.h"
#include "MyDebug.h"
#include "StreamVideoWindow.h"
#include "StreamChatWindow.h"
#include "ChannelManager.h"
#include "BarcodeReader.h"
#include "Playback.h"
#include "MyNetworkCookieJar.h"
#include "FragEnums.h"
#include "MessageParser.h"
#include "ThreadManager.h"
#include "MyThread.h"
#include "MyBrowser.h"
#include "Ipc.h"
#include "IpcParser.h"

#include <QObject>
#include <QtCore>
#include <QtGui>
#include <QNetworkAccessManager>


FragClient::FragClient()
{
    // GUI
    mainWindow = new ClientWindow();
    mainWindow->show();


    myDebug        = new MyDebug()            ; // Custom debug output
    threadManager  = new ThreadManager()      ; // Keep track of threads
    channelManager = new ChannelManager(this) ; // GUI server list
    tcpClient      = new TcpClient(this)      ; // TCP connection
    messageParser  = new MessageParser(this)  ; // TCP message parser
    playback       = new Playback(this)       ; // WolfcamQL stuff and demo writing
    ipcServer      = new Ipc(this)            ; // Communicate with browser process
    ipcParser      = new IpcParser(this)      ; // Translate IPC messages to signals


    connect(myDebug, SIGNAL(print(QString)), CLIENTUI->debugEdit, SLOT(appendPlainText(QString)));


    // Terminate on main window close
    connect(mainWindow       , SIGNAL(myQuit())                             , threadManager             , SLOT(shutdown())                           );
    connect(mainWindow       , SIGNAL(myQuit())                             , this                      , SLOT(beginShutdown())                      );
    connect(threadManager    , SIGNAL(allThreadsFinished())                 , this                      , SLOT(finishShutdown())                     );

    // TcpClient connections
    connect(tcpClient        , SIGNAL(newMessage(bool, QByteArray))         , messageParser             , SLOT(readMessage(bool, QByteArray))        );
    connect(channelManager   , SIGNAL(channelConnect())                     , tcpClient                 , SLOT(tcpConnect())                         );

    // Demo and chat data for Wolfcam
    connect(messageParser    , SIGNAL(demoNew(QString, QByteArray, qint64)) , playback                  , SLOT(demoNew(QString, QByteArray, qint64)) );
    connect(messageParser    , SIGNAL(demoAppend(QString, QByteArray))      , playback                  , SLOT(demoAppend(QString, QByteArray))      );
    connect(messageParser    , SIGNAL(demoFinish())                         , playback                  , SLOT(demoFinish())                         );
    connect(ipcParser        , SIGNAL(writeChat(QStringList))               , playback                  , SLOT(writeChat(QStringList))               );

    // Video synchronization
    connect(playback         , SIGNAL(ipcGetVideoOffset(QStringList))       , ipcServer                 , SLOT(sendMessage(QStringList))             );
    connect(ipcParser        , SIGNAL(videoOffset(int))                     , playback                  , SLOT(setVideoOffset(int))                  );
    connect(mainWindow       , SIGNAL(setDecodeTimeout(QStringList))        , ipcServer                 , SLOT(sendMessage(QStringList))             );

    connect(messageParser    , SIGNAL(browserMessage(QStringList))          , ipcServer                 , SLOT(sendMessage(QStringList))             );
    connect(messageParser    , SIGNAL(motd(QString))                        , CLIENTUI->motdTextBrowser , SLOT(setHtml(QString))                     );

    connect(ipcServer        , SIGNAL(newMessage(QStringList))              , ipcParser                 , SLOT(parseMessage(QStringList))            );


    executeBrowser();
}
FragClient::~FragClient()
{
}


void FragClient::beginShutdown()
{
    ipcServer->deleteLater();
    browserProc.terminate();
}
void FragClient::finishShutdown()
{
    tcpClient->deleteLater();
    playback->deleteLater();
    channelManager->deleteLater();
    myDebug->deleteLater();
 
    mainWindow->deleteLater();
}

void FragClient::executeBrowser()
{
    QString timestamp = QString::number(QDateTime::currentMSecsSinceEpoch());

    ipcServer->listen("FRAGBROWSER" + timestamp);


    bool singleProcess = false; // for debugging..
    if(singleProcess) 
    {
        MyBrowser* browser = new MyBrowser(timestamp, CLIENTUI->dataMatrixTimeoutSpin->value(), true);
        Q_UNUSED(browser);

        return;
    }


    connect( ipcServer , SIGNAL(disconnected()), 
             this      , SLOT(executeBrowser()));


    QStringList browserArguments;
    browserArguments << "-browser";
    browserArguments << "-ipckey" << timestamp;
    browserArguments << "-decodeTimeout" << QString::number(CLIENTUI->dataMatrixTimeoutSpin->value());

    browserProc.startDetached(QApplication::applicationFilePath(), browserArguments);
}

