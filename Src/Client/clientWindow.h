#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_clientWindow.h"

#include <QObject>
#include <QMainWindow>

class Persistence;


class ClientWindow : public QMainWindow
{

    Q_OBJECT

    
public:

    explicit ClientWindow(QWidget *parent = 0);
    ~ClientWindow();


private:

    Ui::ClientWindow *ui;

    Persistence* persistence;


protected:

    void closeEvent(QCloseEvent* ev);


public slots:

    void decodeTimeoutChanged(int timeoutLimit);


signals:

    void myQuit();

    void setDecodeTimeout(QStringList message);


};


extern Ui::ClientWindow* CLIENTUI;

#endif // MAINWINDOW_H
