#include "childlineedit.h"
#include <QKeyEvent>
extern void doLogin();

ChildLineEdit::ChildLineEdit(QWidget* parent): QLineEdit(parent)
{
    this->setEchoMode(QLineEdit::Password);
}

void ChildLineEdit::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return) doLogin();
    QLineEdit::keyPressEvent(event);
}
