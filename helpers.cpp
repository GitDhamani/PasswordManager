#include "helpers.h"
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "regfunctions.h"
#include "dbsetup.h"
#include "sqlite3mc.h" //needed to set the Encryption key.
#include "sqlite3.h"

using namespace sqlite_orm;
extern MainWindow* thisptr;
extern Ui::MainWindow* uiptr;
extern dbType storage;

void initialiseGUI()
{
    uiptr->errorLabel->setVisible(false);
}

void initialiseTree()
{
    QTreeWidget* tree = uiptr->treeWidget;
    uiptr->stackedWidget->setCurrentIndex(0);
    // tree->setColumnCount(1);
    tree->setFont(QFont("Arial", 12));
    tree->setSortingEnabled(true);
    tree->sortItems(0, Qt::AscendingOrder);

    tree->verticalScrollBar()->setStyleSheet(
                "background-color: rgb(0,0,0);"
                "color: rgb(255, 255, 255);"
                "border: 2px solid white;"
                "border-radius: 5px;"
                );
}

void updateTree()
{
    auto records = storage.get_all<Passwords>();
    uiptr->treeWidget->clear();
    for (auto element: records)
    {
            // Check if Category exists
            bool hasThisAlready = false;
            for (int i = 0; i < uiptr->treeWidget->topLevelItemCount(); ++i) {
                QTreeWidgetItem *item = uiptr->treeWidget->topLevelItem(i);
                if (item && item->text(0) == QString::fromStdString(element.Category)) {
                    hasThisAlready = true;
                    break;
                }
            }

            //If the category doesn't exist create it
            if (hasThisAlready == false)
            {
                QTreeWidgetItem* TopLevel = new QTreeWidgetItem(uiptr->treeWidget);
                TopLevel->setText(0, QString::fromStdString(element.Category));
            }

            // Check if Branch exists within this cateogy
            hasThisAlready = false;
            int indexOfParent = 0;
            for (int i = 0; i < uiptr->treeWidget->topLevelItemCount(); ++i) {
                QTreeWidgetItem *topLevelItem = uiptr->treeWidget->topLevelItem(i);
                if (topLevelItem && topLevelItem->text(0) == QString::fromStdString(element.Category)) {
                    indexOfParent = i;
                    for (int j = 0; j < topLevelItem->childCount(); ++j) {
                        QTreeWidgetItem *childItem = topLevelItem->child(j);
                        if (childItem && childItem->text(0) == QString::fromStdString(element.Branch)) {
                            hasThisAlready = true;
                            break;
                        }
                    }
                    break;
                }
            }


            //If the Branch doesn't exist create it
            if (hasThisAlready == false)
            {
                QTreeWidgetItem* BranchLevel = new QTreeWidgetItem(uiptr->treeWidget->topLevelItem(indexOfParent));
                BranchLevel->setText(0, QString::fromStdString(element.Branch));
            }

            // Check if Tag exists within this Branch
            hasThisAlready = false;
            QTreeWidgetItem *childItem;
            for (int i = 0; i < uiptr->treeWidget->topLevelItemCount(); ++i) {
                QTreeWidgetItem *topLevelItem = uiptr->treeWidget->topLevelItem(i);
                if (topLevelItem && topLevelItem->text(0) == QString::fromStdString(element.Category)) {
                    for (int j = 0; j < topLevelItem->childCount(); ++j) {
                        childItem = topLevelItem->child(j);
                        if (childItem && childItem->text(0) == QString::fromStdString(element.Branch)) {
                            for (int k = 0; k < childItem->childCount(); ++k) {
                                QTreeWidgetItem *tagItem = childItem->child(j);
                                if (tagItem && tagItem->text(0) == QString::fromStdString(element.Tag)) {
                                    hasThisAlready = true;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                }
            }

            //If the Tag doesn't exist create it
            if (hasThisAlready == false)
            {
                QTreeWidgetItem* TagLevel = new QTreeWidgetItem(childItem);
                TagLevel->setText(0, QString::fromStdString(element.Tag));
            }

    }


    //Set the initial selected item as the first one
    QTreeWidgetItem *topItem = uiptr->treeWidget->topLevelItem(0);
    if (topItem) uiptr->treeWidget->setCurrentItem(topItem);
}

void failedLogin()
{
    uiptr->errorLabel->setText("Incorrect Login. Try Again.");
    uiptr->errorLabel->setVisible(true);
    uiptr->pwEdit->clear();
    uiptr->otpEdit->clear();
    QTimer::singleShot(1000, NULL, [](){ uiptr->errorLabel->setVisible(false); });
}

bool checkOTP(int N, std::string B32Seed, QString otpCode)
{
    bool success = 0;
    //Tolerance N Windows Previous and N windows Ahead
    for (int i = -N; i <= N; i++)
            {
                std::string code = generate_TOTP(B32Seed, "SHA1", 6, 30, i);
                if (otpCode == QString::fromStdString(code))
                {
                    qDebug() << "OTP Correct!";
                    success = 1;
                    break;
                }
             }
    return success;
}

std::string getKeyVal()
{
    QFile file("Guid.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return RegGetKeyValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography", "MachineGuid");

    // Create a QTextStream to read from the file
    QTextStream in(&file);

    // Read a line from the file into a QString
    QString line = in.readLine();

    // Close the file
    file.close();
    return line.toStdString();
}

std::string verifyLogin()
{
    //Take in PW, Hash it using SHA1 and compare to Registry PWHash (otherwise error)
    std::string verifySHA1PW = sha1(uiptr->pwEdit->text().toStdString());
    std::string getSHA1PW = RegGetKeyValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "PWHash");
    if (verifySHA1PW != getSHA1PW) { failedLogin(); return "failed"; }

    //Retrieve Seed from Registry and decrypt AES256
    std::string getSeed = RegGetKeyValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "Seed");
    std::string verifykeyval = getKeyVal();
    std::string decryptedSeed = decryptString(getSeed, verifykeyval);
    thisptr->B32QRSeed = decryptedSeed;

    //Generate an OTP Code and verify against OTPCode entered (otherwise error)
    if(checkOTP(2, thisptr->B32QRSeed, uiptr->otpEdit->text()) == false)
    { failedLogin(); return "failed"; }

    //Decrypt DBPW using AES256 and remove pwHash from the end
    std::string getDBPW = RegGetKeyValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "DBPW");
    std::string decryptDBPW = decryptString(getDBPW, verifykeyval).c_str();

    //Remove 40 characters from end, which is the SHA1PW
    std::string recoveredSHA1PW = QString::fromStdString(decryptDBPW).right(40).toStdString();
    std::string recoveredDBPW = QString::fromStdString(decryptDBPW).left(decryptDBPW.length()-40).toStdString();
    std::string getPWHash = RegGetKeyValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "PWHash");
    if (getPWHash != recoveredSHA1PW) { failedLogin(); return "failed"; }

    return recoveredDBPW;
}

