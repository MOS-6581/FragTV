#ifndef CHANNELMANAGER_H
#define CHANNELMANAGER_H

#include <QObject>
#include <QtCore>

class QListWidget;
class QListWidgetItem;


class ChannelManager : public QObject
{

    Q_OBJECT


public:

    ChannelManager(QObject* parent = 0);
    ~ChannelManager();


private:

    QListWidget* channelList;

    QTime antiHammer;


public slots:

    void populateList();
    void saveChannel();
    void deleteChannel();

    void connectChannel(QListWidgetItem* item);
    void selectChannel(QListWidgetItem* item);
    void selectionChanged();


signals:

    void channelConnect();

    
};

#endif // CHANNELMANAGER_H
