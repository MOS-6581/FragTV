#ifndef MYTEXTBROWSER_H
#define MYTEXTBROWSER_H

#include <QTextBrowser>

class MyTextBrowser : public QTextBrowser
{

    Q_OBJECT


public:

    MyTextBrowser(QWidget *parent);
    ~MyTextBrowser();


public slots:

    void linkWarning(const QUrl name);

    
};

#endif // MYTEXTBROWSER_H
