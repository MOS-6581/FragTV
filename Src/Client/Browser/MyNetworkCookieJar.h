#ifndef MYNETWORKCOOKIEJAR_H
#define MYNETWORKCOOKIEJAR_H

#include <QObject>
#include <QNetworkCookieJar>

class MyNetworkCookieJar : public QNetworkCookieJar
{

    Q_OBJECT


public:

    MyNetworkCookieJar(QObject* parent);
    ~MyNetworkCookieJar();
   

};

#endif // MYNETWORKCOOKIEJAR_H
