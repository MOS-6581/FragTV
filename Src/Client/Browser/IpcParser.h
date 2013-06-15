#ifndef IPCPARSER_H
#define IPCPARSER_H

#include <QObject>
#include <QtCore>

class IpcParser : public QObject
{

    Q_OBJECT


public:

    IpcParser(QObject *parent);
    ~IpcParser();


public slots:

    void parseMessage(QStringList message);


signals: 

    void relayDebug(QString message);
    
    void chatUrl(QString url);
    void writeChat(QStringList chatLines); 


    void videoUrl(QString url);
    void videoPos(int position);

    void decodeTimeout(int);
    void getVideoOffset();
    void videoOffset(int offset); 


};

#endif // IPCPARSER_H
