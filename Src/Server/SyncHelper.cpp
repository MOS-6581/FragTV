#include "SyncHelper.h"
#include "serverWindow.h"
#include "Persistence.h"

#include "dmtx.h"

#include <QtCore>
#include <QTimer>
#include <QDateTime>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QFlags>
#include <QBitmap>


SyncHelper::SyncHelper()
{
    syncHelperUi = new Ui::SyncHelperForm();
    syncHelperUi->setupUi(this);


    persistence = new Persistence(this);
    persistence->restoreWindow();


    // Generate new barcode image every 100ms
    syncTimer = new QTimer(this);
    syncTimer->setInterval(100);


    // Show/Hide, locked, scale 
    initialize();


    connect(syncTimer                 , SIGNAL(timeout())         , this      , SLOT(updateTimeStamp())   );
    connect(SERVERUI->syncShow        , SIGNAL(clicked())         , this      , SLOT(show())              );
    connect(SERVERUI->syncShow        , SIGNAL(clicked())         , syncTimer , SLOT(start())             );
    connect(SERVERUI->syncShow        , SIGNAL(clicked())         , this      , SLOT(setHelperStatus())   );
    connect(SERVERUI->syncClose       , SIGNAL(clicked())         , this      , SLOT(close())             );
    connect(SERVERUI->syncClose       , SIGNAL(clicked())         , syncTimer , SLOT(stop())              );
    connect(SERVERUI->syncAlwaysOnTop , SIGNAL(stateChanged(int)) , this      , SLOT(setAlwaysOnTop(int)) );
    connect(SERVERUI->syncLocked      , SIGNAL(stateChanged(int)) , this      , SLOT(setLocked(int))      );
    connect(SERVERUI->syncScale       , SIGNAL(valueChanged(int)) , this      , SLOT(setScale(int))       );
}
SyncHelper::~SyncHelper()
{
    persistence->saveWindow();
}

void SyncHelper::initialize()
{
    if(SERVERUI->syncAlwaysOnTop->isChecked())
    {
        this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
        SERVERUI->syncAlwaysOnTop->setChecked(true);
    }
    else
    {
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
    }
    
    timeStampScale = SERVERUI->syncScale->value();

    if(SERVERUI->syncLocked->isChecked())
    {
        updateTimeStamp();


        // Window raise animations can screw up the mask, don't lock until its in the final position
        QTimer* waitForWinAnimation = new QTimer(this);

        connect(waitForWinAnimation , SIGNAL(timeout()), 
                this                , SLOT(setLocked()));

        waitForWinAnimation->setSingleShot(true);
        waitForWinAnimation->start(1000);
    }

    if(SERVERUI->syncShowOnStartup->isChecked())
    {
        this->show();

        syncTimer->start();

        setHelperStatus(true);
    }
    else
    {
        setHelperStatus(false);
    }
}

void SyncHelper::showEvent(QShowEvent* ev)
{
    if(SERVERUI->syncLocked->isChecked())
    {
        updateTimeStamp();


        // Window raise animations can screwup the mask, don't lock until its in the final position
        QTimer* waitForWinAnimation = new QTimer(this);

        connect(waitForWinAnimation , SIGNAL(timeout()), 
                this                , SLOT(setLocked()));

        waitForWinAnimation->setSingleShot(true);
        waitForWinAnimation->start(1000);
    }


    QWidget::showEvent(ev);
}
void SyncHelper::closeEvent(QCloseEvent* ev)
{    
    setHelperStatus(false);


    QWidget::closeEvent(ev);
}
void SyncHelper::mousePressEvent(QMouseEvent* ev)
{
    // Move window 
    oldPos = ev->globalPos();


    QWidget::mousePressEvent(ev);
}
void SyncHelper::mouseMoveEvent(QMouseEvent* ev)
{
    // Move window 
    QPoint posDiff = ev->globalPos() - oldPos;

    this->move(this->x()+posDiff.x(), this->y()+posDiff.y());

    oldPos = ev->globalPos();


    QWidget::mouseMoveEvent(ev);
}

void SyncHelper::updateTimeStamp()
{
    qint64 time = QDateTime::currentMSecsSinceEpoch(); 
    QByteArray timeString = QByteArray::number(time);


    DmtxEncode* encoder = dmtxEncodeCreate();
    dmtxEncodeSetProp(encoder, DmtxPropPixelPacking, DmtxPack32bppRGBX);

    dmtxEncodeDataMatrix(encoder, timeString.length(), (unsigned char*)timeString.constData());

    QImage result(encoder->image->pxl, encoder->image->width, encoder->image->height, QImage::Format_RGB32);
    QImage scaledImage = result.scaledToHeight(1.4 * timeStampScale);

    dmtxEncodeDestroy(&encoder); //cleanup


    QPixmap pixmap = QPixmap::fromImage(scaledImage, Qt::MonoOnly);

    syncHelperUi->dataMatrixLabel->setPixmap(pixmap);
}

void SyncHelper::setHelperStatus(bool state)
{
    int index = SERVERUI->tabWidget->indexOf(SERVERUI->syncTab);

    if(state)
    {
        SERVERUI->tabWidget->setTabIcon(index, QPixmap(":/network/Green Ball.ico"));

        SERVERUI->syncShow->setEnabled(false);
        SERVERUI->syncClose->setEnabled(true);
    }
    else
    {
        SERVERUI->tabWidget->setTabIcon(index, QPixmap(":/network/Red Ball.ico"));

        SERVERUI->syncShow->setEnabled(true);
        SERVERUI->syncClose->setEnabled(false);
    }
}
void SyncHelper::setAlwaysOnTop(int state)
{
    bool visible = this->isVisible();
 

    if(state == 0)
    {
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
    }
    else
    {
        this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
    }


    if(visible)
    {
        this->show();
    }
}
void SyncHelper::setLocked(int state)
{
    if(state == 0)
    {
        this->clearMask();
    }
    else
    {
        this->setMask(syncHelperUi->dataMatrixLabel->geometry());
    }
}
void SyncHelper::setScale(int scale)
{
    timeStampScale = scale;


    updateTimeStamp();


    if((scale*1.4) > this->geometry().width())
    {
        this->resize(scale*1.4, scale*1.4);
    }


    if(SERVERUI->syncLocked->isChecked())
    {
        this->setMask(syncHelperUi->dataMatrixLabel->geometry());
    }
}

