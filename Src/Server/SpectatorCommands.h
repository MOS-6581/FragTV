#ifndef SPECTATORCOMMANDS_H
#define SPECTATORCOMMANDS_H

#include <QObject>

class SpectatorCommands : public QObject
{


    Q_OBJECT


public:

    SpectatorCommands(QObject *parent);
    ~SpectatorCommands();


    QByteArray currentBrowserVideoUrl;
    QByteArray currentBrowserChatUrl;
    QByteArray currentBrowserPosition;
    QByteArray currentMotdMessage;


public slots:

    void setBrowserPosition(int position);
    void setBrowserVideoUrl(QString url);
    void setBrowserChatUrl(QString url);
    void setMotd(QString html);


signals:

    void sendMessage(QByteArray message);

    void relayBrowserPosition(QByteArray BrowserPosition);
    void relayBrowserVideoUrl(QByteArray BrowserVideoUrl);
    void relayBrowserChatUrl(QByteArray BrowserChatUrl);

    void relayMotdMessage(QByteArray motdMessage);
    

};

#endif // SPECTATORCOMMANDS_H
