#ifndef TCPTHREAD_H
#define TCPTHREAD_H

#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>

class Spectator;


class TcpConnectionWorker : public QObject
{

    Q_OBJECT


public:

    TcpConnectionWorker(int workerId);
    ~TcpConnectionWorker();

    int workerId;
    int connectionsCount;

    QByteArray motdMessage;
    QByteArray browserPosition;
    QByteArray browserVideoUrl;
    QByteArray browserChatUrl;


private:

    QList<Spectator*> throttled;
    QHash<int,Spectator*> spectatorList;
    QHash<int,Spectator*> liveOk;
    QHash<int,Spectator*> liveBuffering;

    bool isStreaming;
    QList<QByteArray> currentDemoAllChunks;

    qint64 bytesWrittenSinceDump;


public slots:

    void newConnection(Spectator* connection);
    void removeConnection();
    void serveThrottled();

    void sendMessage(QByteArray message);
    void sendDataPulse();

    void newDemo(QByteArray data);
    void appendDemo(QByteArray data);
    void finishDemo(QByteArray data);

    void setMotd(QByteArray _motdMessage); 
    void setBrowserPosition(QByteArray _browserPosition); 
    void setBrowserVideoUrl(QByteArray _browserVideoUrl);
    void setBrowserChatUrl(QByteArray _browserChatUrl);

    void socketBytesWritten(qint64 bytes);
    void dumpNetStats();


signals:

    void started();

    void error(QTcpSocket::SocketError socketError);
    void disconnected();
    void connectionRemoved(QHostAddress hostAddress);
    
    void bytesWritten(qint64 bytes);

    void serveThrottledFinished();


};


#endif