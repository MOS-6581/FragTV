#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <QObject>
#include <QtCore>
#include <QSettings>


class Persistence : public QObject
{

    Q_OBJECT


public:

    Persistence(QWidget* topWindow = 0);
    ~Persistence();

    QSettings* mySettings;


private:

    QWidget* topWindow;


public slots:

    bool isWindowVisible(QWidget* window);

    void saveWindow();
    void restoreWindow();

    void saveTheChildren(QObject* object);
    void restoreTheChildren(QObject* object);

    
};

#endif // PERSISTENCE_H
