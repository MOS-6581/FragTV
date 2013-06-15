#ifndef THREADMANAGER_H
#define THREADMANAGER_H

#include <QObject>
#include <QtCore>

class MyThread;


class ThreadManager : public QObject
{

    Q_OBJECT


public:

    ThreadManager(QObject *parent = 0);
    ~ThreadManager();


private:

    QHash<QString,MyThread*> allThreads;

    QTimer* shutdownProgressTimer;


public slots:

    void insertThread(MyThread* thread);
    void threadFinished(); 
    
    void shutdown();
    void shutdownProgress();


signals:

    void finishAllThreads();
    void allThreadsFinished();

    
};

extern ThreadManager* GLOBALTHREADMANAGER;

#endif // THREADMANAGER_H
