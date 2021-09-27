#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QListView>
#include <QTextBrowser>
#include <QLabel>
#include <QString>
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

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <iomanip>
#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
namespace Ui { class MIPSBrowser; }
QT_END_NAMESPACE

extern QString genRegList[];
extern QString floatRegList[];

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleInsert();
    void handleSelect();
    void handleSettings();
    void handleDelete();
    void updateFileBuffer();
    void updateWindowBuffer();
    void loadFile();
    void scrollMips(int amount);
    void saveFile();
    void jumpAddress();
    void loadModList();
    void makeModList();
    void checkTable(int row, int column);
    QString reverse_input(QString input, int unitLength);
    QString convToInstruction(QString input);
    QString hex_to_bin(QByteArray arrhex);
    QByteArray convFromInst(QString instruction);
    qint64 byteWrite( QFile& file, int8_t var );
    qint64 shortWrite( QFile& file, int16_t var );
    qint64 intWrite( QFile& file, int32_t var );
    qint64 longWrite( QFile& file, int64_t var );

private:
    long long addressOffset;
    int BufferStart;
    Ui::MainWindow *ui;

    QLineEdit *InstructionBox;
    QPushButton *ButtonInsert;
    QPushButton *ButtonDelete;

    QTextBrowser *MipsWindow;
    QString MipsBuffer;
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
    QByteArray filebuffer;
    QByteArray WindowBuffer;
    QString addresslist[128];
    QString linelist[64];
    QString fileInPath;
    QString fileOutPath;
    QPushButton ButtonUpdateSettings;

    QTableWidget *TableMods;
    QPushButton *ButtonSaveMod;
    QPushButton *ButtonLoadMods;

    QRadioButton *radioInst;
    QRadioButton *radioHex;
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
