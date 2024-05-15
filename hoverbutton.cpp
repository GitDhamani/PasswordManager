#include "hoverbutton.h"

HoverButton::HoverButton(QWidget* parent):QPushButton(parent)
{
   setStyleSheet(
           "background-color: rgb(0, 0, 0);"
           "color: rgb(255, 255, 255);"
           "border: 2px solid white;"
           "border-radius: 10px;"
               );
}

void HoverButton::mousePressEvent(QMouseEvent *event)
{
    setStyleSheet(
            "background-color: #40c2c2;"
            "color: rgb(0, 0, 0);"
            "border: 2px solid red;"
            "border-radius: 10px;"
                );
    QPushButton::mousePressEvent(event);
}

void HoverButton::mouseReleaseEvent(QMouseEvent *event)
{
    setStyleSheet(
            "background-color: #40c2c2;"
            "color: rgb(0, 0, 0);"
            "border: 2px solid white;"
            "border-radius: 10px;"
                );
    QPushButton::mouseReleaseEvent(event);
}

void HoverButton::enterEvent(QEvent *event)
{
    setStyleSheet(
            "background-color: #40c2c2;"
            "color: rgb(0, 0, 0);"
            "border: 2px solid white;"
            "border-radius: 10px;"
                );
    QPushButton::enterEvent(event);
}

void HoverButton::leaveEvent(QEvent *event)
{
    setStyleSheet(
            "background-color: rgb(0, 0, 0);"
            "color: rgb(255, 255, 255);"
            "border: 2px solid white;"
            "border-radius: 10px;"
                );
    QPushButton::leaveEvent(event);
}
