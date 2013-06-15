#include "MessageBuilder.h"
#include "FragEnums.h"


MessageBuilder::MessageBuilder() : compress(false), messageType(-1)
{
}
MessageBuilder::~MessageBuilder()
{
}

QByteArray MessageBuilder::generateXml(int name)
{
    QByteArray xmlBuffer;
    QXmlStreamWriter xml(&xmlBuffer);


    xml.writeStartElement("FragMessage");
    xml.writeAttribute("id", QString::number(name));


    foreach(Argument arg, args)
    {
        xml.writeStartElement("Arg");
        xml.writeAttribute("id", arg.name);

        xml.writeCharacters(arg.value);

        xml.writeEndElement();
    }


    xml.writeEndElement();


    return xmlBuffer;
}

QByteArray MessageBuilder::buildMessage(bool compress, QByteArray xml)
{
    if(compress)
    {
        xml = qCompress(xml, 9); 
    }

    QByteArray data;
    QDataStream dataStream(&data, QIODevice::WriteOnly);

    dataStream.setVersion(QDataStream::Qt_4_0);


    dataStream << (quint32)0; // Reserve a 4 byte buffer

    dataStream << (quint32)FRAGTV::Socket::Signature; 

    if(compress)
    {
        dataStream << (quint32)FRAGTV::Socket::Compressed;
    }
    else
    {
        dataStream << (quint32)FRAGTV::Socket::Uncompressed;
    }

    dataStream << (quint32)FRAGTV::Socket::Version; 
    dataStream << (quint32)FRAGTV::Socket::MinimumVersion; 


    dataStream.writeRawData(xml.constData(), xml.size()); 


    quint32 messageSize = (quint32)(data.size() - sizeof(quint32)); // Message size, subtract buffer size


    dataStream.device()->seek(0); // rewind to the 4 byte buffer

    dataStream << messageSize; // overwrite buffer with the size of the message, letting the client know how much data to receive before processing


    return data;
}


QByteArray MessageBuilder::generate()
{
    QByteArray xml     = generateXml(messageType);
    QByteArray message = buildMessage(compress, xml);

    return message;
}

void MessageBuilder::writeAttributes(int attributeName, QVariant attributeValue)
{
    Argument arg;

    arg.name  = QString::number(attributeName);
    arg.value = attributeValue.toString();

    args << arg;
}
