#include "childbutton.h"

childButton::childButton(QWidget* parent): QPushButton(parent)
{

}

void childButton::mousePressEvent(QMouseEvent *event)
{
   //setStyleSheet("{ background-color: #55ffff; color: rgb(255, 255, 255); border: 2px solid white; border-radius: 10px; }");
    QPushButton::mousePressEvent(event);
}

void childButton::mouseReleaseEvent(QMouseEvent *event)
{
    //this->setStyleSheet("{ background-color: black; }");
    QPushButton::mousePressEvent(event);
}
