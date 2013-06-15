#include "MyNetworkAccessManager.h"
#include "MyThread.h"

MyNetworkAccessManager::MyNetworkAccessManager() : QNetworkAccessManager()
{
    this->setObjectName("MyNetworkAccessManager");

    MyThread* myThread = new MyThread(this);

    this->moveToThread(myThread);

    myThread->start();
}

MyNetworkAccessManager::~MyNetworkAccessManager()
{
}
