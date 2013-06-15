#ifndef STREAMCHATWINDOW_H
#define STREAMCHATWINDOW_H

#include "MyWebView.h"

class StreamChatWindow : public MyWebView
{

    Q_OBJECT


public:

    StreamChatWindow(QWidget* parent = 0);
    ~StreamChatWindow();


private:

    QList<QString> oldLines;


public slots:

    void parseChat();
    void twitchOptimizations();


signals:

    void writeChat(QStringList chatLines);

    
};

#endif // STREAMCHATWINDOW_H
