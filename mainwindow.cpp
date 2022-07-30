#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MIPSReader.h"

QString genRegList[] = {
    "$zero", "$at", "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3",
    "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3",
    "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9", "$k0", "$k1",
    "$gp", "$sp", "$fp", "$ra"
};

QString floatRegList[] = {
    "$f0", "$f1", "$f2", "$f3",
    "$f4", "$f5", "$f6", "$f7",
    "$f8", "$f9", "$f10", "$f11",
    "$f12", "$f13", "$f14", "$f15",
    "$f16", "$f17", "$f18", "$f19",
    "$f20", "$f21", "$f22", "$f23",
    "$f24", "$f25", "$f26", "$f27",
    "$f28", "$f29", "$f30", "$f31"
};

SettingsWindow::~SettingsWindow()
{
    delete sendUpdate;
}

ProgWindow::ProgWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    setW = new SettingsWindow(this);
    loadSettings();
    int longScroll = setW->settingsValues[4].toInt(nullptr)*4;
    int shortScroll = setW->settingsValues[5].toInt(nullptr)*4;
    mainReader = new MIPSReader(this);

    int buttonHeight = 35;
    int labelHeight = 15;

    modList = new ModdedData;
    branchFinder = new BranchFinder;

    ui->setupUi(this);

    //addressOffset = 542360; //hornet lineup
    //addressOffset = 0;


    menuMain = new QMenuBar(this);
    menuMain->setGeometry(QRect(QPoint(int(hSize*0),int(vSize*0)), QSize(int(hSize*1),int(vSize*0.03))));
    QMenu *menuFile = menuMain->addMenu("File");
    QMenu *menuMods = menuMain->addMenu("Mods");
    QMenu *menuSettings = menuMain -> addMenu("Settings");
    QAction *actionLoadELF = menuFile->addAction("Load ELF");
    QAction *actionSaveELF = menuFile ->addAction("Save ELF");
    QAction *actionSaveISO = menuFile ->addAction("Save ISO");
    QAction *actionLoadMod = menuMods->addAction("Load Mods");
    QAction *actionSaveMod = menuMods ->addAction("Save Mods");
    QAction *actionSettings = menuSettings -> addAction("Settings");

    MipsList = new QListWidget(this);
    MipsList->setUniformItemSizes(true);
    MipsList -> setGeometry(QRect(QPoint(int(hSize*.02),int(vSize*0.055)), QSize(int(hSize*0.38),int(vSize*0.92))));

    //MipsTable = new QTableWidget(this);
    //MipsTable->setGeometry(QRect(QPoint(int(hSize*.43),int(vSize*0.055)), QSize(int(hSize*0.38),int(vSize*0.92))));

    InstructionBox = new QLineEdit(this);
    InstructionBox -> setGeometry(QRect(QPoint(int(hSize*.40),int(vSize*0.065) + labelHeight), QSize(int(hSize*0.35),buttonHeight)));
    ButtonInsert = new QPushButton("Insert", this);
    ButtonInsert -> setGeometry(QRect(QPoint(int(hSize*.40),int(vSize*0.075) + buttonHeight + labelHeight), QSize(int(hSize*.11),buttonHeight)));
    ButtonDelete = new QPushButton("Delete", this);
    ButtonDelete -> setGeometry(QRect(QPoint(int(hSize*.52),int(vSize*0.075) + buttonHeight + labelHeight), QSize(int(hSize*.11),buttonHeight)));
    ButtonReplace = new QPushButton("Replace", this);
    ButtonReplace -> setGeometry(QRect(QPoint(int(hSize*.64),int(vSize*0.075) + buttonHeight + labelHeight), QSize(int(hSize*.11),buttonHeight)));
    LabelInstruction = new QLabel("New Instruction:", this);
    LabelInstruction->setGeometry(QRect(QPoint(int(hSize*.41),int(vSize*0.055)), QSize(int(hSize*0.11),labelHeight)));

    radioHex = new QRadioButton("Hex view", this);
    radioHex -> setGeometry(QRect(QPoint(int(hSize*0.45),int(vSize*0.075) + (buttonHeight*2) + labelHeight), QSize(int(hSize*0.12),buttonHeight)));
    radioInst = new QRadioButton("Instruction view", this);
    radioInst -> setGeometry(QRect(QPoint(int(hSize*0.58),int(vSize*0.075) + (buttonHeight*2) + labelHeight), QSize(int(hSize*0.17),buttonHeight)));
    radioInst-> toggle();

    LabelAddress = new QLabel("Jump to address:", this);
    LabelAddress->setGeometry(QRect(QPoint(int(hSize*.45),250-labelHeight), QSize(int(hSize*0.11),labelHeight)));
    Button10Up = new QPushButton("^^", this);
    Button10Up -> setGeometry(QRect(QPoint(int(hSize*.40),190), QSize(buttonHeight,buttonHeight)));
    Button10Down = new QPushButton("vv", this);
    Button10Down -> setGeometry(QRect(QPoint(int(hSize*.40),310), QSize(buttonHeight,buttonHeight)));
    Button1Up = new QPushButton("^", this);
    Button1Up -> setGeometry(QRect(QPoint(int(hSize*.40),220), QSize(buttonHeight,buttonHeight)));
    Button1Down = new QPushButton("v", this);
    Button1Down -> setGeometry(QRect(QPoint(int(hSize*.40),280), QSize(buttonHeight,buttonHeight)));
    AddressBox = new QLineEdit(this);
    AddressBox -> setGeometry(QRect(QPoint(int(hSize*.40),250), QSize(int(hSize*.11),buttonHeight)));
    ButtonAddress = new QPushButton("Jump", this);
    ButtonAddress -> setGeometry(QRect(QPoint(int(hSize*.52),250), QSize(int(hSize*.11),buttonHeight)));

    MessagePopup = new QMessageBox(this);

    connect(ButtonDelete, &QPushButton::released, this, [this] {mainReader->handleDelete();});
    connect(ButtonInsert, &QPushButton::released, this, [this] {mainReader->handleInsert();});
    connect(ButtonReplace, &QPushButton::released, this, [this] {mainReader->handleReplace();});
    connect(Button10Up, &QPushButton::released, this, [this, longScroll] {mainReader->scrollMIPS(-longScroll);});
    connect(Button10Down, &QPushButton::released, this, [this, longScroll] {mainReader->scrollMIPS(longScroll);});
    connect(Button1Up, &QPushButton::released, this, [this, shortScroll] {mainReader->scrollMIPS(-shortScroll);});
    connect(Button1Down, &QPushButton::released, this, [this, shortScroll] {mainReader->scrollMIPS(shortScroll);});

    connect(ButtonAddress, &QPushButton::released, this, [this] {mainReader->jumpAddress();});
    connect(actionLoadELF, &QAction::triggered, this, [this] {mainReader->loadFile();});
    connect(actionSaveELF, &QAction::triggered, this, [this] {mainReader->saveFile();});
    connect(actionSaveISO, &QAction::triggered, this, [this] {mainReader->isoSearcher();});

    connect(actionSaveMod, &QAction::triggered, this, [this] {modList->makeModList(*this);});
    connect(actionLoadMod, &QAction::triggered, this, [this] {modList->loadModList(*this);});
    connect(actionSettings, &QAction::triggered, this, &ProgWindow::handleSettings);
}

