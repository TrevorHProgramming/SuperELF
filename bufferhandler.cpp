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
        if (!filein.open(QIODevice::ReadOnly)){
            MessagePopup->setText("Could not open file " + fileInPath);
            MessagePopup->setWindowTitle("File error!");
            MessagePopup->open();
            return;
        }
        filebuffer = filein.readAll();
        updateFileBuffer();
    }

}

void MainWindow::saveFile(){
    QString entry = QFileDialog::getSaveFileName(this, tr("Save ELF"), QDir::currentPath(), tr("Binary File (*.bin)"));
    if (entry.length() != 0){
        fileOutPath = entry;
        QFile fileout(fileOutPath);
        if(!fileout.open(QIODevice::WriteOnly)) {
            MessagePopup->setText("Could not open file " + fileOutPath);
            MessagePopup->setWindowTitle("File error!");
            MessagePopup->open();
            return;
        }
        fileout.write(filebuffer);
    }
    MessagePopup->setText("ELF Saved.");
    MessagePopup->setWindowTitle("Save complete");
    MessagePopup->open();
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

void MainWindow::handleReplace(){
    QString entry = InstructionBox -> text();
    QString selection = MipsWindow->textCursor().QTextCursor::selectedText();
    QByteArray convReturn;
    QByteArray revReturn;
    if (entry.length() != 0 and selection.length() != 0){
        ButtonReplace->setText("Replacing...");
        QString selectaddress = selection.left(8);
        int numselectaddress = selectaddress.toInt(nullptr, 16) - addressOffset;
        //convert entry to instruction. then convert each part of the instruction into an int and put those into bytearrays. return the bytearrays
        //ex 10000001 00000000 00000000 00000001 -> 129 0 0 1 -> loop to convert each to bytearray then combine them
        if (radioInst->isChecked()){
            convReturn = convFromInst(entry);
            MainWindow::filebuffer.replace(numselectaddress, 4, convReturn);
        }
        else {
            convReturn = QByteArray::fromHex(entry.toUtf8());
            for (int k = 0; k <4; ++k){
                revReturn += convReturn.mid(3-k, 1);
            }
            MainWindow::filebuffer.replace(numselectaddress, 4, revReturn);
        }
        updateFileBuffer();
        ButtonReplace->setText("Inserted");
        ButtonReplace->resize(100,30);
    }
}

QByteArray makeNullArray(int length){
    QByteArray partFiller;
    for (int i = 0; i < length; i++){
        partFiller.append(QByteArray::fromHex("00"));
    }
    return partFiller;
}

void MainWindow::isoSearcher(){
    //alright here's how this is gonna go
    //read 1MB of the file into a buffer
    //search in 32 byte chunks for the first 2 lines of the ELF (32 bytes) in that buffer starting from 0
    //then if a match isn't found, start from 1 and search again until you reach 32
    //if a match is not found in that section, clear the buffer and use the next 1MB
    //once the match is found, take its location in bytes and stop searching
    //then swap to what's already been coded and instead of checking for the value, check if you've reached the determined starting point
    //might be slow but we'll see how bad it actually is

    QString isoInPath = QFileDialog::getOpenFileName(this, tr("Select ISO"), QDir::currentPath(), tr("ISO Files (*.iso)"));
    QString isoOutPath = QFileDialog::getSaveFileName(this, tr("Save ISO"), QDir::currentPath(), tr("ISO Files (*.iso)"));
    //std::ifstream isoInFile(isoInPath.toStdString(), std::ios::binary);
    QFile isoInFile(isoInPath);
    QFile isoOutFile(isoOutPath);
    long long currentAddress = 0;
    //if (!isoInFile){
    if(!isoInFile.open(QIODevice::ReadOnly)){
        qDebug() << "Could not open input iso";
        MessagePopup->setText("Could not open input file " + isoInPath);
        MessagePopup->setWindowTitle("File error!");
        MessagePopup->open();
        return;
    }
    if(!isoOutFile.open(QIODevice::WriteOnly)){
        qDebug() << "Could not open output iso";
        MessagePopup->setText("Could not open output file " + isoOutPath);
        MessagePopup->setWindowTitle("File error!");
        MessagePopup->open();
        return;
    }
    uint32_t value;
    bool startNewData = 0;
    QByteArray elfSearch = filebuffer.mid(0,32);
    int location = 0;
    long bufferSize = 2048; //read as iso partitions
    int leftoverBuffer = filebuffer.size()%bufferSize;
    QByteArray isoSearchBuffer;
    QByteArrayMatcher matcher(elfSearch);
    unsigned long long isoFileSize = isoInFile.size();
    qDebug() << "File size: " << isoFileSize;
    for(unsigned long long i = 0; i < (isoFileSize-bufferSize); i += bufferSize){
        isoInFile.seek(i);
        isoSearchBuffer = isoInFile.read(bufferSize);
        location = matcher.indexIn(isoSearchBuffer, 0);
        if (location != -1){
            qDebug() << "Sequence found at: " << location << " with leftover: " << location%bufferSize;
            isoInFile.seek(i);
            isoSearchBuffer = isoInFile.read(location);
            isoOutFile.write(isoSearchBuffer);
            isoOutFile.write(filebuffer);
            isoOutFile.write(makeNullArray(leftoverBuffer));
            location += i + filebuffer.size() + leftoverBuffer;
            break;
            //i += isoSearchBuffer.size() + filebuffer.size();
        }
        else{
            isoOutFile.write(isoSearchBuffer);
        }
    }
    bufferSize = 1048576; //not looking for anything specific now, so increase the buffer size to speed things up
    for(unsigned long long i = location; i < isoFileSize; i += bufferSize){
        if (i < (isoFileSize-bufferSize)){
            isoInFile.seek(i);
            isoSearchBuffer = isoInFile.read(bufferSize);
            isoOutFile.write(isoSearchBuffer);
        }
        else {
            bufferSize = isoFileSize - i;
            isoInFile.seek(i);
            isoSearchBuffer = isoInFile.read(bufferSize);
            isoOutFile.write(isoSearchBuffer);
        }

    }
    MessagePopup->setText("ISO Saved.");
    MessagePopup->setWindowTitle("Save complete");
    MessagePopup->open();
    qDebug() << "Done.";
}

void MainWindow::findJJAL(long addressChanged, int linesAdded){
    //search file in words. check last section of word for valid address.
    for (long i = 0; i < filebuffer.length(); i+=4){
        QString replacement;
        QByteArray replacementArray;
        QString revChanged;
        int instruction = 0;
        unsigned long potentialAddress = 0;
        QString tempRead = hex_to_bin(filebuffer.mid(i, 4));
        QString revRead = reverse_input(tempRead, 4);
        potentialAddress = revRead.mid(6,26).toULong(nullptr, 2) << 2;
        instruction = revRead.mid(0,5).toInt(nullptr, 2);
        //check if first 6 bits correspond to J or JAL instruction.
        //this could lead to false positives on its own, requiring additional checks
        if (instruction == 3 or instruction == 2){
            //check if potential address is between start of ELF and end of ELF
            //if both this and the instruction check pass, this should safely be a valid target
            if (potentialAddress > addressOffset and potentialAddress < (addressOffset + filebuffer.length())){
                //check if the address needs to be changed at all
                if(potentialAddress > addressChanged){
                    //increase the address reference by the amount needed
                    //then reverse, convert, and replace in the filebuffer
                    potentialAddress += linesAdded*4;
                    revChanged = revRead.mid(0,5);
                    revChanged += reverse_input(QString::number(potentialAddress >> 2, 2), 4);
                    replacement = QString::number(revChanged.toULong(nullptr, 2), 16);
                    replacementArray = replacement.toUtf8();
                    filebuffer.replace(i, 4, replacementArray);
                }
            }
        }
    }
}


//checkBranch, findBranch
bool MainWindow::checkBranch(QString input){
    //gonna do this the quick and ugly way for now
    //need to add COP branches
    int pt1 = input.mid(0,5).toInt(nullptr, 2);
    int pt2 = input.mid(6,11).toInt(nullptr, 2);
    int pt3 = input.mid(12,17).toInt(nullptr, 2);
    if(pt1 == 4 or pt1 == 5 or pt1 == 6 or pt1 == 7 or pt1 == 20 or pt1 == 21 or pt1 == 22 or pt1 == 23){
        return true;
    }
    if(pt1 == 1){
        if(pt3 == 0 or pt3 == 1 or pt3 == 2 or pt3 == 3 or pt3 == 16 or pt3 == 17 or pt3 == 18 or pt3 == 19){
            return true;
        }
    }
    return false;
}

void MainWindow::findBranch(long addressChanged, int linesAdded){
    //search region around address changed for branch instructions
    //adjust branch amount by lines added, whichever direction that is
    unsigned long startBranchRange = 0;
    unsigned long endBranchRange = 0;
    if (addressChanged - 131072 > addressOffset){
        startBranchRange = addressChanged - 131072;
    }
    else{
        startBranchRange = addressOffset;
    }
    if (addressChanged+(linesAdded*4)+131072 < filebuffer.length()+addressOffset){
        endBranchRange = addressChanged+(linesAdded*4)+131072;
    }
    else {
        endBranchRange = filebuffer.length()+addressOffset;
    }
    for(long i = startBranchRange; i < endBranchRange; i += 4){
        QString replacement;
        QByteArray replacementArray;
        QString revChanged;
        unsigned long potentialAddress = 0;
        QString tempRead = hex_to_bin(filebuffer.mid(i, 4));
        QString revRead = reverse_input(tempRead, 4);
        potentialAddress = i + (revRead.mid(16,16).toULong(nullptr, 2) << 2);
        //check if this appears to be a branch instruction.
        //this could lead to false positives on its own, requiring additional checks
        if (checkBranch(revRead)){
            //check if potential address is between start of ELF and end of ELF
            //if both this and the instruction check pass, this should safely be a valid target
            if (potentialAddress > addressOffset and potentialAddress < (addressOffset + filebuffer.length())){
                //check if the address needs to be changed at all
                if(potentialAddress > addressChanged){
                    //increase the address reference by the amount needed
                    //then reverse, convert, and replace in the filebuffer
                    potentialAddress += linesAdded*4;
                    revChanged = revRead.mid(0,15);
                    revChanged += reverse_input(QString::number((potentialAddress - i) >> 2, 2), 4);
                    replacement = QString::number(revChanged.toULong(nullptr, 2), 16);
                    replacementArray = replacement.toUtf8();
                    filebuffer.replace(i, 4, replacementArray);
                }
            }
        }
    }
}
