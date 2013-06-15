#include "Spectator.h"


Spectator::Spectator(QObject* parent) : QTcpSocket(parent), chunksWritten(0), socketId(0), throttled(false), idleSince(0)
{
}
Spectator::~Spectator()
{
}

