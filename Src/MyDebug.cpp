#include "MyDebug.h"


MyDebug* GLOBALDEBUG = NULL;

MyDebug::MyDebug(QObject* parent) : QObject(parent)
{
    GLOBALDEBUG = this;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    qInstallMessageHandler(debugHandler);
#else
    qInstallMsgHandler(debugHandler);
#endif
}

MyDebug::~MyDebug()
{
    GLOBALDEBUG = NULL;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    qInstallMessageHandler(NULL);
#else
    qInstallMsgHandler(NULL);
#endif
}


#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
void MyDebug::debugHandler(QtMsgType type, const QMessageLogContext& context, const QString& message)
{
    Q_UNUSED(context);
#else
void MyDebug::debugHandler(QtMsgType type, const char* message)
{
#endif

    if(!GLOBALDEBUG)
        return;
    
    
    QString output = QDateTime::currentDateTime().toString("HH:mm:ss  "); 


    if(type      == QtWarningMsg)   output += "Warning";
    else if(type == QtCriticalMsg)  output += "Critical";
    else if(type == QtFatalMsg)     output += "Fatal";


    output += " " + QString(message);


    QMetaObject::invokeMethod(GLOBALDEBUG, "print", Qt::QueuedConnection, Q_ARG(QString, output));
}

