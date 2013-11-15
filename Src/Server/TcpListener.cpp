#include "TcpListener.h"
#include "TcpConnectionWorker.h"
#include "serverWindow.h"

#include "MyThread.h"
#include "Spectator.h"

#include <QTcpSocket>

TcpListener::TcpListener() : currentClients(0), lastClientUpdate(0)
{        
    this->setObjectName("TcpListener");

    MyThread* myThread = new MyThread(this);

#ifdef QT_GUI_LIB
    limitConnections = SERVERUI->limitConnectionsSpin->value();
    maximumClients   = SERVERUI->maxClientsSpin->value();
    listenIp         = QHostAddress(SERVERUI->ipField->text());
    listenPort       = SERVERUI->portField->text().toUInt();
#else
    limitConnections = DediServerUI::getInstance()->getLimitConnections();
    maximumClients   = DediServerUI::getInstance()->getMaximumClients();
    listenIp         = QHostAddress(DediServerUI::getInstance()->getListenIP());
    listenPort       = DediServerUI::getInstance()->getListenPort();
#endif

    bulkUpdateConnectedClients = new QTimer(this);
    bulkUpdateConnectedClients->start(500);

    // Reconnect cooldown
    delayedRemoveTimer = new QTimer(this);
    delayedRemoveTimer->start(1000 * 10);


    connect(bulkUpdateConnectedClients     , SIGNAL(timeout())                 , this                          , SLOT(bulkSetConnectedClients())       );
    connect(delayedRemoveTimer             , SIGNAL(timeout())                 , this                          , SLOT(delayedRemove())                 );

#ifdef QT_GUI_LIB
    connect(SERVERUI->listenButton         , SIGNAL(clicked())                 , this                          , SLOT(tcpBind())                       );
    connect(SERVERUI->disconnectButton     , SIGNAL(clicked())                 , this                          , SLOT(tcpUnbind())                     );
    connect(SERVERUI->maxClientsSpin       , SIGNAL(valueChanged(int))         , this                          , SLOT(setMaximumClients(int))          );
    connect(SERVERUI->limitConnectionsSpin , SIGNAL(valueChanged(int))         , this                          , SLOT(setLimitConnections(int))        );
    connect(SERVERUI->ipField              , SIGNAL(textChanged(QString))      , this                          , SLOT(setListenIp(QString))            );
    connect(SERVERUI->portField            , SIGNAL(textChanged(QString))      , this                          , SLOT(setListenPort(QString))          );

    connect(this                           , SIGNAL(setStatus(bool))           , SERVERMAIN                    , SLOT(setNetworkListeningStatus(bool)) );
    connect(this                           , SIGNAL(setStatusMessage(QString)) , SERVERUI->networkStatusField  , SLOT(setText(QString))                );

    if(SERVERUI->listenOnStartupCheck->isChecked()) 
#else
    if (DediServerUI::getInstance()->getListenOnStartup())
#endif
    {
        connect(myThread, SIGNAL(started()), this, SLOT(tcpBind()));
    }
    else 
    {
        emit this->setStatus(false);
    }
    

    this->moveToThread(myThread);

    myThread->start();
}
TcpListener::~TcpListener()
{
}

void TcpListener::tcpBind()
{
    if(!this->listen(listenIp, listenPort)) 
    {
        emit this->setStatus(false);
        emit this->setStatusMessage("Unable to start the server: " + this->errorString());

        qDebug() << "Unable to start the server: " << this->errorString();

        return;
    }

    emit this->setStatus(true);
    emit this->setStatusMessage("The server is running on IP: " + listenIp.toString() + " port: " + QString::number(listenPort));

    qDebug() << "The server is running on IP: " << listenIp.toString() << " port: " << listenPort;
}
void TcpListener::tcpUnbind()
{
    this->close();

    emit this->setStatus(false);
    emit this->setStatusMessage("Unbound..");
    
    qDebug() << "TcpListener disconnected";
}

void TcpListener::incomingConnection(int socketDescriptor)
{
    if(currentClients >= maximumClients)
    {
        QTcpSocket connection;

        if(!connection.setSocketDescriptor(socketDescriptor, QAbstractSocket::ConnectedState, QIODevice::WriteOnly)) 
        {
            return;
        }

        connection.abort();

        return;
    }


    Spectator* spectator = new Spectator();

    if(!spectator->setSocketDescriptor(socketDescriptor, QAbstractSocket::ConnectedState, QIODevice::WriteOnly)) 
    {
        spectator->deleteLater();

        return;
    }


    QHostAddress hostAddress = spectator->peerAddress();

    if(hostList.count(hostAddress) >= limitConnections) 
    {
        spectator->abort();
        spectator->deleteLater();

        return;
    }


    spectator->socketId = socketDescriptor;

    currentClients++;
    hostList.append(hostAddress);


    emit this->newConnection(spectator);
}
void TcpListener::tcpClientDisc(QHostAddress hostAddress)
{
    hostListDelayedRemove << hostAddress;
    currentClients--;
}
void TcpListener::delayedRemove()
{
    foreach(QHostAddress host, hostListDelayedRemove)
    {
        hostListDelayedRemove.removeOne(host);
        hostList.removeOne(host);
    }
}
void TcpListener::bulkSetConnectedClients()
{
    if(currentClients == lastClientUpdate)
    {
        return;
    }

    lastClientUpdate = currentClients;


    emit this->setConnectedClients(currentClients);
}

