#include "WidgetFlash.h"

#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>


WidgetFlash::WidgetFlash() 
{
}
WidgetFlash::~WidgetFlash()
{
    // cleanup is performed by the widget
}

void WidgetFlash::flash(QWidget* widget, QColor startColor, QColor endColor)
{
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(15);
    shadow->setOffset(0);

    QPropertyAnimation *animation = new QPropertyAnimation(shadow, "color", shadow);
    animation->setDuration(2000);
    animation->setStartValue(startColor);
    animation->setEndValue(endColor);
    animation->start();

    widget->setGraphicsEffect(shadow); 
}


void WidgetFlash::red(QWidget* widget)
{
    flash(widget, QColor(255, 0, 0, 255), QColor(255, 0, 0, 0));
}
void WidgetFlash::green(QWidget* widget)
{
    flash(widget, QColor(0, 255, 0, 255), QColor(0, 255, 0, 0));
}
void WidgetFlash::yellow(QWidget* widget)
{
    flash(widget, QColor(255, 255, 0, 255), QColor(255, 255, 0, 0));
}
