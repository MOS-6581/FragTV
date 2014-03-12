#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <QObject>
#include <QtCore>
#include <QSettings>


class Persistence : public QObject
{

    Q_OBJECT


public:

    Persistence();
#ifdef QT_GUI_LIB
    Persistence(QWidget* aTopWindow);
#endif

    ~Persistence();

    QSettings* mySettings;


private:

#ifdef QT_GUI_LIB
    QWidget* topWindow;
#endif

public slots:

#ifdef QT_GUI_LIB
    void setTopWindow(QWidget* aTopWindow) { topWindow = aTopWindow; }

    bool isWindowVisible(QWidget* window);

    void saveWindow();
    void restoreWindow();

    void saveTheChildren(QObject* object);
    void restoreTheChildren(QObject* object);
#endif

    bool getBoolSetting(const QString &name, bool defaultVal) const;
    QString getStringSetting(const QString &name, QString defaultVal) const;
    int getIntSetting(const QString &name, int defaultVal) const;
    void processCommandline(const QStringList &args);
    
};

#endif // PERSISTENCE_H
