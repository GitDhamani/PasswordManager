#ifndef NEWSEEDDIALOG_H
#define NEWSEEDDIALOG_H

#include <QDialog>

namespace Ui {
class newSeedDialog;
}

class newSeedDialog : public QDialog
{
    Q_OBJECT

public:
    explicit newSeedDialog(QWidget *parent = nullptr);
    ~newSeedDialog();

private:
    Ui::newSeedDialog *ui;
};

#endif // NEWSEEDDIALOG_H
