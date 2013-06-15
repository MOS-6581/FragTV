#include "NetStats.h"

NetStats::NetStats(QObject *parent) : QObject(parent), bytesWrittenSinceDump(0)
{
    // Dump performance stats to console
    statDumpTimer = new QTimer(this);
    statDumpTimer->start(5000);

    connect( statDumpTimer, SIGNAL(timeout()), 
             this, SLOT(statsDump()));
}
NetStats::~NetStats()
{
}


void NetStats::statsDump()
{
    if(bytesWrittenSinceDump == 0)
    {
        return;
    }

    if(bytesWrittenSinceDump < 1024 * 5)
    {
        qint64 Bps = bytesWrittenSinceDump / 5;
        qDebug() << "Netstats:  " << Bps << " B/s  " << bytesWrittenSinceDump << " bytes (5sec)";
    }
    else if(bytesWrittenSinceDump < 1024 * 1024 * 5)
    {
        qint64 KBps    = bytesWrittenSinceDump / 1024 / 5;
        qint64 KBTotal = bytesWrittenSinceDump / 1024;

        qDebug() << "Netstats:  " << KBps << " KB/s  " << KBTotal << " KB (5sec)";
    }
    else
    {
        qint64 MBps    = bytesWrittenSinceDump / 1024 / 1024 / 5;
        qint64 MBTotal = bytesWrittenSinceDump / 1024 / 1024;

        qDebug() << "Netstats:  " << MBps << " MB/s  " << MBTotal << " MB (5sec)";
    }


    bytesWrittenSinceDump = 0;
}
void NetStats::bytesWritten(qint64 bytes)
{
    bytesWrittenSinceDump += bytes;
}
