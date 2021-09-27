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
    QString realInst;
    QString realHex;
    QByteArray tempBuffer;
    int j = 0;
    if (radioInst->isChecked()){
        for (int i = 0; i < 256; i+=4){
            addresslist[j] = QString::number(BufferNow + addressOffset, 16).rightJustified(8, '0');
            temp = " | ";
            //actual code will translate here and append to line
            realInst = hex_to_bin(WindowBuffer.mid(i,4));
            temp += convToInstruction(realInst);
            temp += '\n';

            linelist[j] = temp;
            BufferNow += 4;
            j += 1;
        }
    }
    else {
        for (int i = 0; i < 256; i+=4){
            addresslist[j] = QString::number(BufferNow + addressOffset, 16).rightJustified(8, '0');
            tempBuffer.clear();
            temp = " | ";
            for (int k = 0; k <4; ++k){
                tempBuffer += WindowBuffer.mid(i+3-k, 1);
            }
            realHex = QString(tempBuffer.toHex());
            //realHex = QString(WindowBuffer.mid(i,4).toHex()); //needs to be reversed
            temp += realHex;
            temp += '\n';
            linelist[j] = temp;
            BufferNow += 4;
            j += 1;
        }
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
    QString entry = QFileDialog::getOpenFileName(this, tr("Select ELF"), QDir::currentPath(), tr("All Files (*.*)"));
    if (entry.length() != 0){
        fileInPath = entry;
        QFile filein(fileInPath);
        if (!filein.open(QIODevice::ReadOnly)) return;
        filebuffer = filein.readAll();
        updateFileBuffer();
    }

}

void MainWindow::saveFile(){
    QString entry = QFileDialog::getSaveFileName(this, tr("Save ELF"), QDir::currentPath(), tr("Binary File (*.bin)"));
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
    QByteArray revReturn;
    if (entry.length() != 0 and selection.length() != 0){
        ButtonInsert->setText("Inserting...");
        QString selectaddress = selection.left(8);
        int numselectaddress = selectaddress.toInt(nullptr, 16) - addressOffset;

        //convert entry to instruction. then convert each part of the instruction into an int and put thos into bytearrays. return the bytearrays
        //ex 10000001 00000000 00000000 00000001 -> 129 0 0 1 -> loop to convert each to bytearray then combine them
        if (radioInst->isChecked()){
            convReturn = convFromInst(entry);
            filebuffer.insert(numselectaddress, convReturn);
            updateFileBuffer();
            ButtonInsert->setText("Inserted");
            ButtonInsert->resize(100,30);
        }
        else {
            convReturn = QByteArray::fromHex(entry.toUtf8());
            for (int k = 0; k <4; ++k){
                revReturn += convReturn.mid(3-k, 1);
            }
            filebuffer.insert(numselectaddress, revReturn);
            updateFileBuffer();
            ButtonInsert->setText("Inserted");
            ButtonInsert->resize(100,30);
        }

    }
}

void MainWindow::handleDelete(){
    //get address of line and remove from buffer
    QString selection = MipsWindow->textCursor().QTextCursor::selectedText();
    qDebug() << selection;
    QString selectaddress = selection.left(8);
    int numselectaddress = selectaddress.toInt(nullptr, 16) - addressOffset;
    filebuffer.replace(numselectaddress, 4, "");
    updateFileBuffer();
}
