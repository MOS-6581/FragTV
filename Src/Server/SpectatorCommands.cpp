#include "SpectatorCommands.h"
#include "MessageBuilder.h"
#include "FragEnums.h"

SpectatorCommands::SpectatorCommands(QObject *parent) : QObject(parent)
{
}
SpectatorCommands::~SpectatorCommands()
{
}

void SpectatorCommands::setBrowserPosition(int position)
{
    MessageBuilder builder;
    builder.messageType    = FRAGTV::BrowserMessage;
    builder.writeAttributes(FRAGTV::Browser::VideoPosition, position);

    QByteArray message     = builder.generate();    
    currentBrowserPosition = message;


    emit this->relayBrowserPosition(message);
}
void SpectatorCommands::setBrowserVideoUrl(QString url)
{
    MessageBuilder builder;
    builder.messageType    = FRAGTV::BrowserMessage;
    builder.writeAttributes(FRAGTV::Browser::VideoUrl, url);

    QByteArray message     = builder.generate();
    currentBrowserVideoUrl = message;


    qDebug() << "Video URL: " << url;

    emit this->relayBrowserVideoUrl(message);
}
void SpectatorCommands::setBrowserChatUrl(QString url)
{
    MessageBuilder builder;
    builder.messageType   = FRAGTV::BrowserMessage;
    builder.writeAttributes(FRAGTV::Browser::ChatUrl, url);

    QByteArray message    = builder.generate();
    currentBrowserChatUrl = message;


    qDebug() << "Chat URL: " << url;

    emit this->relayBrowserChatUrl(message);
}
void SpectatorCommands::setMotd(QString html)
{
    MessageBuilder builder;
    builder.messageType   = FRAGTV::MotdMessage;
    builder.writeAttributes(FRAGTV::Motd::Html, html);

    QByteArray message    = builder.generate();
    currentMotdMessage = message;


    emit this->relayMotdMessage(message);
}