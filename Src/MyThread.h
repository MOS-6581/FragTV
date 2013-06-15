#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QObject>
#include <QtCore>
#include <QThread>


class MyThread : public QThread
{

    Q_OBJECT


public:

    MyThread(QObject *object);
    ~MyThread();


private:

    QObject* parent;


public slots:

    void myQuit();


signals:

    void aboutToQuit();

    
};

#endif // MYTHREAD_H
