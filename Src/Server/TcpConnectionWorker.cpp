#include "TcpConnectionWorker.h"
#include "Spectator.h"
#include "MessageBuilder.h"
#include "MyThread.h"
#include "FragEnums.h"

#include <QtNetwork>
#include <QByteArray>
#include <QDateTime>


TcpConnectionWorker::TcpConnectionWorker(int workerId) : workerId(workerId), connectionsCount(0), motdMessage(NULL), browserPosition(NULL), browserVideoUrl(NULL), browserChatUrl(NULL), isStreaming(false), bytesWrittenSinceDump(0)
{
    this->setObjectName("TcpWorker" + QString::number(workerId));

    MyThread* myThread = new MyThread(this);


    QTimer* sendDataPulseTimer = new QTimer(this);
    sendDataPulseTimer->start(100);


    QTimer* netStatTimer = new QTimer(this);
    netStatTimer->start(1000);


    connect(sendDataPulseTimer , SIGNAL(timeout()) , this , SLOT(sendDataPulse()) );
    connect(netStatTimer       , SIGNAL(timeout()) , this , SLOT(dumpNetStats())  );
    connect(myThread           , SIGNAL(started()) , this , SIGNAL(started())     );


    this->moveToThread(myThread);

    myThread->start();
}
TcpConnectionWorker::~TcpConnectionWorker()
{
    QList<Spectator*> spectators = spectatorList.values();

    for(int i=0; i < spectators.size(); i++)
    {
        spectators.value(i)->abort();
    }
}

void TcpConnectionWorker::newConnection(Spectator* spectator)
{
    spectator->setReadBufferSize(1);


    spectatorList.insert(spectator->socketId, spectator);


    if(isStreaming)
    {
        // Game in progress, wait in line
        spectator->throttled = true;

        throttled.append(spectator);
    }
    else
    {
        spectator->throttled = false;

        liveOk.insert(spectator->socketId, spectator);
    }


    connectionsCount++;


    connect(spectator , SIGNAL(disconnected())       , this , SLOT(removeConnection())         );
    connect(spectator , SIGNAL(bytesWritten(qint64)) , this , SLOT(socketBytesWritten(qint64)) );


    if(!motdMessage.isNull())
    {
        spectator->write(motdMessage);
    }

    if(!browserChatUrl.isNull())
    {
        spectator->write(browserChatUrl);
    }

    if(!browserVideoUrl.isNull())
    {
        spectator->write(browserVideoUrl);
    }

    if(!browserPosition.isNull())
    {
        spectator->write(browserPosition);
    }

    spectator->setSocketOption(QAbstractSocket::LowDelayOption, 1);
}
void TcpConnectionWorker::removeConnection()
{
    Spectator* spectator = qobject_cast<Spectator*>(sender());

    QHostAddress hostAddress = spectator->peerAddress();

    liveOk.remove(spectator->socketId);
    liveBuffering.remove(spectator->socketId);
    spectatorList.remove(spectator->socketId);
    throttled.removeOne(spectator);

    spectator->abort();
    spectator->deleteLater();


    connectionsCount--;

    emit this->connectionRemoved(hostAddress);
}
void TcpConnectionWorker::serveThrottled()
{
    if(throttled.isEmpty())
    {
        emit this->serveThrottledFinished();

        return;
    }


    Spectator* spectator = throttled.takeFirst();

    spectator->throttled = true;


    liveBuffering.insert(spectator->socketId, spectator);
}

