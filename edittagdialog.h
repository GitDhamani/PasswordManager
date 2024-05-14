#ifndef EDITTAGDIALOG_H
#define EDITTAGDIALOG_H

#include <QDialog>

namespace Ui {
class EditTagDialog;
}

class EditTagDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditTagDialog(QWidget *parent = nullptr);
    ~EditTagDialog();
    bool success = 0;
    QString tagName;
    QString URL;
    QString Username;
    QString Password;
    QString Notes;

private:
    Ui::EditTagDialog *ui;
};

#endif // EDITTAGDIALOG_H