ProgWindow::~ProgWindow()
{
    delete ui;
}

void ProgWindow::handleSettings(){
    //open a settings window
    //should probably just make the connection to settingswindow::open but this will do for now
    setW->open();
}

void ProgWindow::loadSettings(){
    qDebug() << Q_FUNC_INFO << QCoreApplication::applicationDirPath();
    QString fileIn = QCoreApplication::applicationDirPath() + "/settings.txt";
    QString settingsRead;
    QStringList settingsSplit;
    QFile inputFile(fileIn);
    if (inputFile.exists()){
        inputFile.open(QIODevice::ReadOnly);
        settingsRead = inputFile.readAll();
        settingsSplit = settingsRead.split("\n");
        setW->settingsValues.clear();
        for (int i = settingsSplit.length()-1; i >= 0; i--){
            qDebug() << Q_FUNC_INFO << settingsSplit[i];
            if (settingsSplit[i] != ""){
                setW->settingsValues.push_front(settingsSplit[i].split(":")[1].trimmed());
            }
        }
    } else {
        //settings file doesn't exist, write a file with the default settings
        setW->writeSettings(&inputFile);
    }
}

SettingsWindow::SettingsWindow(ProgWindow *sentParent){

    parent = sentParent;
    savedChanges = true;
    sendUpdate = new QPushButton("Save Settings", this);
    sendUpdate -> setGeometry(QRect(QPoint(25,25), QSize(200,25)));

    connect(sendUpdate, &QPushButton::released, this, &SettingsWindow::updateSettings);
}

