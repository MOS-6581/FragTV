#include "clientWindow.h"
#include "ui_clientWindow.h"
#include "TcpClient.h"
#include "Persistence.h"

#include <QSettings>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QDesktopServices>
#include <QDesktopWidget>


Ui::ClientWindow* CLIENTUI = NULL;

ClientWindow::ClientWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::ClientWindow)
{
    ui->setupUi(this);
    CLIENTUI = ui;


    persistence = new Persistence(this);
    persistence->restoreWindow();


    ui->networkStatusField->setText("");
    ui->wolfStatusField->setText("");
    ui->syncAutoDetectedDelaySpin->setValue(0);

    ui->versionLabel->setText(ui->versionLabel->text() + " " + QApplication::applicationVersion());


    connect(ui->dataMatrixTimeoutSpin , SIGNAL(valueChanged(int)) , this , SLOT(decodeTimeoutChanged(int)) );
}
ClientWindow::~ClientWindow()
{
    persistence->saveWindow();

    delete ui;
}

void ClientWindow::closeEvent(QCloseEvent* ev)
{
    ev->ignore();

    emit this->myQuit();
}

void ClientWindow::decodeTimeoutChanged(int timeoutLimit)
{
    QStringList message("decodeTimeout");
    message << QString::number(timeoutLimit);

    emit this->setDecodeTimeout(message);
}
