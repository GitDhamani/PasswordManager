#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "helpers.h"

MainWindow* thisptr;
Ui::MainWindow* uiptr;
extern dbType storage;

MainWindow::MainWindow(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Setting up external Pointers
    thisptr = this;
    uiptr = ui;

    //Initialise GUI
    initialiseGUI();

    //Check if Brand New Installation, return 1 if yes, return 0 if no.
    newInstallation = isNewInstall();

    //If new installation, load QR Seed on main login page
    if (newInstallation) GenRandomQRSeed();

    //Login Page Connect Statements
    loginPageConnect();

    //Main Page Connect Statements
    mainPageConnect();

    //Tree Widget Connect Statements
    treeWidgetConnect();
}

MainWindow::~MainWindow()
{
    delete ui;
}



