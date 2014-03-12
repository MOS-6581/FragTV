#include "MessageParser.h"
#include "FragEnums.h"

#include <QtCore>
#include <QXmlStreamAttributes>
#include <QXmlStreamAttribute>

#ifndef QT_GUI_LIB
#include "DediServerUI.h"
#endif

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
#ifdef QT_GUI_LIB
        QStringList message("videoPos");
        message << attributeValue;

        emit this->browserMessage(message);
#else
        emit this->cmdBrowserPosition(attributeValue.toInt());
#endif
    }
    else if(attributeName == FRAGTV::Browser::VideoUrl)
    {
#ifdef QT_GUI_LIB
        QStringList message("videoUrl");
        message << attributeValue;

        emit this->browserMessage(message);
#else
        emit this->cmdVideoUrl(attributeValue);
#endif
    }
    else if(attributeName == FRAGTV::Browser::ChatUrl)
    {
#ifdef QT_GUI_LIB
        QStringList message("chatUrl");
        message << attributeValue;

        emit this->browserMessage(message);
#else
	emit this->cmdChatUrl(attributeValue);
#endif
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

#ifdef QT_GUI_LIB
    emit this->motd(motdHtml);
#else
    emit this->cmdMotd(motdHtml);
#endif
}
