#include "mainwindow.h"
#include "ui_mainwindow.h"

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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    MipsBuffer = " ";
    BufferStart = 256;

    ui->setupUi(this);
    int longScroll = 64;
    int shortScroll = 16;

    SettingsWindow = new QWidget();

    ButtonInsert = new QPushButton("Insert", this);
    ButtonInsert -> setGeometry(QRect(QPoint(440,90), QSize(100,30)));
    InstructionBox = new QLineEdit(this);
    InstructionBox -> setGeometry(QRect(QPoint(440,50), QSize(300,30)));
    ButtonDelete = new QPushButton("Delete", this);
    ButtonDelete -> setGeometry(QRect(QPoint(540,90), QSize(100,30)));
    radioHex = new QRadioButton("Hex view", this);
    radioHex -> setGeometry(QRect(QPoint(440,120), QSize(100,30)));
    radioInst = new QRadioButton("Instruction view", this);
    radioInst -> setGeometry(QRect(QPoint(540,120), QSize(120,30)));
    radioInst-> toggle();


    ButtonSettings = new QPushButton("Settings", this);
    ButtonSettings -> setGeometry(QRect(QPoint(540,190), QSize(100,30)));


    ButtonLoad = new QPushButton("Load", this);
    ButtonLoad -> setGeometry(QRect(QPoint(540,250), QSize(100,30)));
    ButtonExport = new QPushButton("Save", this);
    ButtonExport -> setGeometry(QRect(QPoint(540,280), QSize(100,30)));
    MipsWindow = new QTextBrowser(this);
    MipsWindow -> setGeometry(QRect(QPoint(50,50), QSize(300,500)));
    MipsWindow->setText(MipsBuffer);
    Button10Up = new QPushButton("^^", this);
    Button10Up -> setGeometry(QRect(QPoint(350,190), QSize(30,30)));
    Button10Down = new QPushButton("vv", this);
    Button10Down -> setGeometry(QRect(QPoint(350,310), QSize(30,30)));
    Button1Up = new QPushButton("^", this);
    Button1Up -> setGeometry(QRect(QPoint(350,220), QSize(30,30)));
    Button1Down = new QPushButton("v", this);
    Button1Down -> setGeometry(QRect(QPoint(350,280), QSize(30,30)));
    AddressBox = new QLineEdit(this);
    AddressBox -> setGeometry(QRect(QPoint(350,250), QSize(75,30)));
    ButtonAddress = new QPushButton("Jump", this);
    ButtonAddress -> setGeometry(QRect(QPoint(425,250), QSize(50,30)));
    connect(MipsWindow, &QTextBrowser::cursorPositionChanged, this, &MainWindow::handleSelect);
    connect(ButtonDelete, &QPushButton::released, this, &MainWindow::handleDelete);
    connect(ButtonInsert, &QPushButton::released, this, &MainWindow::handleInsert);
    connect(ButtonSettings, &QPushButton::released, this, &MainWindow::handleSettings);
    connect(ButtonLoad, &QPushButton::released, this, &MainWindow::loadFile);
    connect(ButtonExport, &QPushButton::released, this, &MainWindow::saveFile);

    connect(Button10Up, &QPushButton::released, [this, longScroll]{MainWindow::scrollMips(-longScroll);});
    connect(Button10Down, &QPushButton::released, [this, longScroll]{MainWindow::scrollMips(longScroll);});
    connect(Button1Up, &QPushButton::released, [this, shortScroll]{MainWindow::scrollMips(-shortScroll);});
    connect(Button1Down, &QPushButton::released, [this, shortScroll]{MainWindow::scrollMips(shortScroll);});
    connect(ButtonAddress, &QPushButton::released, this, &MainWindow::jumpAddress);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::handleSelect(){
    //selects a line of text.
    if (MipsWindow->textCursor().atBlockEnd() == false){
        MipsWindow->moveCursor(QTextCursor::StartOfLine);
        MipsWindow->moveCursor(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    }
}

void MainWindow::handleSettings(){
    //open a settings window
    SettingsWindow->show();
}

void MainWindow::scrollMips(int amount){
    BufferStart += amount;
    MainWindow::updateFileBuffer();
}

void MainWindow::jumpAddress(){
    QString entry = AddressBox -> text();
    if (entry.length() != 0){
        QString jumpaddress = entry;
        int numjumpaddress = jumpaddress.toInt(nullptr, 16);
        BufferStart = numjumpaddress;
        updateFileBuffer();
    }
    else {
        qDebug() << "Not a valid address.";
    }
}
