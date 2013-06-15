#include "MyTextBrowser.h"

#include <QtGui>
#include <QMessageBox>
#include <QDesktopServices>


MyTextBrowser::MyTextBrowser(QWidget* parent) : QTextBrowser(parent)
{
    connect(this , SIGNAL(anchorClicked(const QUrl)), 
            this , SLOT(linkWarning(const QUrl)));
}
MyTextBrowser::~MyTextBrowser()
{
}

void MyTextBrowser::linkWarning(const QUrl name)
{
    if(!name.isValid())
    {
        qDebug() << "URL error: " << name.errorString();

        return;
    }


    int result = QMessageBox::warning(0, "Open with default browser", "Are you sure you want to open:\n" + name.toString(), QMessageBox::Open | QMessageBox::Cancel);

    if(result == QMessageBox::Open)
    {
        QDesktopServices::openUrl(name);

        qDebug() << "URL open: " << name.toString();
    }
    else
    {
        qDebug() << "Ignore URL: " << name.toString();
    }
}
