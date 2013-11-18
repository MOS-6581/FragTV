#include "TcpWorkerManager.h"
#include "TcpConnectionWorker.h"
#include "Spectator.h"
#include "serverWindow.h"
#include "ThreadManager.h"
#include "SpectatorCommands.h"
#include "FragEnums.h"
#include "NetStats.h"

#ifndef QT_GUI_LIB
#include "MessageParser.h"
#endif

TcpWorkerManager::TcpWorkerManager(QObject* parent) : QObject(parent), throttledInProgress(0)
{
#ifdef QT_GUI_LIB
    maxThrottle  = SERVERUI->throttleConnectionsSpin->value();

    QString html     = SERVERUI->motdHtmlEditField->toPlainText();
    QString videoUrl = SERVERUI->videoBrowserUrlField->text();
    QString chatUrl  = SERVERUI->chatBrowserUrlField->text();
#else
    maxThrottle  = DediServerUI::getInstance()->getMaxThrottle();

    QString html = DediServerUI::getInstance()->getMotdHtml();
    QString videoUrl = DediServerUI::getInstance()->getVideoURL();
    QString chatUrl = DediServerUI::getInstance()->getChatURL();
#endif

    // Browser commands etc
    spectatorCommands = new SpectatorCommands(this);
    spectatorCommands->setMotd(html);
    spectatorCommands->setBrowserVideoUrl(videoUrl);
    QUrl url = QUrl(videoUrl);
    if(!url.isValid())
        spectatorCommands->setBrowserPosition(FRAGTV::Browser::VideoHide);
    else
        spectatorCommands->setBrowserPosition(FRAGTV::Browser::VideoNormal);

    spectatorCommands->setBrowserChatUrl(chatUrl);

#ifdef QT_GUI_LIB
    connect(SERVERMAIN , SIGNAL(cmdBrowserPosition(int)) , spectatorCommands , SLOT(setBrowserPosition(int))     );
    connect(SERVERMAIN , SIGNAL(cmdMotd(QString))        , spectatorCommands , SLOT(setMotd(QString))            );
    connect(SERVERMAIN , SIGNAL(cmdVideoUrl(QString))    , spectatorCommands , SLOT(setBrowserVideoUrl(QString)) );
    connect(SERVERMAIN , SIGNAL(cmdChatUrl(QString))     , spectatorCommands , SLOT(setBrowserChatUrl(QString))  );
#endif

    netStats = new NetStats(this);


    // Check for available slots and serve waiting connections
    serveThrottledTimer = new QTimer(this);
    serveThrottledTimer->start(5000);

    connect( serveThrottledTimer , SIGNAL(timeout()), 
             this                , SLOT(serveThrottled()));


#ifdef QT_GUI_LIB
    for(int i=0; i < SERVERUI->threadsSpin->value(); i++)
#else
    for(int i=0; i < DediServerUI::getInstance()->getNumThreads(); i++)
#endif
    {
        // TcpWorkers handle buffering and transmission of data 
        createTcpWorker();
    }
}
TcpWorkerManager::~TcpWorkerManager()
{
}

#ifndef QT_GUI_LIB
void TcpWorkerManager::connectRemoteSettings(MessageParser *messageParser)
{
    connect(messageParser, SIGNAL(cmdBrowserPosition(int)) , spectatorCommands , SLOT(setBrowserPosition(int))     );
    connect(messageParser, SIGNAL(cmdMotd(QString))        , spectatorCommands , SLOT(setMotd(QString))            );
    connect(messageParser, SIGNAL(cmdVideoUrl(QString))    , spectatorCommands , SLOT(setBrowserVideoUrl(QString)) );
    connect(messageParser, SIGNAL(cmdChatUrl(QString))     , spectatorCommands , SLOT(setBrowserChatUrl(QString))  );
}
#endif

