#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QPlainTextEdit>
#include <QLabel>
#include <QMouseEvent>
#include <QTextCursor>
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

class ProgWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ProgWindow(QWidget *parent = nullptr);
    ~ProgWindow();
    const static int hSize = 1024;
    const static int vSize = 768;
    Ui::MainWindow *ui;
    QMenuBar *menuMain;
    MIPSReader *mainReader;
    BranchFinder *branchFinder;
    BinChanger *binChanger;
    ModdedData *modList;

    QLabel *LabelInstruction;
    QLineEdit *InstructionBox;
    QPushButton *ButtonInsert;
    QPushButton *ButtonDelete;
    QPushButton *ButtonReplace;

    QLabel *LabelAddress;
    QPlainTextEdit *MipsWindow;

    QPushButton *Button1Up;
    QPushButton *Button10Up;
    QPushButton *Button1Down;
    QPushButton *Button10Down;
    QLineEdit *AddressBox;
    QPushButton *ButtonAddress;

    QPushButton *ButtonSettings;
    QWidget *SettingsWindow;
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

    void handleSelect();
    void handleSettings();

//    void loadModList();
//    void makeModList();
    void messageError(QString message);
    void messageSuccess(QString message);

    qint64 byteWrite( QFile& file, int8_t var );
    qint64 shortWrite( QFile& file, int16_t var );
    qint64 intWrite( QFile& file, int32_t var );
    qint64 longWrite( QFile& file, int64_t var );
};

/*class SettingsWindow : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private slots:
    void updateSettings();
private:
    QPushButton *sendUpdate;
};*/

#endif // MAINWINDOW_H
