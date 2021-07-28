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
    QString convToInstruction(QString input);
    QString hex_to_bin(QByteArray arrhex);
    QByteArray convFromInst(QString instruction);

private:
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
    QLabel *labelPathIn;
    QLabel *labelPathOut;
    QLineEdit *userInPath;
    QLineEdit *userOutPath;
    QString fileInPath;
    QString fileOutPath;
    QPushButton ButtonUpdateSettings;
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