void TcpWorkerManager::createTcpWorker()
{
    int workerId = allWorkers.size();

    TcpConnectionWorker* tcpConnectionWorker = new TcpConnectionWorker(workerId);


    allWorkers.append(tcpConnectionWorker);


    tcpConnectionWorker->motdMessage     = spectatorCommands->currentMotdMessage;
    tcpConnectionWorker->browserVideoUrl = spectatorCommands->currentBrowserVideoUrl;
    tcpConnectionWorker->browserPosition = spectatorCommands->currentBrowserPosition;
    tcpConnectionWorker->browserChatUrl  = spectatorCommands->currentBrowserChatUrl;


    connect(tcpConnectionWorker , SIGNAL(started())                        , this                , SLOT(createTcpWorkerDone())          );
    connect(tcpConnectionWorker , SIGNAL(serveThrottledFinished())         , this                , SLOT(serveThrottledFinished())       );
    connect(tcpConnectionWorker , SIGNAL(bytesWritten(qint64))             , netStats            , SLOT(bytesWritten(qint64))           );
    connect(tcpConnectionWorker , SIGNAL(connectionRemoved(QHostAddress))  , this                , SIGNAL(clientDisc(QHostAddress))     );

    connect(spectatorCommands   , SIGNAL(sendMessage(QByteArray))          , tcpConnectionWorker , SLOT(sendMessage(QByteArray))        );
    connect(spectatorCommands   , SIGNAL(relayMotdMessage(QByteArray))     , tcpConnectionWorker , SLOT(setMotd(QByteArray))            );
    connect(spectatorCommands   , SIGNAL(relayBrowserPosition(QByteArray)) , tcpConnectionWorker , SLOT(setBrowserPosition(QByteArray)) );
    connect(spectatorCommands   , SIGNAL(relayBrowserVideoUrl(QByteArray)) , tcpConnectionWorker , SLOT(setBrowserVideoUrl(QByteArray)) );
    connect(spectatorCommands   , SIGNAL(relayBrowserChatUrl(QByteArray))  , tcpConnectionWorker , SLOT(setBrowserChatUrl(QByteArray))  );

    connect(this                , SIGNAL(newDemo(QByteArray))              , tcpConnectionWorker , SLOT(newDemo(QByteArray))            );
    connect(this                , SIGNAL(appendDemo(QByteArray))           , tcpConnectionWorker , SLOT(appendDemo(QByteArray))         );
    connect(this                , SIGNAL(finishDemo(QByteArray))           , tcpConnectionWorker , SLOT(finishDemo(QByteArray))         );
}
void TcpWorkerManager::createTcpWorkerDone()
{
    TcpConnectionWorker* worker = qobject_cast<TcpConnectionWorker*>(sender());

    throttledWorkersQueue.append(worker);
}

void TcpWorkerManager::delegateConnection(Spectator* spectator)
{
    TcpConnectionWorker* worker = getAvailableWorker();

    if(!worker)
    {
        return;
    }


    spectator->moveToThread(worker->thread());

    QMetaObject::invokeMethod(worker, "newConnection", Qt::QueuedConnection, Q_ARG(Spectator*, spectator));
}
TcpConnectionWorker* TcpWorkerManager::getAvailableWorker()
{
    if(allWorkers.isEmpty())
    {
        return NULL;
    }


    TcpConnectionWorker* bestWorker = NULL;

    for(int i=0; i < allWorkers.size(); i++)
    {
        TcpConnectionWorker* worker = allWorkers.at(i);

        if(!worker)
        {
            allWorkers.removeAt(i);
            continue;
        }

        if(!bestWorker)
        {
            bestWorker = worker;
        }
        else if(worker->connectionsCount < bestWorker->connectionsCount)
        {
            bestWorker = worker;
        }
    }

    return bestWorker;
}

void TcpWorkerManager::serveThrottled()
{
    if(throttledWorkersQueue.isEmpty())
    {
        return;
    }


    while(throttledInProgress < maxThrottle)
    {
        TcpConnectionWorker* nextWorker = throttledWorkersQueue.takeFirst();

        if(!nextWorker)
        {
            continue;
        }


        throttledWorkersQueue.append(nextWorker);


        QMetaObject::invokeMethod(nextWorker, "serveThrottled", Qt::QueuedConnection);

        throttledInProgress++;
    }
}
void TcpWorkerManager::serveThrottledFinished()
{
    throttledInProgress--;
}
void TcpWorkerManager::resetThrottle()
{
    throttledInProgress = 0;
}

