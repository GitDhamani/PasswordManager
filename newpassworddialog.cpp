#include "newpassworddialog.h"
#include "ui_newpassworddialog.h"
#include "dbsetup.h"
#include <QMessageBox>
#include <set>

extern dbType storage;

NewPasswordDialog::NewPasswordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewPasswordDialog)
{
    ui->setupUi(this);
    setFixedSize(361,421);
    success = 0;

    //Preload the rootBox with all the categories
    //preload the branchBox with all the branches

    auto getCats = storage.get_all<Passwords>();

    //Load Cateogy and Branch Names into Sets
    std::set<std::string> catNames;
    std::set<std::string> branchNames;
    for (auto e: getCats)
    {
        catNames.insert(e.Category);
        branchNames.insert(e.Branch);
    }

    //Load Category Combo Box
    for (auto e: catNames)
    ui->rootBox->addItem(QString::fromStdString(e));

    //Load Branch Combo Box
    for (auto e: branchNames)
    ui->branchBox->addItem(QString::fromStdString(e));

    connect(ui->pushButton, &QPushButton::clicked, [this](){

        //Some type of input validation, not allowing root, branch or tag names.
        if (ui->tagEdit->text().isEmpty() || ui->branchBox->currentText().isEmpty() || ui->rootBox->currentText().isEmpty())
        {
            QMessageBox::information(this, "Error", "Please provide names for the Root, Branch and Tag Names.");
            return;
        }


        //Make sure this tag doesn't already exist
        auto selectedPW = storage.get_all<Passwords>(where(c(&Passwords::Category) == ui->rootBox->currentText().toStdString() && c(&Passwords::Branch) == ui->branchBox->currentText().toStdString() && c(&Passwords::Tag) == ui->tagEdit->text().toStdString()));
        if (selectedPW.size() > 0)
        {
            QMessageBox::information(this, "Error", "This Tag Already Exists. \nPlease Choose a Unique Name.");
            return;
        }

        //Create new record
        storage.insert(Passwords{ui->rootBox->currentText().toStdString(), ui->branchBox->currentText().toStdString(), ui->tagEdit->text().toStdString(), ui->urlEdit->text().toStdString(), ui->usernameEdit->text().toStdString(), ui->passwordEdit->text().toStdString(), ui->notesEdit->toPlainText().toStdString()});

        success = 1;
        this->close();
    });

}

NewPasswordDialog::~NewPasswordDialog()
{
    delete ui;
}
