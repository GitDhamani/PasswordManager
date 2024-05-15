#ifndef CHILDLINEEDIT_H
#define CHILDLINEEDIT_H

#include <QLineEdit>

class ChildLineEdit : public QLineEdit
{
public:
    ChildLineEdit(QWidget* parent = nullptr);

    // QWidget interface
protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // CHILDLINEEDIT_H
