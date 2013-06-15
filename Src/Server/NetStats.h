#ifndef NETSTATS_H
#define NETSTATS_H

#include <QObject>
#include <QtCore>

class NetStats : public QObject
{

    Q_OBJECT


public:

    NetStats(QObject *parent);
    ~NetStats();


private:

    QTimer* statDumpTimer;
    qint64 bytesWrittenSinceDump;


public slots:

    void bytesWritten(qint64 bytes);
    void statsDump();

    
};

#endif // NETSTATS_H
