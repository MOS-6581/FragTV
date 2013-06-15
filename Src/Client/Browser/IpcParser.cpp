#include "IpcParser.h"

IpcParser::IpcParser(QObject *parent) : QObject(parent)
{
}
IpcParser::~IpcParser()
{
}


void IpcParser::parseMessage(QStringList message)
{
    QString messageType = message.takeFirst();


         if(messageType == "relayDebug")                qDebug() << message.first();

    else if(messageType == "chatUrl")                   emit this->chatUrl(message.first());
    else if(messageType == "writeChat")                 emit this->writeChat(message);


    else if(messageType == "videoUrl")                  emit this->videoUrl(message.first());
    else if(messageType == "videoPos")                  emit this->videoPos(message.first().toInt());

    else if(messageType == "decodeTimeout")             emit this->decodeTimeout(message.first().toInt());
    else if(messageType == "getVideoOffset")            emit this->getVideoOffset();
    else if(messageType == "videoOffset")               emit this->videoOffset(message.first().toInt());
}