void SettingsWindow::updateSettings(){
    qDebug() << Q_FUNC_INFO << QCoreApplication::applicationDirPath();
    QString fileOut = QCoreApplication::applicationDirPath() + "/settings.txt";
    QFile outputFile(fileOut);

    if (!savedChanges){
        qDebug() << Q_FUNC_INFO << "Settings changed";
        outputFile.open(QIODevice::WriteOnly);
        for(int i = 0; i < settingsNames.length(); i++){
            if(i!= 0){
                outputFile.write("\n");
            }
            outputFile.write(settingsNames[i].toUtf8()+": " + settingsValues[i].toUtf8());
        }
        savedChanges = true;
    }

    qDebug() << Q_FUNC_INFO << "Settings updated";
}

void SettingsWindow::writeSettings(QFile *outputFile){
    qDebug() << Q_FUNC_INFO << "Settings file not found. Creating a default settings file.";
    outputFile->open(QIODevice::WriteOnly);
    for(int i = 0; i < settingsNames.length(); i++){
        if(i!= 0){
            outputFile->write("\n");
        }
        outputFile->write(settingsNames[i].toUtf8()+": " + settingsValues[i].toUtf8());
    }
}

void SettingsWindow::open(){
    settingsEdit = new QTableWidget(settingsNames.size(), 2, this);
    settingsEdit->setGeometry(QRect(QPoint(50,50), QSize(300,300)));
    for (int i =0; i < settingsNames.size();i++) {
        QTableWidgetItem *nextSetName = settingsEdit->item(i ,0);
        QTableWidgetItem *nextSetValue = settingsEdit->item(i ,1);
        qDebug() << Q_FUNC_INFO << settingsNames[i] << ": " << settingsValues[i];
        if(!nextSetName){
            nextSetName = new QTableWidgetItem;
            nextSetName->setFlags(Qt::ItemIsEditable);
            settingsEdit->setItem(i ,0, nextSetName);
        }
        nextSetName->setText(settingsNames[i]);
        if(!nextSetValue){
            nextSetValue = new QTableWidgetItem;
            settingsEdit->setItem(i ,1, nextSetValue);
        }
        nextSetValue->setText(settingsValues[i]);
    }
    connect(settingsEdit, &QTableWidget::cellChanged, this, [this](int row, int column) {changeSetting(row, column);});

    show();
}

void SettingsWindow::changeSetting(int row, int column){
    savedChanges = false;
    settingsValues[row] = settingsEdit->item(row,1)->text();;
}

void ProgWindow::messageError(QString message){
    MessagePopup->setText(message);
    MessagePopup->setWindowTitle("Error!");
    MessagePopup->exec();
}

void ProgWindow::messageSuccess(QString message){
    MessagePopup->setText(message);
    MessagePopup->setWindowTitle("Success.");
    MessagePopup->exec();
}
