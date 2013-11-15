#include <QtCore>

#include "DediServerUI.h"


DediServerUI *DediServerUI::singletonInstance = NULL;

DediServerUI::DediServerUI(Persistence *aPersistence) 
	: persistence(aPersistence)
{
    singletonInstance = this;
}

DediServerUI::~DediServerUI()
{
    singletonInstance = NULL;
}

QString DediServerUI::getMotdHtml() const
{
    QString motdHtml;
    QFile motdHtmlFile(QDir::currentPath() + QDir::separator() + "motdHtml.html"); 
    if (motdHtmlFile.exists())
    {
        motdHtmlFile.open(QIODevice::ReadOnly);

        motdHtml = motdHtmlFile.readAll();
    }

    return motdHtml;
}