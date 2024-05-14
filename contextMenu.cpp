#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "helpers.h"
#include "edittagdialog.h"
#include "newpassworddialog.h"
#include "dbsetup.h"
#include "newseeddialog.h"
#include "regfunctions.h"
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDir>

extern MainWindow* thisptr;
extern Ui::MainWindow* uiptr;
extern dbType storage;

void writeQStringToFile(const QString &text) {
    // Specify the file path
    QString filePath = QDir::homePath() + "/Desktop/Exported Passwords.txt";

    // Create a QFile object
    QFile file(filePath);

    // Open the file in write-only mode
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        // Create a QTextStream object with the file
        QTextStream stream(&file);

        // Write the QString to the file
        stream << text;

        // Close the file
        file.close();
    } else {
        // Handle error if unable to open the file
        qDebug() << "Error: Unable to open file for writing";
    }
}


void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    //Menu Style Sheet
    QString ss = "QMenu::item {background-color: #f0f0f0; padding: 2px 25px 2px 5px;} \
            QMenu::item:selected {background-color: rgb(0, 85, 127); color: rgb(255, 255, 255);} \
            QMenu::item:hover {background-color: rgb(0, 85, 127); color: rgb(255, 255, 255);} \
            QMenu::separator {background-color: rgb(255, 225, 225); height: 2px; margin: 5px 0px;}";

    //Initialise Variables to Identify Password Item
    QTreeWidgetItem* item;
    QString parentText;
    QString grandParentText;
    QMenu contextMenu("Context Menu", this);

    //Check if Database empty only show new password option
    auto search = storage.get_all<Passwords>();
    if (search.size() == 0)
    {
        // Create the context menu
        contextMenu.addAction("New Password");

        //Add Style to remove padding
        contextMenu.setStyleSheet(ss);
    }
    else
    {
        item = ui->treeWidget->currentItem();
        parentText = item->parent() ? item->parent()->text(0) : "No Parent";
        grandParentText = (item->parent() && item->parent()->parent()) ? item->parent()->parent()->text(0) : "No Grandparent";
        QString str = QString("Clicked: %1 %2 %3").arg(item->text(0)).arg(parentText).arg(grandParentText);

        //If Top Levels are collapsed, remove selections
        bool topLevelCollapsed = true;
        for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i) {
            QTreeWidgetItem *topLevelItem = ui->treeWidget->topLevelItem(i);
            if (topLevelItem && ui->treeWidget->isItemExpanded(topLevelItem)) {
                topLevelCollapsed = false;
                break;
            }
        }

        //If Tree Collapsed, Remove all Selections
        if (topLevelCollapsed) ui->treeWidget->clearSelection();

        //Create the Menu Options
        if (!ui->treeWidget->selectedItems().isEmpty())
            contextMenu.addAction("Edit Node");
        contextMenu.addAction("New Password");
        if (!ui->treeWidget->selectedItems().isEmpty())
            if (!parentText.contains("No Parent") && !grandParentText.contains("No Grandparent"))
                contextMenu.addAction("Delete Password");
        contextMenu.addAction("Find Password");
        contextMenu.addAction("Export All Passwords");
        contextMenu.addAction("Change Seed");
        contextMenu.addAction("Expand All");
        contextMenu.addAction("Collapse All");

        //Add Style to remove padding
        contextMenu.setStyleSheet(ss);
    }

    // Show the Menu
    QAction* selectedAction = contextMenu.exec(ui->treeWidget->mapToGlobal(pos));

    //Handle nothing being clicked
    if (selectedAction == nullptr) return;

    // Handle the Actions
    if (selectedAction->text() == "Edit Node")
    {
        //Variable to Identify the Level of the Password
        int level = 0; //1 = top level, 2 = branch, 3 = tag

        //Default assumption is that we're Right clicking on a tag
        auto selectedPW = storage.get_all<Passwords>(where(c(&Passwords::Category) == grandParentText.toStdString() && c(&Passwords::Branch) == parentText.toStdString() && c(&Passwords::Tag) == item->text(0).toStdString()));
        if (selectedPW.size() > 0) level = 3;

        //Check if we're clicking on a branch
        if (!parentText.contains("No Parent") && grandParentText.contains("No Grandparent"))
        {
            selectedPW = storage.get_all<Passwords>(where(c(&Passwords::Category) == parentText.toStdString() && c(&Passwords::Branch) == item->text(0).toStdString()));
            if (selectedPW.size() > 0) level = 2;
        }

        //Check if we're clicking on a category top level
        if (parentText.contains("No Parent") && grandParentText.contains("No Grandparent"))
        {
            selectedPW = storage.get_all<Passwords>(where(c(&Passwords::Category) == item->text(0).toStdString()));
            if (selectedPW.size() > 0) level = 1;
        }

        //Check if Node is not a tag
        if (parentText.contains("No Parent") || grandParentText.contains("No Grandparent"))
        {
            QString currentName;
            if (parentText.contains("No Parent") && grandParentText.contains("No Grandparent"))
                currentName = QString::fromStdString(selectedPW.at(0).Category);
            if (!parentText.contains("No Parent") && grandParentText.contains("No Grandparent"))
                currentName = QString::fromStdString(selectedPW.at(0).Branch);

            bool okpressed;
            QString pw1 = QInputDialog::getText(NULL, "Enter New Name", "Please Enter a new Name ", QLineEdit::Normal, currentName, &okpressed);
            if (okpressed)
            {
                if (level == 1) storage.update_all(set(c(&Passwords::Category) = pw1.toStdString()), where(c(&Passwords::Category) == selectedPW.at(0).Category));
                if (level == 2) storage.update_all(set(c(&Passwords::Branch) = pw1.toStdString()), where(c(&Passwords::Category) == selectedPW.at(0).Category && c(&Passwords::Branch) == selectedPW.at(0).Branch));
                updateTree();
                clearData();
            }
        }
        else
        {
            //Store values so dialog can access them remotely and load them as initial values.
            Tag = QString::fromStdString(selectedPW.at(0).Tag);
            URL = QString::fromStdString(selectedPW.at(0).URL);
            Username = QString::fromStdString(selectedPW.at(0).Username);
            Password = QString::fromStdString(selectedPW.at(0).PW);
            Notes = QString::fromStdString(selectedPW.at(0).Notes);

            EditTagDialog* dialog = new EditTagDialog();
            dialog->exec();
            if (dialog->success == 1)
            {
                storage.update_all(set(c(&Passwords::Tag) = dialog->tagName.toStdString(), c(&Passwords::URL) = dialog->URL.toStdString(), c(&Passwords::Username) = dialog->Username.toStdString(), c(&Passwords::PW) = dialog->Password.toStdString(), c(&Passwords::Notes) = dialog->Notes.toStdString()), where(c(&Passwords::Category) == selectedPW.at(0).Category && c(&Passwords::Branch) == selectedPW.at(0).Branch && c(&Passwords::Tag) == selectedPW.at(0).Tag));
                updateTree();
                clearData();
            }
            delete dialog;
        }
    }

    if (selectedAction->text() == "New Password")
    {
        NewPasswordDialog* dialog = new NewPasswordDialog();
        dialog->exec();
        if (dialog->success)
        {
            updateTree();
            clearData();
        }
        delete dialog;
    }

    if (selectedAction->text() == "Delete Password")
    {
        //Confirm
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(nullptr, "Confirm", "Delete " + item->text(0) + " ?", QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            //This option only shows up on a tag
            storage.remove_all<Passwords>(where(c(&Passwords::Category) == grandParentText.toStdString() && c(&Passwords::Branch) == parentText.toStdString() && c(&Passwords::Tag) == item->text(0).toStdString()));
            updateTree();
            clearData();
        }
    }

    else if (selectedAction->text() == "Expand All")
    {
        ui->treeWidget->expandAll();
        ui->treeWidget->clearSelection();
        clearData();
    }

    else if (selectedAction->text() == "Collapse All")
    {
        ui->treeWidget->collapseAll();
        ui->treeWidget->clearSelection();
        clearData();
    }

    else if (selectedAction->text() == "Find Password")
    {
        bool okpressed;
        QString findText = QInputDialog::getText(NULL, "Search", "Enter Text to Search For: ", QLineEdit::Normal, "", &okpressed);
        if (findText.size() == 0) return; //no entry

        if (okpressed)
        {
            std::set<QString> searchResults;
            auto search = storage.get_all<Passwords>(where(
                                                         like(&Passwords::Category, "%" + findText.toStdString() + "%") ||
                                                         like(&Passwords::Branch, "%" + findText.toStdString() + "%") ||
                                                         like(&Passwords::Tag, "%" + findText.toStdString() + "%") ||
                                                         like(&Passwords::URL, "%" + findText.toStdString() + "%") ||
                                                         like(&Passwords::Username, "%" + findText.toStdString() + "%") ||
                                                         like(&Passwords::PW, "%" + findText.toStdString() + "%") ||
                                                         like(&Passwords::Notes, "%" + findText.toStdString() + "%")
                                                         ));
            for (auto e: search)
                searchResults.insert(QString::fromStdString(e.Category) + "->" + QString::fromStdString(e.Branch) + "->" + QString::fromStdString(e.Tag));

            if (searchResults.size() == 0)
            {
                QMessageBox::information(0, "Results", "No Results Found");
                return;
            }

            //Create String
            QString str = "Found in Nodes:\n\n";
            for (auto e: searchResults)
                str.append(e + "\n");

            //Print Results
            QMessageBox::information(0, "Results", str);

        }
    }

    else if (selectedAction->text() == "Export All Passwords")
    {
        //Input Dialog to Ask for password
        bool okpressed;
        QString pw = QInputDialog::getText(NULL, "Enter Password", "Please Enter your Password: ", QLineEdit::Password, "", &okpressed);
        if (okpressed)
        {
            //Take in PW, Hash it using SHA1 and compare to Registry PWHash (otherwise error)
            std::string verifySHA1PW = sha1(pw.toStdString());
            std::string getSHA1PW = RegGetKeyValue("HKEY_LOCAL_MACHINE\\SOFTWARE\\PasswordMgr", "PWHash");
//            qDebug() << "VerifySHA1PW: " << verifySHA1PW.c_str();
//            qDebug() << "GetSHA1PW: " << getSHA1PW.c_str();
            if (verifySHA1PW != getSHA1PW) return;

            //Gather All Passwords and put into TXT File
            auto ALLPW = storage.get_all<Passwords>();
            QString OutputStr;
            for (auto e: ALLPW)
            {
                QString str = QString("Category: %1\nBranch: %2\nTag: %3\nURL: %4\nUsername: %5\nPassword: %6\nNotes: %7\n\n").arg(QString::fromStdString(e.Category)).arg(QString::fromStdString(e.Branch)).arg(QString::fromStdString(e.Tag)).arg(QString::fromStdString(e.URL)).arg(QString::fromStdString(e.Username)).arg(QString::fromStdString(e.PW)).arg(QString::fromStdString(e.Notes));
                OutputStr.append(str);
            }

            //Write to Desktop file Exported Passwords
            writeQStringToFile(OutputStr);

            QMessageBox::information(0, "Success", "Your Passwords have been exported to\nDesktop\\Exported Passwords.txt");
        }

    }

    else if (selectedAction->text() == "Change Seed")
    {
        //Input Dialog to Ask for new B32 Seed
        newSeedDialog* dialog = new newSeedDialog();
        dialog->exec();
        delete dialog;
    }

    else
    {
        // No action was selected
    }
}
