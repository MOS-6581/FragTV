#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#ifdef QT_GUI_LIB

#include "ui_serverWindow.h"

#include <QMainWindow>

class Persistence;
class FragServer;


class ServerWindow : public QMainWindow
{

    Q_OBJECT
    

public:

    explicit ServerWindow(Persistence *aPersistence);
    ~ServerWindow();


protected:

    void closeEvent(QCloseEvent* ev);

    
private:

    Ui::ServerWindow* ui;

    Persistence* persistence;


public slots:

    void getDemoFolderPath();

    void setNetworkListeningStatus(bool state);
    void setDemoScannerStatus(bool state);

    void previewMotdHtml();
    void saveMotdHtml();

    void browser_hide();
    void browser_maximized();
    void browser_normal();
    void browser_url();
    void browser_chatUrl();


signals:

    void myQuit();


    void cmdMotd(QString html);
    void cmdBrowserPosition(int position);
    void cmdVideoUrl(QString url);
    void cmdChatUrl(QString url);


};

extern Ui::ServerWindow* SERVERUI;
extern ServerWindow* SERVERMAIN;

#else

#include "DediServerUI.h"

#endif


#endif // MAINWINDOW_H
