#ifndef MIPSREADER_H
#define MIPSREADER_H

#include <QByteArray>
#include <QString>
#include "InstToBin.h"
#include "BinToInst.h"

class ProgWindow;

class MIPSReader{
  public:
    void updateFileBuffer(ProgWindow& MainWindow);
    void updateWindowBuffer(ProgWindow& MainWindow);
    void start(MIPSReader& mainReader);

    BinToInst *binToInst;
    InstToBin *instToBin;

    long long addressOffset;
    long long BufferStart;
    QByteArray filebuffer;
    QByteArray WindowBuffer;
    QString addresslist[128];
    QString linelist[64];
    QString fileInPath;
    QString fileOutPath;
    QString MipsBuffer;

 public slots:
    void loadFile(ProgWindow& MainWindow);
    void saveFile(ProgWindow& MainWindow);
    void isoSearcher(ProgWindow& MainWindow);
    void scrollMIPS(int amount, ProgWindow& MainWindow);
    void handleInsert(ProgWindow& MainWindow);
    void handleDelete(ProgWindow& MainWindow);
    void handleReplace(ProgWindow& MainWindow);
    void jumpAddress(ProgWindow& MainWindow);
};

#endif // MIPSREADER_H
