#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QLabel>
#include <QMouseEvent>
#include <QTextDocumentFragment>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QBitArray>
#include <QIODevice>
#include <QRadioButton>
#include <QFileDialog>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QByteArrayMatcher>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QListWidget>

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <iomanip>
#include <vector>

#include "BinChanger.h"
#include "MIPSReader.h"
#include "BranchFinder.h"
#include "ModManager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
namespace Ui { class MIPSBrowser; }
QT_END_NAMESPACE

extern QString genRegList[];
extern QString floatRegList[];

class SettingsWindow : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWindow(ProgWindow *sentParent = nullptr);
    ~SettingsWindow();
    ProgWindow *parent;
    QStringList settingsNames = {"Starting window height","Starting window width","Address offset","Starting address","Long scroll length","Short scroll length"};
    QStringList settingsValues = {"1024","768","0","0","16","4"}; //these are stored as strings since the address ones will be in hex
    QTableWidget *settingsEdit;

    void open();
    void writeSettings(QFile *outputFile);
    void changeSetting(int row, int column);

private slots:
    void updateSettings();
private:
    QPushButton *sendUpdate;
    bool savedChanges;
};

class ProgWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProgWindow(QWidget *parent = nullptr);
    ~ProgWindow();
    int hSize = 1024;
    int vSize = 768;
    Ui::MainWindow *ui;
    SettingsWindow *setW;
    QMenuBar *menuMain;
    MIPSReader *mainReader;
    BranchFinder *branchFinder;
    BinChanger binChanger;
    ModdedData *modList;

    QLabel *LabelInstruction;
    QLineEdit *InstructionBox;
    QPushButton *ButtonInsert;
    QPushButton *ButtonDelete;
    QPushButton *ButtonReplace;

    QLabel *LabelAddress;
    FileData fileData;

    QListWidget *MipsList;
    //QTableWidget *MipsTable;

    QPushButton *Button1Up;
    QPushButton *Button10Up;
    QPushButton *Button1Down;
    QPushButton *Button10Down;
    QLineEdit *AddressBox;
    QPushButton *ButtonAddress;

    QPushButton *ButtonSettings;
    QPushButton *ButtonLoad;
    QPushButton *ButtonExport;
    QPushButton ButtonUpdateSettings;

    QPushButton *ButtonSaveMod;
    QPushButton *ButtonLoadMods;
    QPushButton *ButtonISO;
    QMessageBox *MessagePopup;

    QRadioButton *radioInst;
    QRadioButton *radioHex;

public slots:

    void handleSettings();

//    void loadModList();
//    void makeModList();
    void messageError(QString message);
    void messageSuccess(QString message);

    qint64 byteWrite( QFile& file, int8_t var );
    qint64 shortWrite( QFile& file, int16_t var );
    qint64 intWrite( QFile& file, int32_t var );
    qint64 longWrite( QFile& file, int64_t var );

private:
    void loadSettings();
};



#endif // MAINWINDOW_H
