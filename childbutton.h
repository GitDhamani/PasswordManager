#ifndef CHILDBUTTON_H
#define CHILDBUTTON_H

#include <QPushButton>

class childButton : public QPushButton
{
public:
    childButton(QWidget* parent = nullptr);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
};

#endif // CHILDBUTTON_H
