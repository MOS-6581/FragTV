#ifndef STREAMVIDEOWINDOW_H
#define STREAMVIDEOWINDOW_H

#include "MyWebView.h"


class StreamVideoWindow : public MyWebView
{

    Q_OBJECT


public:

    StreamVideoWindow(QWidget* parent = 0);
    ~StreamVideoWindow();


private:

    QImage screenshot1;
    QImage screenshot2;

    QRect windowSize;


public slots:

    void setWindowPosition(int position);

    QImage screenshot();

    void getOffset();
    void getOffsetFinish();


signals:

    void decodeTimeStamp(qint64 screenShotTime, QImage screenShot1, QImage screenShot2); 
    

};

#endif // STREAMVIDEOWINDOW_H
