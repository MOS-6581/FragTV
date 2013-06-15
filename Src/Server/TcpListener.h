#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QtCore>
#include <QObject>

#include <QtNetwork>
#include <QTcpServer>

class Spectator;


class TcpListener : public QTcpServer
{

    Q_OBJECT


public:

    TcpListener();
    ~TcpListener();


private:

    QHostAddress listenIp;
    quint16 listenPort;

    QList<QHostAddress> hostList;
    int limitConnections;

    QList<QHostAddress> hostListDelayedRemove;
    QTimer* delayedRemoveTimer;

    int maximumClients;
    int currentClients;
    int lastClientUpdate;
    QTimer* bulkUpdateConnectedClients;


public slots:

    void setListenIp(QString ip)           { listenIp         = QHostAddress(ip);    };
    void setListenPort(QString portString) { listenPort       = portString.toUInt(); };
    void setLimitConnections(int value)    { limitConnections = value;               };
    void setMaximumClients(int value)      { maximumClients   = value;               };

    void tcpBind();
    void tcpUnbind();

    void tcpClientDisc(QHostAddress hostAddress);
    void delayedRemove();
    void bulkSetConnectedClients();


protected:

    void incomingConnection(int socketDescriptor);


signals:
    
    void setStatus(bool listenerStatus);
    void setStatusMessage(QString statusMessage);

    void newConnection(Spectator* spectator);

    void setConnectedClients(int connectedClients);


};

#endif