void TcpConnectionWorker::sendMessage(QByteArray message)
{
    QList<Spectator*> spectators = spectatorList.values();

    for(int i=0; i < spectators.size(); i++)
    {
        Spectator* spectator = spectators.value(i);

        spectator->write(message);
    }
}
void TcpConnectionWorker::sendDataPulse()
{
    if(currentDemoAllChunks.isEmpty())
    {
        return;
    }


    int idleSpecs = 0;
    int bufferingComplete = 0;

    qint64 currentTime = QDateTime::currentMSecsSinceEpoch();


    QList<Spectator*> spectators = liveBuffering.values();

    for(int i=0; i < spectators.size(); i++)
    {
        Spectator* spectator = spectators.value(i);


        if(!spectator->bytesToWrite())
        {
            for(int j=0; j < 3; j++)
            {
                if(currentDemoAllChunks.size() > spectator->chunksWritten)
                {
                    QByteArray chunk = currentDemoAllChunks.value(spectator->chunksWritten);

                    spectator->write(chunk);
                    spectator->chunksWritten++;
                }
                else
                {
                    // The spectator has received all current chunks, stop buffering messages 
                    bufferingComplete++;

                    liveBuffering.remove(spectator->socketId);
                    liveOk.insert(spectator->socketId, spectator);

                    if(spectator->throttled)
                    {
                        spectator->throttled = false;

                        emit this->serveThrottledFinished();
                    }

                    break;
                }
            }
            spectator->idleSince = currentTime;
        }
        else
        {
            if(spectator->idleSince == 0)
            {
                spectator->idleSince = currentTime;
                continue;
            }


            qint64 idleTime = currentTime - spectator->idleSince;


            if(idleTime > 1000 * 30)
            {
                idleSpecs++;

                if(spectator->throttled)
                {
                    emit this->serveThrottledFinished();
                }

                liveBuffering.remove(spectator->socketId);
                spectator->abort();
            }
        }
    }


    if(bufferingComplete > 0)
    {
        qDebug() << this->objectName() << bufferingComplete << " Spectator(s) emptied their buffer, remove from manual buffering";
    }

    if(idleSpecs > 0)
    {
        qDebug() << this->objectName() << idleSpecs << " Spectator(s) idle for 30sec, disconnecting";
    }
}

void TcpConnectionWorker::newDemo(QByteArray data)
{
    currentDemoAllChunks.clear();
    currentDemoAllChunks.append(data);

    isStreaming = true;


    // Take everyone out of throttled status on new demos
    for(int i=0; i < throttled.size(); i++)
    {
        Spectator* spectator = throttled.value(i);


        int socketId = spectator->socketId;

        liveOk.insert(socketId, spectator);
    }

    throttled.clear();


    // Reset buffering on new demo
    QList<Spectator*> liveBufferingList = liveBuffering.values();

    for(int i=0; i < liveBufferingList.size(); i++)
    {
        Spectator* spectator = liveBufferingList.value(i);
        liveOk.insert(spectator->socketId, spectator);
    }

    liveBuffering.clear();


    // Write first chunk
    QList<Spectator*> spectators = spectatorList.values();

    for(int i=0; i < spectators.size(); i++)
    {
        Spectator* spectator = spectators.value(i);


        if(!spectator->bytesToWrite())
        {
            spectator->write(data);

            spectator->chunksWritten = 1;
        }
        else
        {
            spectator->chunksWritten = 0;
        }
    }
}
void TcpConnectionWorker::appendDemo(QByteArray data)
{
    currentDemoAllChunks.append(data);


    int bufferedSpecs = 0;

    QList<Spectator*> spectators = liveOk.values();

    for(int i=0; i < spectators.size(); i++)
    {
        Spectator* spectator = spectators.value(i);
        

        // Flooding the socket buffers burns too much memory, if spectators lag behind handle buffering manually
        if(spectator->bytesToWrite() > 4096)
        {
            liveOk.remove(spectator->socketId);
            liveBuffering.insert(spectator->socketId, spectator);

            bufferedSpecs++;

            continue;
        }

        spectator->write(data);
        spectator->chunksWritten++;
    }

    if(bufferedSpecs > 0)
    {
        qDebug() << bufferedSpecs << " Spectator(s) lagging behind, moved to buffering queue";
    }
}
void TcpConnectionWorker::finishDemo(QByteArray data)
{
    isStreaming = false;

    sendMessage(data);
}

void TcpConnectionWorker::setMotd(QByteArray _motdMessage)
{
    motdMessage = _motdMessage;
    sendMessage(motdMessage);
}
void TcpConnectionWorker::setBrowserPosition(QByteArray _browserPosition)
{
    browserPosition = _browserPosition;
    sendMessage(browserPosition);
}
void TcpConnectionWorker::setBrowserVideoUrl(QByteArray _browserVideoUrl)
{
    browserVideoUrl = _browserVideoUrl;
    sendMessage(browserVideoUrl);
}
void TcpConnectionWorker::setBrowserChatUrl(QByteArray _browserChatUrl)
{
    browserChatUrl = _browserChatUrl;
    sendMessage(browserChatUrl);
}

void TcpConnectionWorker::socketBytesWritten(qint64 bytes)
{
    // For statistics only
    bytesWrittenSinceDump += bytes;
}
void TcpConnectionWorker::dumpNetStats()
{
    if(bytesWrittenSinceDump == 0)
    {
        return;
    }


    emit this->bytesWritten(bytesWrittenSinceDump);

    bytesWrittenSinceDump = 0;
}

