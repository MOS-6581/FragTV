#include "ThreadManager.h"
#include "MyThread.h"


ThreadManager* GLOBALTHREADMANAGER = NULL;


ThreadManager::ThreadManager(QObject *parent) : QObject(parent)
{
    GLOBALTHREADMANAGER = this;


    shutdownProgressTimer = new QTimer(this);
    shutdownProgressTimer->setInterval(100);

    connect(shutdownProgressTimer , SIGNAL(timeout()), 
            this                  , SLOT(shutdownProgress()));
}
ThreadManager::~ThreadManager()
{
}

void ThreadManager::insertThread(MyThread* thread)
{
    allThreads.insert(thread->objectName(), thread);

    connect(thread , SIGNAL(finished()), 
            this   , SLOT(threadFinished()));
}

void ThreadManager::threadFinished()
{
    MyThread* thread = qobject_cast<MyThread*>(sender());

    QString threadName = thread->objectName();


    qDebug() << "Thread: " << threadName << " has finished";

    
    allThreads.remove(threadName);

    thread->deleteLater();
}

void ThreadManager::shutdown()
{
    shutdownProgressTimer->start();

    emit this->finishAllThreads();
}

void ThreadManager::shutdownProgress()
{
    if(!allThreads.isEmpty())
    {
        qDebug() << "ThreadManager shutdown in progress";
        

        foreach(MyThread* thread, allThreads.values())
        {
            qDebug() << "Waiting for " << thread->objectName();
        }
    }
    else
    {
        shutdownProgressTimer->stop();

        emit this->allThreadsFinished();
    }
}

