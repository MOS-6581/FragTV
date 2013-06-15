#include "ChannelManager.h"
#include "clientWindow.h"
#include "WidgetFlash.h"

#include <QSettings>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>
#include <QDateTime>
#include <QDesktopServices>
#include <QListWidget>
#include <QListWidgetItem>

ChannelManager::ChannelManager(QObject* parent) : QObject(parent)
{
    channelList = CLIENTUI->streamChannelList;

    populateList();


    connect(CLIENTUI->streamSaveButton   , SIGNAL(clicked())                           , this , SLOT(saveChannel())                    );
    connect(CLIENTUI->streamDeleteButton , SIGNAL(clicked())                           , this , SLOT(deleteChannel())                  );

    connect(CLIENTUI->streamChannelList  , SIGNAL(itemSelectionChanged())              , this , SLOT(selectionChanged())               );
    connect(CLIENTUI->streamChannelList  , SIGNAL(itemClicked(QListWidgetItem*))       , this , SLOT(selectChannel(QListWidgetItem*))  );
    connect(CLIENTUI->streamChannelList  , SIGNAL(itemDoubleClicked(QListWidgetItem*)) , this , SLOT(connectChannel(QListWidgetItem*)) );
}
ChannelManager::~ChannelManager()
{
}

void ChannelManager::populateList()
{
    QSettings settings;
    QString selectedChannelName = settings.value("STREAMLISTSELECTED").toString();

    settings.beginGroup("STREAMLIST");


    foreach(QString channelName, settings.allKeys())
    {
        QListWidgetItem* newItem = new QListWidgetItem();

        QString channelHost = settings.value(channelName).toString();

        newItem->setData(0, channelName);
        newItem->setData(1, channelHost);

        channelList->insertItem(channelList->count(), newItem);

        if(channelName == selectedChannelName)
        {
            channelList->setItemSelected(newItem, true);
            selectChannel(newItem);
        }
    }
}
void ChannelManager::saveChannel()
{
    QString channelName = CLIENTUI->serverNameField->text();
    QString channelHost = CLIENTUI->serverAddressField->text();

    if(channelName.isEmpty())
    {
        CLIENTUI->networkStatusField->setText("Enter a name!");
        WidgetFlash widgetFlash;
        widgetFlash.red(CLIENTUI->networkStatusField);

        return;
    }


    QList<QListWidgetItem*> searchResults = channelList->findItems(channelName, Qt::MatchFixedString);

    if(!searchResults.isEmpty()) // Already exists
    {
        CLIENTUI->networkStatusField->setText("Channel name must be unique");
        WidgetFlash widgetFlash;
        widgetFlash.red(CLIENTUI->networkStatusField);

        return; 
    }


    QListWidgetItem* newItem = new QListWidgetItem();

    newItem->setData(0, channelName);
    newItem->setData(1, channelHost);

    channelList->insertItem(channelList->count(), newItem);
    channelList->sortItems();
    channelList->scrollToBottom();


    QSettings settings;
    settings.beginGroup("STREAMLIST");

    settings.setValue(channelName, channelHost);


    CLIENTUI->networkStatusField->setText("Saved: " + channelName);
    WidgetFlash widgetFlash;
    widgetFlash.green(CLIENTUI->networkStatusField);
}
void ChannelManager::deleteChannel()
{
    QString channelName = CLIENTUI->serverNameField->text();
    QString channelHost = CLIENTUI->serverAddressField->text();

    if(channelName.isEmpty())
    {
        return;
    }


    QList<QListWidgetItem*> items = channelList->findItems(channelName, Qt::MatchFixedString);

    foreach(QListWidgetItem* item, items)
    {
        delete item;

        CLIENTUI->networkStatusField->setText("Deleted: " + channelName);
        WidgetFlash widgetFlash;
        widgetFlash.green(CLIENTUI->networkStatusField);
    }


    QSettings settings;
    settings.beginGroup("STREAMLIST");

    settings.remove(channelName);
}

void ChannelManager::connectChannel(QListWidgetItem* item)
{
    selectChannel(item);


    if(antiHammer.isValid() && antiHammer.elapsed() < 5000)
    {
        QString timeleft = QString::number(5000 - antiHammer.elapsed());

        CLIENTUI->networkStatusField->setText("Wait " + timeleft + "ms");
        WidgetFlash widgetFlash;
        widgetFlash.yellow(CLIENTUI->networkStatusField);
        
        return;
    }

    antiHammer.restart();


    emit this->channelConnect();
}
void ChannelManager::selectChannel(QListWidgetItem* item)
{
    QString channelName = item->data(0).toString();
    QString channelHost = item->data(1).toString();

    CLIENTUI->serverNameField->setText(channelName);
    CLIENTUI->serverAddressField->setText(channelHost);

    QSettings settings;
    settings.setValue("STREAMLISTSELECTED", channelName);
}
void ChannelManager::selectionChanged()
{
    selectChannel(channelList->selectedItems().value(0));
}

