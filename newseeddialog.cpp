#include "newseeddialog.h"
#include "ui_newseeddialog.h"
#include "regfunctions.h"
#include "helpers.h"
#include <QTimer>

extern bool checkOTP(int N, std::string B32Seed, QString otpCode);

newSeedDialog::newSeedDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newSeedDialog)
{
    ui->setupUi(this);
    ui->errorLabel->setVisible(false);

    connect(ui->pushButton, &QPushButton::clicked, [this](){

        //Check Password is correct
        //Take in PW, Hash it using SHA1 and compare to Registry PWHash (otherwise error)
        std::string verifySHA1PW = sha1(ui->pwEdit->text().toStdString());
        std::string getSHA1PW = RegGetKeyValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "PWHash");
        if (verifySHA1PW != getSHA1PW)
        {
            ui->errorLabel->setVisible(true);
            QTimer::singleShot(1000, NULL, [this](){ ui->errorLabel->setVisible(false);});
            return;
        }

        //Check OTP is correct
        bool ret = checkOTP(2, ui->seedEdit->text().toStdString(), ui->otpEdit->text());
        if (ret == 0)
        {
            ui->errorLabel->setVisible(true);
            QTimer::singleShot(1000, NULL, [this](){ ui->errorLabel->setVisible(false);});
            return;
        }

        //Encrypt Seed with AES256 and install into Registry Key Seed
        //Use the MachineGuid as the Encryption Key
        std::string keyval = RegGetKeyValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography", "MachineGuid");
        std::string encryptedSeed = encryptString(ui->seedEdit->text().toStdString(), keyval);
        RegAddKey("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "Seed", encryptedSeed);

        this->close();

    });
}

newSeedDialog::~newSeedDialog()
{
    delete ui;
}
