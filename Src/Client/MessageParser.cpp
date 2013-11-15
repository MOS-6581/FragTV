#include "MessageParser.h"
#include "FragEnums.h"

#include <QtCore>
#include <QXmlStreamAttributes>
#include <QXmlStreamAttribute>


MessageParser::MessageParser(QObject *parent) : QObject(parent)
{
}
MessageParser::~MessageParser()
{
}

void MessageParser::readMessage(bool compressed, QByteArray message)
{
    if(compressed)
    {
        message = qUncompress(message);
    }


    QXmlStreamReader* xml = new QXmlStreamReader(message);


    if(xml->hasError()) 
    {
        qDebug() << xml->errorString();


        delete xml;

        return;
    }


    if(!xml->readNextStartElement()) 
    {
        delete xml;

        return;
    }


    int messageName = xml->attributes().first().value().toString().toInt();


    if(messageName == FRAGTV::BrowserMessage)
    {
        parseBrowserMessage(xml);
    }
    else if(messageName == FRAGTV::DemoMessage)
    {
        parseDemoMessage(xml);
    }
    else if(messageName == FRAGTV::MotdMessage)
    {
        parseMotd(xml);
    }
    else
    {
        QString unknownMessage = xml->name().toString();

        qWarning() << "Unknown message data, possible version mismatch";
        qDebug() << unknownMessage;
    }

    delete xml;
}

void MessageParser::parseBrowserMessage(QXmlStreamReader* xml)
{
    xml->readNextStartElement();

    int attributeName = xml->attributes().first().value().toString().toInt();
    QString attributeValue = xml->readElementText();
    

    if(attributeName == FRAGTV::Browser::VideoPosition)
    {
        QStringList message("videoPos");
        message << attributeValue;

        emit this->browserMessage(message);
    }
    else if(attributeName == FRAGTV::Browser::VideoUrl)
    {
        QStringList message("videoUrl");
        message << attributeValue;

        emit this->browserMessage(message);
    }
    else if(attributeName == FRAGTV::Browser::ChatUrl)
    {
        QStringList message("chatUrl");
        message << attributeValue;

        emit this->browserMessage(message);
    }
    else
    {
        qDebug() << "Unknown browser message..";
    }
}
void MessageParser::parseDemoMessage(QXmlStreamReader* xml)
{
    if(!xml->readNextStartElement())
        return;


    int     attributeName  = xml->attributes().first().value().toString().toInt();
    QString attributeValue = xml->readElementText();


    if(attributeName == FRAGTV::Demo::New)
    {
        QString fileName = attributeValue;


        if(!xml->readNextStartElement())
            return;

        QByteArray fileContents = QByteArray::fromBase64(xml->readElementText().toLatin1());


        if(!xml->readNextStartElement())
            return;

        qint64 fileTimeStamp    = xml->readElementText().toLongLong();

        emit this->demoNew(fileName, fileContents, fileTimeStamp);
    }
    else if(attributeName == FRAGTV::Demo::Append)
    {
        QString fileName = attributeValue;


        if(!xml->readNextStartElement())
            return;

        QByteArray fileContents = QByteArray::fromBase64(xml->readElementText().toLatin1());


        emit this->demoAppend(fileName, fileContents);
    }
    else if(attributeName == FRAGTV::Demo::Finished)
    {
        emit this->demoFinish();
    }
    else
    {
        qDebug() << "Unknown demo message..";
    }
}
void MessageParser::parseMotd(QXmlStreamReader* xml)
{
    if(!xml->readNextStartElement())
        return;


    QString motdHtml = xml->readElementText();


    emit this->motd(motdHtml);
}
