#include "MyNetworkCookieJar.h"

#include <QtCore>
#include <QSettings>
#include <QNetworkCookieJar>
#include <QNetworkCookie>


MyNetworkCookieJar::MyNetworkCookieJar(QObject* parent) : QNetworkCookieJar(parent)
{
    QSettings settings;
    settings.beginGroup("savedCookies");

    QList<QNetworkCookie> parsedCookies;

    foreach(QString cookieName, settings.allKeys())
    {
        QByteArray cookieRawHexed = settings.value(cookieName).toByteArray();
        QByteArray cookieRaw      = QByteArray::fromHex(cookieRawHexed);

        parsedCookies.append(QNetworkCookie::parseCookies(cookieRaw));
    }

    setAllCookies(parsedCookies);
}

MyNetworkCookieJar::~MyNetworkCookieJar()
{
    QSettings settings;
    settings.beginGroup("savedCookies");


    foreach(QNetworkCookie cookie, allCookies())
    {
        QByteArray id        = QByteArray(cookie.domain().toLatin1() + cookie.name()).toHex();
        QByteArray rawCookie = cookie.toRawForm(QNetworkCookie::Full).toHex();

        settings.setValue(id, rawCookie);
    }
}
