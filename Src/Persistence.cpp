#include "Persistence.h"

#include <QtCore>
#include <QSettings>

#ifdef QT_GUI_LIB

#include <QtGui>
#include <QCheckBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDesktopServices>
#include <QDesktopWidget>
#endif

Persistence::Persistence()
{
    mySettings = new QSettings();
}

Persistence::~Persistence()
{
    mySettings->deleteLater();
}

#ifdef QT_GUI_LIB

Persistence::Persistence(QWidget* aTopWindow) : topWindow(aTopWindow)
{
    mySettings = new QSettings();
}

void Persistence::saveWindow()
{
    if(isWindowVisible(topWindow))
    {
        QRect currentRect = topWindow->geometry();
        mySettings->setValue(topWindow->objectName() + "geometry", currentRect);
    }

    saveTheChildren(topWindow);
}
void Persistence::restoreWindow()
{
    if(mySettings->contains(topWindow->objectName() + "geometry"))
    {
        QRect rect = mySettings->value(topWindow->objectName() + "geometry").toRect();

        if(!rect.isNull() && rect.isValid())
        {
            topWindow->setGeometry(rect);
        }
    }

    restoreTheChildren(topWindow);
}

void Persistence::saveTheChildren(QObject* object)
{
    QString name = object->objectName(); 

    QCheckBox* checkbox = qobject_cast<QCheckBox*>(object);
    if(checkbox)
    {
        bool value = checkbox->isChecked();
        mySettings->setValue(name, value); 
    }

    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(object);
    if(lineEdit)
    {
        QString value = lineEdit->text();
        mySettings->setValue(name, value);
    }

    QSpinBox* spinBox = qobject_cast<QSpinBox*>(object);
    if(spinBox)
    {
        int value = spinBox->value();
        mySettings->setValue(name, value);
    }

    foreach(QObject* child, object->children())
    {
        saveTheChildren(child);
    }
}
void Persistence::restoreTheChildren(QObject* object)
{
    QString name = object->objectName(); 

    QCheckBox* checkbox = qobject_cast<QCheckBox*>(object);
    if(checkbox)
    {
        bool value = mySettings->value(name).toBool();
        if(mySettings->contains(name))
        {
            checkbox->setChecked(value);
        }

        return;
    }

    QLineEdit* lineEdit = qobject_cast<QLineEdit*>(object);
    if(lineEdit)
    {
        QString value = mySettings->value(name).toString();
        if(mySettings->contains(name))
        {
            lineEdit->setText(value);
        }

        return;
    }

    QSpinBox* spinBox = qobject_cast<QSpinBox*>(object);
    if(spinBox)
    {
        int value = mySettings->value(name).toInt();
        if(mySettings->contains(name))
        {
            spinBox->setValue(value);
        }

        return;
    }

    foreach(QObject* child, object->children())
    {
        restoreTheChildren(child);
    }
}

bool Persistence::isWindowVisible(QWidget* window)
{
    QDesktopWidget desktop;
    QRect desktopRect = desktop.screenGeometry();

    QRect windowRect = window->geometry();

    if(desktopRect.contains(windowRect))
    {
        return true;
    }
    else
    {
        return false;
    }
}

#endif

bool Persistence::getBoolSetting(const QString &name) const
{
	bool value = mySettings->value(name).toBool();
	if(mySettings->contains(name))
		return value;

	return false;
}

QString Persistence::getStringSetting(const QString &name) const
{
        QString value = mySettings->value(name).toString();
        if(mySettings->contains(name))
		return value;

	return QString("");
}


int Persistence::getIntSetting(const QString &name) const
{
        int value = mySettings->value(name).toInt();
        if(mySettings->contains(name))
		return value;

	return 0;
}

