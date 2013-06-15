#include "Ipc.h"

Ipc::Ipc(QObject* parent) : QObject(parent), blockSize(0)
{
}
Ipc::~Ipc()
{
}

bool Ipc::listen(QString key)
{
    localServer = new QLocalServer(this);

    if(!localServer->listen(key)) 
    {
        qDebug() << "Ipc:: Unable to start the server: " << localServer->errorString();

        return false;
    }


    QObject::connect( localServer , SIGNAL(newConnection()), 
                      this        , SLOT(acceptConnection()) );

    return true;
}
void Ipc::connect(QString key)
{
    localSocket = new QLocalSocket(this);

    QObject::connect(localSocket , SIGNAL(readyRead())                           , this , SLOT(readMessage())    );
    QObject::connect(localSocket , SIGNAL(error(QLocalSocket::LocalSocketError)) , this , SLOT(printError())     );
    QObject::connect(localSocket , SIGNAL(disconnected())                        , this , SIGNAL(disconnected()) );


    localSocket->connectToServer(key);
}

void Ipc::acceptConnection()
{
    localSocket = localServer->nextPendingConnection();


    QObject::connect(localSocket , SIGNAL(disconnected())                        , this , SIGNAL(disconnected()) );
    QObject::connect(localSocket , SIGNAL(readyRead())                           , this , SLOT(readMessage())    );
    QObject::connect(localSocket , SIGNAL(error(QLocalSocket::LocalSocketError)) , this , SLOT(printError())     );
}

void Ipc::sendMessage(QStringList message)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);

    out.setVersion(QDataStream::Qt_4_0);
    out << (quint32)0;

    out << message;

    out.device()->seek(0);
    out << (quint32)(block.size() - sizeof(quint32));


    localSocket->write(block);
}
void Ipc::readMessage()
{
    QDataStream data(localSocket);
    data.setVersion(QDataStream::Qt_4_0);

    if(blockSize == 0) 
    {
        if(localSocket->bytesAvailable() < (int)sizeof(quint32))
            return;

        data >> blockSize;
    }


    if(localSocket->bytesAvailable() < blockSize) 
        return;

    blockSize = 0;


    QStringList message;
    data >> message;

    emit this->newMessage(message);


    if(localSocket->bytesAvailable() > 0)
    {
        QMetaObject::invokeMethod(this, "readMessage", Qt::QueuedConnection);
    }
}

void Ipc::printError()
{
    qDebug() << localSocket->errorString();
}

