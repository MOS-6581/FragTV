#include "serverWindow.h"
#include "TcpListener.h"
#include "Persistence.h"
#include "FragEnums.h"

#include <QtCore>
#include <QFileDialog>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QDateTime>


Ui::ServerWindow* SERVERUI = NULL;
ServerWindow* SERVERMAIN = NULL;


ServerWindow::ServerWindow(Persistence *aPersistence) : QMainWindow(), ui(new Ui::ServerWindow), persistence(aPersistence)
{
    SERVERMAIN = this;

    ui->setupUi(this);
    SERVERUI = ui;

    persistence->setTopWindow(this);
    persistence->restoreWindow();

    connect(ui->demoPathBrowseButton    , SIGNAL(clicked())       , this , SLOT(getDemoFolderPath()) );
    connect(ui->htmlPreviewButton       , SIGNAL(clicked())       , this , SLOT(previewMotdHtml())   );
    connect(ui->htmlSaveButton          , SIGNAL(clicked())       , this , SLOT(previewMotdHtml())   );
    connect(ui->htmlSaveButton          , SIGNAL(clicked())       , this , SLOT(saveMotdHtml())      );

    connect(ui->browserNormalButton     , SIGNAL(clicked())       , this , SLOT(browser_normal())    );
    connect(ui->browserMaximizeButton   , SIGNAL(clicked())       , this , SLOT(browser_maximized()) );
    connect(ui->browserHideButton       , SIGNAL(clicked())       , this , SLOT(browser_hide())      );
    connect(ui->videoBrowserUrlGoButton , SIGNAL(clicked())       , this , SLOT(browser_url())       );
    connect(ui->videoBrowserUrlField    , SIGNAL(returnPressed()) , this , SLOT(browser_url())       );
    connect(ui->chatBrowserUrlGoButton  , SIGNAL(clicked())       , this , SLOT(browser_chatUrl())   );
    connect(ui->chatBrowserUrlField     , SIGNAL(returnPressed()) , this , SLOT(browser_chatUrl())   );


    setDemoScannerStatus(false);
    setNetworkListeningStatus(false);

    ui->demoStatusField->setText("");
    ui->networkStatusField->setText("");
    ui->connectedClientsSpin->setValue(0);

    ui->versionLabel->setText(ui->versionLabel->text() + " " + QApplication::applicationVersion());


    QFile* motdHtmlFile = new QFile(QDir::currentPath() + QDir::separator() + "motdHtml.html"); 

    if(motdHtmlFile->exists())
    {
        motdHtmlFile->open(QIODevice::ReadOnly);

        QString motdHtml = motdHtmlFile->readAll();

        SERVERUI->motdHtmlEditField->setPlainText(motdHtml);
        SERVERUI->motdHtmlPreview->setHtml(motdHtml);
    }
}
ServerWindow::~ServerWindow()
{
    persistence->saveWindow();

    delete ui;
}

void ServerWindow::closeEvent(QCloseEvent* ev)
{
    ev->ignore();

    emit this->myQuit();
}

void ServerWindow::getDemoFolderPath()
{
    QString currentPath = SERVERUI->demoPathField->text();

    QString demoPath = QFileDialog::getExistingDirectory(this, "Locate the demo folder to monitor", currentPath);


    if(demoPath.isNull())
    {
        return;
    }

    SERVERUI->demoPathField->setText(demoPath);
    SERVERUI->demoStatusField->setText("");
}

