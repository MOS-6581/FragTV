#include "MyThread.h"
#include "ThreadManager.h"


MyThread::MyThread(QObject *object) : QThread(), parent(object)
{
    QString threadName = object->objectName() + "Thread";

    this->setObjectName(threadName);


    GLOBALTHREADMANAGER->insertThread(this);


    // Self kill command from the thread manager to all threads
    connect(GLOBALTHREADMANAGER , SIGNAL(finishAllThreads()), 
            this                , SLOT(myQuit()),
            Qt::QueuedConnection);


    // Make sure the thread cleans up before terminating
    connect(this   , SIGNAL(aboutToQuit()), 
            object , SLOT(deleteLater()), 
            Qt::BlockingQueuedConnection);
}
MyThread::~MyThread()
{
}

void MyThread::myQuit()
{
    emit this->aboutToQuit(); // Blocking connection will allow the deconstructor to finish before continuing

    quit();
}
