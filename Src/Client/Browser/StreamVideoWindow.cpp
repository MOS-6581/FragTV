#include "StreamVideoWindow.h"
#include "FragEnums.h"
#include "MyThread.h"

#include <QWebFrame>
#include <QDesktopWidget>


StreamVideoWindow::StreamVideoWindow(QWidget* parent) : MyWebView(parent)
{
    this->setObjectName("StreamVideoWindow");

    this->setWindowTitle("Video");
    this->setWindowFlags(Qt::WindowStaysOnTopHint);


    loadGeometry();

    if(!isCustomSizeActive)
    {
        QDesktopWidget desktop;
        QRect desktopSize = desktop.screenGeometry();

        this->setGeometry(desktopSize.width()-660, desktopSize.height()-400, 660, 400);

        isCustomSizeActive = true;
    }
}
StreamVideoWindow::~StreamVideoWindow()
{
}


void StreamVideoWindow::setWindowPosition(int position)
{
    if(position == FRAGTV::Browser::VideoHide)
    {
        if(isCustomSizeActive)
        {
            windowSize = this->geometry();
        }

        this->isCustomSizeActive = false;

        this->lower();
        this->hide();


        qDebug() << "Video Browser hide";
    }
    else if(position == FRAGTV::Browser::VideoNormal)
    {
        this->isCustomSizeActive = true;


        this->clearMask();

        this->setWindowFlags(Qt::WindowStaysOnTopHint);
        this->raise();
        this->activateWindow();
        this->showNormal();

        if(!windowSize.isNull() && windowSize.isValid())
        {
            this->setGeometry(windowSize);
        }

        this->hideTitleBar();


        qDebug() << "Video Browser show normal";
    }
    else if(position == FRAGTV::Browser::VideoMaximize)
    { 
        if(isCustomSizeActive)
        {
            windowSize = this->geometry();
        }

        this->isCustomSizeActive = false;


        this->clearMask();

        this->setWindowFlags(Qt::WindowStaysOnTopHint);
        this->raise();
        this->activateWindow();
        this->showMaximized();

        qDebug() << "Video Browser show maximized";
    }
}

QImage StreamVideoWindow::screenshot()
{
    QWebFrame* webFrame = this->page()->mainFrame();

    QImage image = QImage(webFrame->geometry().width(), webFrame->geometry().height(), QImage::Format_ARGB32);

    QPainter painter(&image);

    webFrame->render(&painter);


    return image;
}

void StreamVideoWindow::getOffset()
{
    screenshot1 = screenshot();

    // Wait 200ms then take another screenshot to make sure the video isn't frozen
    QTimer::singleShot(200, this, SLOT(getOffsetFinish()));
}
void StreamVideoWindow::getOffsetFinish()
{
    qint64 screenshotTime = QDateTime::currentMSecsSinceEpoch();
    QImage screenshot2 = screenshot();

    emit this->decodeTimeStamp(screenshotTime, screenshot1, screenshot2);
}

