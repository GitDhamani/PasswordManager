#ifndef HOVERBUTTON_H
#define HOVERBUTTON_H
#include <QPushButton>

class HoverButton: public QPushButton
{
public:
    HoverButton(QWidget* parent = nullptr);

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
};

#endif // HOVERBUTTON_H
