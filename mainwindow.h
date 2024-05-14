#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void clearData();
    QClipboard *clipboard;
    QString Tag, URL, Username, Password, Notes;
    bool newInstallation = 0;
    std::string B32QRSeed;

private slots:
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);


private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
