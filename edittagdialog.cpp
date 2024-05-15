#include "edittagdialog.h"
#include "ui_edittagdialog.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "dbsetup.h"
#include <QClipboard>

extern MainWindow* thisptr;
extern Ui::MainWindow* uiptr;
extern dbType storage;
extern QString generateRandomPassword();

EditTagDialog::EditTagDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditTagDialog)
{
    ui->setupUi(this);
    setFixedSize(372,349);
    success = 0;

    //Load initial values from the MainWindow class
    ui->tagEdit->setText(thisptr->Tag);
    ui->urlEdit->setText(thisptr->URL);
    ui->usernameEdit->setText(thisptr->Username);
    ui->passwordEdit->setText(thisptr->Password);
    ui->notesEdit->setText(thisptr->Notes);

    connect(ui->pushButton, &QPushButton::clicked, [this](){
        tagName = ui->tagEdit->text();
        Username = ui->usernameEdit->text();
        URL = ui->urlEdit->text();
        Password = ui->passwordEdit->text();
        Notes = ui->notesEdit->toPlainText();
        success = 1;
        this->close();
    });

    //Generate Random Password "G" Button
    connect(ui->genButton, &QPushButton::clicked, [this](){
        QString randomPW = generateRandomPassword();
        ui->passwordEdit->setText(randomPW);

        // Save password to clipboard
        QApplication::clipboard()->setText(randomPW);
    });
}

EditTagDialog::~EditTagDialog()
{
    delete ui;
}
