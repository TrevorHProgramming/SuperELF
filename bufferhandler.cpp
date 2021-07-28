#include "mainwindow.h"

void MainWindow::updateFileBuffer(){
    //Pull data from the full file buffer and translate it
    //much more efficient than translating the entire file and holding that in RAM
    int BufferEnd = BufferStart + 256;
    int BufferNow = BufferStart;
    WindowBuffer = "";
    MipsBuffer = "";
    WindowBuffer.append(MainWindow::filebuffer.mid(BufferStart, BufferEnd));
    linelist->clear();
    addresslist->clear();
    QString temp = "";
    int j = 0;
    for (int i = 0; i < 256; i+=4){
        addresslist[j] = QString::number(BufferNow, 16).rightJustified(8, '0');
        temp = " | ";
        //actual code will translate here and append to line
        QString realInst = hex_to_bin(WindowBuffer.mid(i,4));
        temp += convToInstruction(realInst);
        temp += '\n';

        linelist[j] = temp;
        BufferNow += 4;
        j += 1;
    }
    updateWindowBuffer();
}

void MainWindow::updateWindowBuffer(){
    MipsBuffer = "";
    for (int i = 0; i < 64; ++i){
        MipsBuffer += addresslist[i];
        MipsBuffer += linelist[i];
    }
    MipsWindow->setText(MipsBuffer);
}

void MainWindow::loadFile(){
    QString entry = userInPath -> text();
    if (entry.length() != 0){
        fileInPath = entry;
        QFile filein(fileInPath);
        if (!filein.open(QIODevice::ReadOnly)) return;
        filebuffer = filein.readAll();
        updateFileBuffer();
    }

}

void MainWindow::saveFile(){
    QString entry = userOutPath -> text();
    if (entry.length() != 0){
        fileOutPath = entry;
        QFile fileout(fileOutPath);
        if(!fileout.open(QIODevice::WriteOnly)) return;
        fileout.write(filebuffer);
    }
}

void MainWindow::handleInsert(){
    //step 1: check to see if anything has been typed in the entry line
    //step 2: check to see if something is selected in MipsWindow
    //step 3: insert new line after selected line into the buffer
    //step 4: update MipsWindow and clear entry
    QString entry = InstructionBox -> text();
    QString selection = MipsWindow->textCursor().QTextCursor::selectedText();
    QByteArray convReturn;
    if (entry.length() != 0 and selection.length() != 0){
        ButtonInsert->setText("Inserting...");
        QString selectaddress = selection.left(8);
        int numselectaddress = selectaddress.toInt(nullptr, 16);

        //convert entry to instruction. then convert each part of the instruction into an int and put thos into bytearrays. return the bytearrays
        //ex 10000001 00000000 00000000 00000001 -> 129 0 0 1 -> loop to convert each to bytearray then combine them
        convReturn = convFromInst(entry);
        MainWindow::filebuffer.insert(numselectaddress, convReturn);
        updateFileBuffer();
        ButtonInsert->setText("Inserted");
        ButtonInsert->resize(100,30);

    }
}

void MainWindow::handleDelete(){
    //get address of line and remove from buffer
    QString selection = MipsWindow->textCursor().QTextCursor::selectedText();
    qDebug() << selection;
    QString selectaddress = selection.left(8);
    int numselectaddress = selectaddress.toInt(nullptr, 16);
    MainWindow::filebuffer.replace(numselectaddress, 4, "");
    updateFileBuffer();
}
