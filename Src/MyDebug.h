#ifndef MYDEBUG_H
#define MYDEBUG_H

#include <QObject>
#include <QtCore>


class MyDebug : public QObject
{

    Q_OBJECT


public:

    MyDebug(QObject* parent = 0);
    ~MyDebug();


#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    static void debugHandler(QtMsgType type, const QMessageLogContext& context, const QString& message);
#else
    static void debugHandler(QtMsgType type, const char* message);
#endif


signals:

    void print(QString message);


};


extern MyDebug* GLOBALDEBUG;

#endif // MYDEBUG_H
