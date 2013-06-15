#ifndef WIDGETFLASH_H
#define WIDGETFLASH_H

#include <QObject>
#include <QtCore>
#include <QWidget>


class WidgetFlash 
{

public:

    WidgetFlash();
    ~WidgetFlash();


    void red(QWidget* widget);
    void green(QWidget* widget);
    void yellow(QWidget* widget);

    void flash(QWidget* widget, QColor startColor, QColor endColor);

};

#endif

