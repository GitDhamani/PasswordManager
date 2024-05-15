#ifndef HELPERS_H
#define HELPERS_H

//Headers
#include <Windows.h>
#include <Lmcons.h>
#include <QDebug>
#include <QScrollBar>
#include <QTimer>
#include <QMenu>
#include <QInputDialog>
#include <QMessageBox>
#include "dbsetup.h"
#include <QClipboard>
#include "edittagdialog.h"
#include "newpassworddialog.h"
#include <QRandomGenerator>
#include <QTreeWidget>
#include "QR/QrToPng.h"
#include <QFile>
#include <QString>
#include <QChar>
#include <QDateTime>
#include <string>


//Prototypes
void initialiseGUI();
void initialiseTree();
void loginPageConnect();
void mainPageConnect();
void treeWidgetConnect();
void SetupTable(std::string pw);
void updateTree();
bool isNewInstall();
void GenRandomQRSeed();
std::string generate_TOTP(const std::string& secret, std::string algorithm, uint8_t digits, uint8_t time_window, int interval);
std::string encryptString(const std::string &plaintext, const std::string &key);
std::string decryptString(const std::string &ciphertext, const std::string &key);
std::string sha1(const std::string& input);
std::string GenRandomPW();
#endif // HELPERS_H
