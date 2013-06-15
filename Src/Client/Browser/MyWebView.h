#ifndef MYWEBVIEW_H
#define MYWEBVIEW_H

#include <QtCore>
#include <QWebView>
#include <QCloseEvent>

class Persistence;

class MyWebView : public QWebView
{

    Q_OBJECT


public:

    MyWebView(QWidget* parent = 0);
    ~MyWebView();

    bool isCustomSizeActive;


private:

    Persistence* persistence;
    QTimer* saveDelay;
    QTimer* hideTitleDelay;

    QStringList trustedSites;


protected:

    void resizeEvent(QResizeEvent* ev);
    void closeEvent(QCloseEvent* ev);
    void enterEvent(QEvent *ev);
    void leaveEvent(QEvent* ev);


public slots:

    void hideTitleBar();

    void saveGeometry();
    void resetGeometry();
    void loadGeometry();

    void safeLoad(QString urlString);

    
};

#endif // MYWEBVIEW_H