void ServerWindow::setNetworkListeningStatus(bool state)
{
    int index = SERVERUI->tabWidget->indexOf(SERVERUI->networkTab);

    if(state)
    {
        SERVERUI->tabWidget->setTabIcon(index, QPixmap(":/network/Green Ball.ico"));

        SERVERUI->listenButton->setEnabled(false);
        SERVERUI->disconnectButton->setEnabled(true);
    }
    else
    {
        SERVERUI->tabWidget->setTabIcon(index, QPixmap(":/network/Red Ball.ico"));

        SERVERUI->listenButton->setEnabled(true);
        SERVERUI->disconnectButton->setEnabled(false);
    }
}
void ServerWindow::setDemoScannerStatus(bool state)
{
    int index = SERVERUI->tabWidget->indexOf(SERVERUI->gameTab);

    if(state)
    {
        SERVERUI->tabWidget->setTabIcon(index, QPixmap(":/network/Green Ball.ico"));

        SERVERUI->scannerStartButton->setEnabled(false);
        SERVERUI->scannerStopButton->setEnabled(true);
    }
    else
    {
        SERVERUI->tabWidget->setTabIcon(index, QPixmap(":/network/Red Ball.ico"));

        SERVERUI->scannerStartButton->setEnabled(true);
        SERVERUI->scannerStopButton->setEnabled(false);
    }
}

void ServerWindow::previewMotdHtml()
{
    QString html = SERVERUI->motdHtmlEditField->toPlainText();

    SERVERUI->motdHtmlPreview->setHtml(html);
}
void ServerWindow::saveMotdHtml()
{
    QString html = SERVERUI->motdHtmlEditField->toPlainText();

    QString path = QDir::currentPath() + QDir::separator() + "motdHtml.html";

    QFile* motdMessage = new QFile(path);


    if(motdMessage->open(QIODevice::ReadWrite))
    {
        motdMessage->write(html.toLatin1());

        motdMessage->close();
    }
    else
    {
        qDebug() << "Failed to write html file: " << motdMessage->errorString();
    }


    emit this->cmdMotd(html);
}

void ServerWindow::browser_hide()
{
    SERVERUI->browserHideButton->setEnabled(false);
    SERVERUI->browserNormalButton->setEnabled(true);
    SERVERUI->browserMaximizeButton->setEnabled(true);

    emit this->cmdBrowserPosition(FRAGTV::Browser::VideoHide);
}
void ServerWindow::browser_maximized()
{
    SERVERUI->browserHideButton->setEnabled(true);
    SERVERUI->browserNormalButton->setEnabled(true);
    SERVERUI->browserMaximizeButton->setEnabled(false);

    emit this->cmdBrowserPosition(FRAGTV::Browser::VideoMaximize);
}
void ServerWindow::browser_normal()
{
    SERVERUI->browserHideButton->setEnabled(true);
    SERVERUI->browserNormalButton->setEnabled(false);
    SERVERUI->browserMaximizeButton->setEnabled(true);

    emit this->cmdBrowserPosition(FRAGTV::Browser::VideoNormal);
}
void ServerWindow::browser_url()
{
    SERVERUI->browserHideButton->setEnabled(true);
    SERVERUI->browserNormalButton->setEnabled(false);
    SERVERUI->browserMaximizeButton->setEnabled(true);

    QString urlRaw = SERVERUI->videoBrowserUrlField->text();
    QUrl url = QUrl(urlRaw);

    if(url.scheme().isEmpty())
    {
        qDebug() << "Video url: no scheme set.. prepending http";

        url = QUrl("http://" + urlRaw);
    }

    if(!url.isValid())
    {
        QMessageBox::warning(0, "Invalid URL", url.toString() + " " + url.errorString());

        return;
    }


    SERVERUI->videoBrowserUrlField->setText(url.toString());

    emit this->cmdVideoUrl(url.toString());
}
void ServerWindow::browser_chatUrl()
{
    QString urlRaw = SERVERUI->chatBrowserUrlField->text();
    QUrl url = QUrl(urlRaw);

    if(url.scheme().isEmpty())
    {
        qDebug() << "Chat url: no scheme set.. prepending http";

        url = QUrl("http://" + urlRaw);
    }

    if(!url.isValid())
    {
        QMessageBox::warning(0, "Invalid URL", url.toString() + " " + url.errorString());

        return;
    }

    SERVERUI->chatBrowserUrlField->setText(url.toString());

    emit this->cmdChatUrl(url.toString());
}
