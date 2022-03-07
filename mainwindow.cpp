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


ProgWindow::ProgWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    int buttonHeight = 35;
    int labelHeight = 15;

    modList = new ModdedData;
    branchFinder = new BranchFinder;

    ui->setupUi(this);
    int longScroll = 64;
    int shortScroll = 16;

    //addressOffset = 542360; //hornet lineup
    //addressOffset = 0;

    mainReader = new MIPSReader();
    mainReader->start(*mainReader);

    SettingsWindow = new QWidget();

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

    MipsWindow = new QPlainTextEdit(this);
    MipsWindow -> setGeometry(QRect(QPoint(int(hSize*.02),int(vSize*0.055)), QSize(int(hSize*0.38),int(vSize*0.92))));
    MipsWindow->setReadOnly(true);
    MipsWindow->setPlainText(mainReader->MipsBuffer);

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

    /*ButtonSettings = new QPushButton("Settings", this);
    ButtonSettings -> setGeometry(QRect(QPoint(int(hSize*.73),360), QSize(int(hSize*.23),buttonHeight)));

    ButtonLoad = new QPushButton("Load ELF", this);
    ButtonLoad -> setGeometry(QRect(QPoint(int(hSize*.85),250), QSize(int(hSize*.11),buttonHeight)));
    ButtonExport = new QPushButton("Save ELF", this);
    ButtonExport -> setGeometry(QRect(QPoint(int(hSize*.73),250), QSize(int(hSize*.11),buttonHeight)));

    ButtonLoadMods = new QPushButton("Load Mods", this);
    ButtonLoadMods -> setGeometry(QRect(QPoint(int(hSize*.85),280), QSize(int(hSize*.11),buttonHeight)));
    ButtonSaveMod = new QPushButton("Save Mod", this);
    ButtonSaveMod -> setGeometry(QRect(QPoint(int(hSize*.73),280), QSize(int(hSize*.11),buttonHeight)));

    ButtonISO = new QPushButton("Save ISO", this);
    ButtonISO -> setGeometry(QRect(QPoint(int(hSize*.73),325), QSize(int(hSize*.23),buttonHeight)));*/
    MessagePopup = new QMessageBox(this);

    connect(ButtonDelete, &QPushButton::released, [this] {mainReader->handleDelete(*this);});
    connect(ButtonInsert, &QPushButton::released, [this] {mainReader->handleInsert(*this);});
    connect(ButtonReplace, &QPushButton::released, [this] {mainReader->handleReplace(*this);});
    connect(Button10Up, &QPushButton::released, [this, longScroll] {mainReader->scrollMIPS(-longScroll, *this);});
    connect(Button10Down, &QPushButton::released, [this, longScroll] {mainReader->scrollMIPS(longScroll, *this);});
    connect(Button1Up, &QPushButton::released, [this, shortScroll] {mainReader->scrollMIPS(-shortScroll, *this);});
    connect(Button1Down, &QPushButton::released, [this, shortScroll] {mainReader->scrollMIPS(shortScroll, *this);});

//    connect(Button10Up, SIGNAL(QPushButton::released()), this, SLOT(mainReader.scrollMIPS(-longScroll)));
//    connect(Button10Down, SIGNAL(QPushButton::released()), this, SLOT(mainReader.scrollMIPS(longScroll)));
//    connect(Button1Up, SIGNAL(QPushButton::released()), this, SLOT(mainReader.scrollMIPS(-shortScroll)));
//    connect(Button1Down, SIGNAL(QPushButton::released()), this, SLOT(mainReader.scrollMIPS(shortScroll)));

    connect(ButtonAddress, &QPushButton::released, [this] {mainReader->jumpAddress(*this);});
    //connect(ButtonAddress, SIGNAL(QPushButton::released()), this, SLOT(mainReader.jumpAddress(this)));
    //connect(actionLoadELF, &QAction::triggered, this, [this] {mainReader->loadFile(*this->mainReader,*this);});
    connect(actionLoadELF, &QAction::triggered, [this] {mainReader->loadFile(*this);});
    connect(actionSaveELF, &QAction::triggered, [this] {mainReader->saveFile(*this);});
    connect(actionSaveISO, &QAction::triggered, [this] {mainReader->isoSearcher(*this);});

    connect(MipsWindow, &QPlainTextEdit::cursorPositionChanged, this, &ProgWindow::handleSelect);

    connect(actionSaveMod, &QAction::triggered, [this] {modList->makeModList(*this);});
    connect(actionLoadMod, &QAction::triggered, [this] {modList->loadModList(*this);});
//    connect(actionLoadMod, &QAction::triggered, this, &ProgWindow::loadModList);
//    connect(actionSaveMod, &QAction::triggered, this, &ProgWindow::makeModList);
//    connect(actionSettings, &QAction::triggered, this, &ProgWindow::handleSettings);
    //connect(actionLoadMod, &QAction::triggered, [this](){ProgWindow::handleOpen();});

    //connect(ButtonSaveMod, &QPushButton::released, this, &MainWindow::makeModList);
    //connect(ButtonLoadMods, &QPushButton::released, this, &MainWindow::loadModList);

    //connect(ButtonISO, &QPushButton::released, this, &MainWindow::isoSearcher);
}

ProgWindow::~ProgWindow()
{
    delete ui;
}

void ProgWindow::handleSelect(){
    //selects a line of text.
    if (MipsWindow->textCursor().atBlockEnd() == false){
        MipsWindow->moveCursor(QTextCursor::StartOfLine);
        MipsWindow->moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    }
}

void ProgWindow::handleSettings(){
    //open a settings window
    SettingsWindow->show();
}

void ProgWindow::messageError(QString message){
    MessagePopup->setText(message);
    MessagePopup->setWindowTitle("Error!");
    MessagePopup->open();
}

void ProgWindow::messageSuccess(QString message){
    MessagePopup->setText(message);
    MessagePopup->setWindowTitle("Success.");
    MessagePopup->open();
}
