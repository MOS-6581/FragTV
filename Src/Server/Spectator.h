#ifndef SPECTATOR_H
#define SPECTATOR_H

#include <QTcpSocket>


class Spectator : public QTcpSocket
{

    Q_OBJECT


public:

    Spectator(QObject* parent = 0);
    ~Spectator();

    int chunksWritten;
    int socketId;

    bool throttled;

    qint64 idleSince;
    
};

#endif // SPECTATOR_H
