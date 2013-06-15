#ifndef SYNCHELPER_H
#define SYNCHELPER_H

#include "ui_syncHelper.h"

#include <QtCore>
#include <QWidget>


class Persistence;

class SyncHelper : public QWidget
{

    Q_OBJECT


public:

    SyncHelper();
    ~SyncHelper();


private:

    Ui::SyncHelperForm* syncHelperUi;
    QPoint oldPos;

    Persistence* persistence;


    QTimer* syncTimer;
    int timeStampScale;


    void initialize();


protected:

    void showEvent(QShowEvent* ev);
    void closeEvent(QCloseEvent* ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    

public slots:

    void updateTimeStamp();

    void setHelperStatus(bool state = true);
    void setAlwaysOnTop(int state);
    void setLocked(int state = 2);
    void setScale(int scale);

    
};

#endif // SYNCHELPER_H