bool CheckWinPass(const std::string& Username, const std::string& PW) {
    std::wstring wUsername(Username.begin(), Username.end());
    std::wstring wPW(PW.begin(), PW.end());

    HANDLE token;
    if (LogonUserW(wUsername.c_str(), L".", wPW.c_str(), LOGON32_LOGON_INTERACTIVE, LOGON32_PROVIDER_DEFAULT, &token)) {
        CloseHandle(token);
        return true; // Password is correct.
    } else {
        return false; // Password is incorrect or could not log in.
    }
}

void loginPageConnect()
{
    //The login button
    thisptr->connect(uiptr->loginButton, &QPushButton::clicked, [](){

        //Are Fields Empty? If so return
        if (uiptr->pwEdit->text().isEmpty() || uiptr->otpEdit->text().isEmpty())
        { failedLogin(); return; }

        //Check if a brand new installation or not
        bool newInstallCheck = isNewInstall();

        if (newInstallCheck == 1) //New Install
        {
            //Is the OTP Code Correct, 2 Window Tolerance;
            if(checkOTP(2, thisptr->B32QRSeed, uiptr->otpEdit->text()) == false)
            { failedLogin(); return; }

            //Encrypt Seed with AES256 and install into Registry Key Seed
            //Use the MachineGuid as the Encryption Key
            std::string keyval = getKeyVal();
            std::string encryptedSeed = encryptString(thisptr->B32QRSeed, keyval);
            RegAddKey("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "Seed", encryptedSeed);

            //Take in PW, hash using SHA1 and install into Registry Key PWHash
            std::string SHA1PW = sha1(uiptr->pwEdit->text().toStdString());
            RegAddKey("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "PWHash", SHA1PW);

            //Delete Database
            QFile DBFile {"Passwords.db"};
            DBFile.remove();

            //Generate Random Database Password
            std::string randomPW = GenRandomPW();
            //theDBPW = randomPW;

            //Create DBPW = [RandomPW + PWHash], and Encrypt with AES256 using MachineGuid as key.
            randomPW += SHA1PW;
            std::string DBPW = encryptString(randomPW, keyval);
            RegAddKey("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "DBPW", DBPW);
        }

        std::string theDBPW = verifyLogin();
        if (theDBPW == "failed") return;

        //Using Database PW to encrypt database, if it fails, Program will quit.
        SetupTable(theDBPW);

        //Prepare Tree Format
        initialiseTree();

        //Load Tree with Database
        updateTree();

        uiptr->stackedWidget->setCurrentIndex(1);
    });

    //The Reset Button
    thisptr->connect(uiptr->resetButton, &QPushButton::clicked, [](){

        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(nullptr, "Confirm", "A Reset will wipe the Database and\nrequires an application restart.\nAre you sure?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::No) return;

        //Make sure User knows Windows Logon PW

        //Get Current User Username
        char username[UNLEN + 1];
        DWORD username_len = UNLEN + 1;
        if (!GetUserNameA(username, &username_len))
        {
            qDebug() << "Failed to retrieve username. Error code: " << GetLastError();
            exit(1);
        }

        //Request Windows Password from user
        bool okpressed;
        QString pw = QInputDialog::getText(NULL, "Enter Password", "Please Enter your Windows Password: ", QLineEdit::Password, "", &okpressed);
        if (!okpressed) return;

        //Check Credentials
        if (!CheckWinPass(username, pw.toStdString())) return; //Password Failed

        //Wipe Registry
        RegDeleteValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "PWHash");
        RegDeleteValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "Seed");
        RegDeleteValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "DBPW");

        //Clear Clipboard and Data
        thisptr->clearData();

        //Quit
        QApplication::exit(0);
    });
}

void mainPageConnect()
{
        //URL Button
        thisptr->connect(uiptr->urlButton, &QPushButton::clicked, [](){
            if (uiptr->treeWidget->selectedItems().isEmpty()) return;

            QTreeWidgetItem* item = uiptr->treeWidget->currentItem();
            QString parentText = item->parent() ? item->parent()->text(0) : "No Parent";
            QString grandParentText = (item->parent() && item->parent()->parent()) ? item->parent()->parent()->text(0) : "No Grandparent";
            QString str = QString("Clicked: %1 %2 %3").arg(item->text(0)).arg(parentText).arg(grandParentText);
            if (parentText.contains("No Parent") || grandParentText.contains("No Grandparent")) return;

            uiptr->urlButton->setText("Copied");
            thisptr->clipboard = QGuiApplication::clipboard();
            thisptr->clipboard->setText(thisptr->URL);

            QTimer::singleShot(500, NULL, [](){
                uiptr->urlButton->setText("URL");
            });

        });

        //Username Button
        thisptr->connect(uiptr->usernameButton, &QPushButton::clicked, [](){
            if (uiptr->treeWidget->selectedItems().isEmpty()) return;

            QTreeWidgetItem* item = uiptr->treeWidget->currentItem();
            QString parentText = item->parent() ? item->parent()->text(0) : "No Parent";
            QString grandParentText = (item->parent() && item->parent()->parent()) ? item->parent()->parent()->text(0) : "No Grandparent";
            QString str = QString("Clicked: %1 %2 %3").arg(item->text(0)).arg(parentText).arg(grandParentText);
            if (parentText.contains("No Parent") || grandParentText.contains("No Grandparent")) return;

            uiptr->usernameButton->setText("Copied");
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setText(thisptr->Username);

            QTimer::singleShot(500, NULL, [](){
                uiptr->usernameButton->setText("Username");
            });

        });

        //Password Button
        thisptr->connect(uiptr->pwButton, &QPushButton::clicked, [](){
            if (uiptr->treeWidget->selectedItems().isEmpty()) return;

            QTreeWidgetItem* item = uiptr->treeWidget->currentItem();
            QString parentText = item->parent() ? item->parent()->text(0) : "No Parent";
            QString grandParentText = (item->parent() && item->parent()->parent()) ? item->parent()->parent()->text(0) : "No Grandparent";
            QString str = QString("Clicked: %1 %2 %3").arg(item->text(0)).arg(parentText).arg(grandParentText);
            if (parentText.contains("No Parent") || grandParentText.contains("No Grandparent")) return;

            uiptr->pwButton->setText("Copied");
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setText(thisptr->Password);

            QTimer::singleShot(500, NULL, [](){
                uiptr->pwButton->setText("Password");
            });

        });

}

void treeWidgetConnect()
{
    //When Tag is clicked, fill the Notes Box and load Database Values into MainWindow Class Variables.
    thisptr->connect(uiptr->treeWidget, &QTreeWidget::itemClicked, [=](QTreeWidgetItem* item, int val){
        QString parentText = item->parent() ? item->parent()->text(0) : "No Parent";
        QString grandParentText = (item->parent() && item->parent()->parent()) ? item->parent()->parent()->text(0) : "No Grandparent";
        QString str = QString("Clicked: %1 %2 %3").arg(item->text(0)).arg(parentText).arg(grandParentText);

        auto selectedPW = storage.get_all<Passwords>(where(c(&Passwords::Category) == grandParentText.toStdString() && c(&Passwords::Branch) == parentText.toStdString() && c(&Passwords::Tag) == item->text(0).toStdString()));
        if (selectedPW.size() == 0) return;

        uiptr->textEdit->setText(QString::fromStdString(selectedPW.at(0).Notes));
        thisptr->Password = selectedPW.at(0).PW.c_str();
        thisptr->URL = selectedPW.at(0).URL.c_str();
        thisptr->Username = selectedPW.at(0).Username.c_str();

    });
}

void MainWindow::clearData()
{
    ui->textEdit->clear();
    URL = "";
    Username = "";
    Password = "";
    clipboard->setText("");
}

bool isNewInstall()
{
    //Check if Key exists
    QString regPWString = QString::fromStdString(RegGetKeyValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\PassWordMgr", "pwHash"));
    if (regPWString == "Key Not Found")
        return 1;
    else
        return 0;
}

std::string GenRandomPW()
{
    //Generate Random 40 Digit Base 32 Seed
    std::string B32 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    std::string B32String = "";
    for (int i = 0; i<40; i++)
    B32String.push_back(B32[QRandomGenerator::global()->bounded(32)]);
    return B32String;
}

void GenRandomQRSeed()
{
    //Remove any Trace of original database
    QFile fileDB{"Passwords.db"};
    fileDB.remove();

    //Give instructions to the user
    uiptr->errorLabel->setVisible(true);
    uiptr->errorLabel->setText("Scan the QR Code above into your Authenticator App");

    //Generate Random 40 Digit Base 32 Seed
    std::string B32 = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    std::string B32String = "";
    for (int i = 0; i<40; i++)
    B32String.push_back(B32[QRandomGenerator::global()->bounded(32)]);

    //Store Hash for Later
    thisptr->B32QRSeed = B32String;

    std::string fullcode = "otpauth://totp/Password Manager:OTP Code (GitDhamani)?secret=" + B32String + "&algorithm=SHA1&digits=6&period=30";

    std::string filename = "Seed.png";
    int imgSize = 300;
    int minModulePixelSize = 3;
    auto exampleQrPng1 = QrToPng(filename, imgSize, minModulePixelSize, fullcode, true, qrcodegen::QrCode::Ecc::MEDIUM);

    //Keep these Debug Statements in, they cause a small delay that
    //is needed for the Generation of Seed.png
    if (exampleQrPng1.writeToPNG())
        qDebug() << "QRCode Generation Success!";
    else
        qDebug() << "QRCode Generation Failure...";

    //Load QR Code into Login Page Label
    QPixmap pixmap("Seed.png");
    uiptr->loginLabel->setPixmap(pixmap);

    //Delete the Seed Image For Security
    QFile file("Seed.png");
    file.remove();
}
