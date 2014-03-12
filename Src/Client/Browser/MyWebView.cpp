#include "MyWebView.h"
#include "MyNetworkCookieJar.h"
#include "Persistence.h"

#include <QWebSettings>
#include <QMessageBox>


MyWebView::MyWebView(QWidget* parent) : QWebView(parent), isCustomSizeActive(false)
{
    this->settings()->setAttribute(QWebSettings::JavascriptEnabled      , true);
    this->settings()->setAttribute(QWebSettings::PluginsEnabled         , true);
    this->settings()->setAttribute(QWebSettings::LocalStorageEnabled    , true);
    this->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled , true);


    trustedSites << "www.twitch.tv";
    trustedSites << "twitch.tv";


    persistence = new Persistence();


    saveDelay = new QTimer();
    saveDelay->setInterval(2000);
    saveDelay->setSingleShot(true);

    connect(saveDelay , SIGNAL(timeout()), 
            this      , SLOT(saveGeometry()));


    hideTitleDelay = new QTimer();
    hideTitleDelay->setInterval(5000);
    hideTitleDelay->setSingleShot(true);

    connect( hideTitleDelay , SIGNAL(timeout()),
             this           , SLOT(hideTitleBar()));
}
MyWebView::~MyWebView()
{
    saveGeometry();
}

void MyWebView::resizeEvent(QResizeEvent* ev)
{
    saveDelay->start();

    
    QWebView::resizeEvent(ev);
}
void MyWebView::closeEvent(QCloseEvent* ev)
{
    ev->ignore();

    this->showMinimized();
}

void MyWebView::enterEvent(QEvent *ev)
{
    hideTitleDelay->stop();

    this->clearMask();
    
    
    QWebView::enterEvent(ev); 
}
void MyWebView::leaveEvent(QEvent* ev)
{
    hideTitleDelay->start();
    
    
    QWebView::leaveEvent(ev); 
}
void MyWebView::hideTitleBar()
{
    this->setMask(QRegion(0, 0, this->width(), this->height()));
}

void MyWebView::saveGeometry()
{
    if(isCustomSizeActive)
    {
        QRect currentGeometry = this->geometry();
        persistence->mySettings->setValue("geometry", currentGeometry);
    }
}
void MyWebView::resetGeometry()
{
    setGeometry(50, 50, 300, 200);
}
void MyWebView::loadGeometry()
{    
    persistence->mySettings->beginGroup(this->objectName());

    QRect savedGeometry = persistence->mySettings->value("geometry").toRect();

    if(!savedGeometry.isNull())
    {
        isCustomSizeActive = true;
        this->setGeometry(savedGeometry);
    }
}

void MyWebView::safeLoad(QString urlString)
{
    QUrl url = QUrl(urlString);

    if(!url.isValid())
    {
        qDebug() << "Invalid url..";
        hide();

        return;
    }


    if(url.toString() == "http://")
    {
        load(QUrl(""));

        showMinimized();

        return;
    }


    if(!trustedSites.contains(url.host()))
    {
        int result = QMessageBox::warning(this, "Unknown stream provider", "Are you sure you want to open:\n" + url.toString(), QMessageBox::Open | QMessageBox::Cancel);

        if(result != QMessageBox::Open)
        {
            qDebug() << "Cancel URL load: " << url.toString();

            return;
        }
    }


    load(url);

    show();

    hideTitleDelay->start();


    qDebug() << "Loaded URL: " << url.toString();
}

