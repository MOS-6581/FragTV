#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>


class TcpClient : public QObject
{

    Q_OBJECT


public:

    TcpClient(QObject* parent = 0);
    ~TcpClient();


private:

    QTcpSocket *tcpSocket;

    QString currentString;
    quint32 blockSize;


private slots:

    void tcpConnect();
    void tcpConnected();

    void tcpDisconnect();
    void tcpDisconnected();

    void tcpReadData();

    void tcpError(QAbstractSocket::SocketError socketError);


signals:

    void newMessage(bool compressed, QByteArray message);

    void incompatibleVersion(quint32 minimumVersion);


};


#endif // TCPCLIENT_H
