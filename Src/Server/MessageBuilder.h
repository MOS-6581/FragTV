#ifndef MESSAGEBUILDER_H
#define MESSAGEBUILDER_H

#include <QObject>
#include <QtCore>


struct Argument
{
    QString name;
    QString value;
};


class MessageBuilder 
{


public:

    MessageBuilder();
    ~MessageBuilder();

    bool compress;
    int messageType;


    void writeAttributes(int attributeName, QVariant attributeValue);

    QByteArray generate();


private:

    QList<Argument> args;

    QByteArray generateXml(int name); 

    QByteArray buildMessage(bool compress, QByteArray xml);
    

};

#endif // MESSAGEBUILDER_H
