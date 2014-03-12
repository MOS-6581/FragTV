#ifndef MESSAGEPARSER_H
#define MESSAGEPARSER_H

#include <QObject>
#include <QtCore>
#include <QXmlStreamReader>


class MessageParser : public QObject
{

    Q_OBJECT


public:

    MessageParser(QObject* parent);
    ~MessageParser();


public slots:

    void readMessage(bool compressed, QByteArray message);

    void parseBrowserMessage(QXmlStreamReader* xml);
    void parseDemoMessage(QXmlStreamReader* xml);
    void parseMotd(QXmlStreamReader* xml);


signals:

#ifdef QT_GUI_LIB
    void browserMessage(QStringList message);
    void motd(QString html);
#else
    void cmdBrowserPosition(int position);
    void cmdMotd(QString html);
    void cmdChatUrl(QString url);
    void cmdVideoUrl(QString url);
#endif

    void demoNew(QString fileName, QByteArray contents, qint64 timeStamp);
    void demoAppend(QString fileName, QByteArray contents);
    void demoFinish();


};

#endif // MESSAGEPARSER_H
