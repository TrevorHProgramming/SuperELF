#include "mainwindow.h"

void MIPSReader::start(MIPSReader& mainReader){
    mainReader.addressOffset = 1048320;
    mainReader.BufferStart = 256;
    mainReader.MipsBuffer = " ";
    mainReader.filebuffer = QByteArray(0);
}

void MIPSReader::updateFileBuffer(ProgWindow& MainWindow){
    //Pull data from the full file buffer and translate it
    //much more efficient than translating the entire file and holding that in RAM
    int BufferEnd = BufferStart + 256;
    int BufferNow = BufferStart;
    WindowBuffer = "";
    MipsBuffer = "";
    WindowBuffer.append(filebuffer.mid(BufferStart, BufferEnd));
    linelist->clear();
    addresslist->clear();
    QString temp = "";
    QString realInst;
    QString realHex;
    QByteArray tempBuffer;
    int j = 0;
    if (MainWindow.radioInst->isChecked()){
        for (int i = 0; i < 256; i+=4){
            addresslist[j] = QString::number(BufferNow + addressOffset, 16).rightJustified(8, '0');
            temp = " | ";
            //actual code will translate here and append to line
            realInst = MainWindow.binChanger->hex_to_bin(WindowBuffer.mid(i,4));
            temp += binToInst->convToInstruction(realInst, MainWindow);
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
    updateWindowBuffer(MainWindow);
}

void MIPSReader::updateWindowBuffer(ProgWindow& MainWindow){
    MipsBuffer = "";
    for (int i = 0; i < 64; ++i){
        MipsBuffer += addresslist[i];
        MipsBuffer += linelist[i];
    }
    MainWindow.MipsWindow->setPlainText(MipsBuffer);
}

void MIPSReader::scrollMIPS(int amount, ProgWindow& MainWindow){
    BufferStart += amount;
    updateFileBuffer(MainWindow);
}

void MIPSReader::jumpAddress(ProgWindow& MainWindow){
    QString entry = MainWindow.AddressBox -> text();
    if (entry.length() != 0){
        QString jumpaddress = entry;
        long long numjumpaddress = jumpaddress.toLongLong(nullptr, 16) - addressOffset;
        BufferStart = numjumpaddress;
        updateFileBuffer(MainWindow);
    }
    else {
        qDebug() << "Not a valid address.";
        MainWindow.messageError("Not a valid address.");
    }
}

void MIPSReader::loadFile(ProgWindow& MainWindow){
    QString entry = QFileDialog::getOpenFileName(&MainWindow, MainWindow.tr("Load ELF"), QDir::currentPath(), MainWindow.tr("All Files (*.*)"));
    qDebug() << "entry value is: " << entry;
    if (entry.length() != 0){
        QFile filein(entry);
        if (!filein.open(QIODevice::ReadOnly)){
            MainWindow.MessagePopup->setText("Could not open file " + fileInPath);
            MainWindow.MessagePopup->setWindowTitle("File error!");
            MainWindow.MessagePopup->open();
            return;
        }
        MainWindow.mainReader->filebuffer = QByteArray(filein.readAll());
        MainWindow.modList->clear();
        MainWindow.mainReader->updateFileBuffer(MainWindow);
    }

}

void MIPSReader::saveFile(ProgWindow& MainWindow){
    if (MainWindow.mainReader->filebuffer.isEmpty()) {
        MainWindow.messageError("No file to save. Load an ELF first.");
        return;
    }
    QString entry = QFileDialog::getSaveFileName(&MainWindow, MainWindow.tr("Save ELF"), QDir::currentPath(), MainWindow.tr("Binary Files (*.bin)"));
    if (entry.length() != 0){
        fileOutPath = entry;
        QFile fileout(fileOutPath);
        if(!fileout.open(QIODevice::WriteOnly)) {
            MainWindow.messageError("Could not open file " + fileOutPath);
            return;
        }
        fileout.write(filebuffer);
        MainWindow.messageSuccess("ELF Saved.");
    }
}

void MIPSReader::handleInsert(ProgWindow& MainWindow){
    //step 1: check to see if anything has been typed in the entry line
    //step 2: check to see if something is selected in MipsWindow
    //step 3: insert new line after selected line into the buffer
    //step 4: update MipsWindow and clear entry
    QString entry = MainWindow.InstructionBox -> text();
    QString selection = MainWindow.MipsWindow->textCursor().QTextCursor::selectedText();
    QTextCursor currentCursor(MainWindow.MipsWindow->textCursor());
    int currentLine = currentCursor.position();
    QByteArray convReturn;
    QByteArray revReturn;
    ModLine modLine;
    if (entry.length() != 0 and selection.length() != 0){
        QString selectaddress = selection.left(8);
        int numselectaddress = selectaddress.toInt(nullptr, 16) - addressOffset;

        //convert entry to instruction. then convert each part of the instruction into an int and put thos into bytearrays. return the bytearrays
        //ex 10000001 00000000 00000000 00000001 -> 129 0 0 1 -> loop to convert each to bytearray then combine them
        if (MainWindow.radioInst->isChecked()){
            convReturn = instToBin->convFromInst(entry);
            filebuffer.insert(numselectaddress, convReturn);
            modLine = modLine.create(0, MainWindow.modList->modLineList.size(), numselectaddress, convReturn);
            MainWindow.modList->adjustAddresses(*MainWindow.modList, modLine.address, modLine.lineType);
            MainWindow.modList->modLineList.append(modLine);
            updateFileBuffer(MainWindow);
        }
        else {
            convReturn = QByteArray::fromHex(entry.toUtf8());
            for (int k = 0; k <4; ++k){
                revReturn += convReturn.mid(3-k, 1);
            }
            filebuffer.insert(numselectaddress, revReturn);
            modLine = modLine.create(0, MainWindow.modList->modLineList.size(), numselectaddress, revReturn);
            MainWindow.modList->adjustAddresses(*MainWindow.modList, modLine.address, modLine.lineType);
            MainWindow.modList->modLineList.append(modLine);
            updateFileBuffer(MainWindow);
        }
        currentCursor.setPosition(currentLine);
        MainWindow.MipsWindow->setTextCursor(currentCursor);
    }
}

void MIPSReader::handleDelete(ProgWindow& MainWindow){
    //get address of line and remove from buffer
    QString selection = MainWindow.MipsWindow->textCursor().QTextCursor::selectedText();
    QTextCursor currentCursor(MainWindow.MipsWindow->textCursor());
    int currentLine = currentCursor.position();
    //qDebug() << selection;
    QString selectaddress = selection.left(8);
    int numselectaddress = selectaddress.toInt(nullptr, 16) - addressOffset;


    ModLine modLine;
    modLine = modLine.create(1, MainWindow.modList->modLineList.size(), numselectaddress, 0);
    MainWindow.modList->adjustAddresses(*MainWindow.modList, modLine.address, modLine.lineType);
    MainWindow.modList->modLineList.append(modLine);

    //qDebug() << MainWindow.binChanger->reverse_input(MainWindow.binChanger->hex_to_bin(filebuffer.mid(numselectaddress, 4)), 8);
    filebuffer.replace(numselectaddress, 4, "");
    updateFileBuffer(MainWindow);
    currentCursor.setPosition(currentLine);
    MainWindow.MipsWindow->setTextCursor(currentCursor);
}

void MIPSReader::handleReplace(ProgWindow& MainWindow){
    QString entry = MainWindow.InstructionBox -> text();
    QString selection = MainWindow.MipsWindow->textCursor().QTextCursor::selectedText();
    QTextCursor currentCursor(MainWindow.MipsWindow->textCursor());
    int currentLine = currentCursor.position();
    QByteArray convReturn;
    QByteArray revReturn;
    ModLine modLine;
    if (entry.length() != 0 and selection.length() != 0){
        QString selectaddress = selection.left(8);
        int numselectaddress = selectaddress.toInt(nullptr, 16) - addressOffset;
        //convert entry to instruction. then convert each part of the instruction into an int and put those into bytearrays. return the bytearrays
        //ex 10000001 00000000 00000000 00000001 -> 129 0 0 1 -> loop to convert each to bytearray then combine them
        if (MainWindow.radioInst->isChecked()){
            convReturn = instToBin->convFromInst(entry);
            filebuffer.replace(numselectaddress, 4, convReturn);
            modLine = modLine.create(1, MainWindow.modList->modLineList.size(), numselectaddress, 0);
            MainWindow.modList->adjustAddresses(*MainWindow.modList, modLine.address, modLine.lineType);
            MainWindow.modList->modLineList.append(modLine);
            modLine = modLine.create(0, MainWindow.modList->modLineList.size(), numselectaddress, convReturn);
            MainWindow.modList->adjustAddresses(*MainWindow.modList, modLine.address, modLine.lineType);
            MainWindow.modList->modLineList.append(modLine);
        }
        else {
            convReturn = QByteArray::fromHex(entry.toUtf8());
            /*for (int k = 0; k <4; ++k){
                revReturn += convReturn.mid(3-k, 1);
            }*/ //removing the reversal to make copying from IDA easier
            filebuffer.replace(numselectaddress, 4, revReturn);
            modLine = modLine.create(1, MainWindow.modList->modLineList.size(), numselectaddress, 0);
            MainWindow.modList->adjustAddresses(*MainWindow.modList, modLine.address, modLine.lineType);
            MainWindow.modList->modLineList.append(modLine);
            modLine = modLine.create(0, MainWindow.modList->modLineList.size(), numselectaddress, revReturn);
            MainWindow.modList->adjustAddresses(*MainWindow.modList, modLine.address, modLine.lineType);
            MainWindow.modList->modLineList.append(modLine);
        }
        updateFileBuffer(MainWindow);
    }
    currentCursor.setPosition(currentLine);
    MainWindow.MipsWindow->setTextCursor(currentCursor);
}

//move to BinChanger
QByteArray makeNullArray(int length){
    QByteArray partFiller;
    for (int i = 0; i < length; i++){
        partFiller.append(QByteArray::fromHex("00"));
    }
    return partFiller;
}

void MIPSReader::isoSearcher(ProgWindow& MainWindow){
    if (MainWindow.mainReader->filebuffer.isEmpty()) {
        MainWindow.messageError("No ELF data to write to an ISO. Load an ELF first.");
        return;
    }

    //alright here's how this is gonna go
    //read 1MB of the file into a buffer
    //search in 32 byte chunks for the first 2 lines of the ELF (32 bytes) in that buffer starting from 0
    //then if a match isn't found, start from 1 and search again until you reach 32
    //if a match is not found in that section, clear the buffer and use the next 1MB
    //once the match is found, take its location in bytes and stop searching
    //then swap to what's already been coded and instead of checking for the value, check if you've reached the determined starting point
    //might be slow but we'll see how bad it actually is


    //std::ifstream isoInFile(isoInPath.toStdString(), std::ios::binary);
    QString isoInPath = QFileDialog::getOpenFileName(&MainWindow, MainWindow.tr("Load Base ISO"), QDir::currentPath(), MainWindow.tr("ISO Files (*.iso)"));
    QString isoOutPath = QFileDialog::getSaveFileName(&MainWindow, MainWindow.tr("Save New ISO"), QDir::currentPath(), MainWindow.tr("ISO Files (*.iso)"));

    QFile isoInFile(isoInPath);
    QFile isoOutFile(isoOutPath);
    long long currentAddress = 0;
    //if (!isoInFile){
    if(!isoInFile.open(QIODevice::ReadOnly)){
        qDebug() << "Could not open input iso";
        MainWindow.messageError("Could not open input file " + isoInPath);
        return;
    }
    if(!isoOutFile.open(QIODevice::WriteOnly)){
        qDebug() << "Could not open output iso";
        MainWindow.messageError("Could not open input file " + isoOutPath);
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
    qDebug() << "leftovers: " << leftoverBuffer;
    for(unsigned long long i = 0; i < (isoFileSize-bufferSize); i += bufferSize){
        isoInFile.seek(i);
        isoSearchBuffer = isoInFile.read(bufferSize);
        location = matcher.indexIn(isoSearchBuffer, 0);
        if (location != -1){
            //qDebug() << "Sequence found at: " << location << " with leftover: " << location%bufferSize;
            isoInFile.seek(i);
            isoSearchBuffer = isoInFile.read(location);
            isoOutFile.write(isoSearchBuffer);
            isoOutFile.write(filebuffer);
            //isoOutFile.write(makeNullArray(leftoverBuffer));
            location += i + filebuffer.size();
            break;
            //i += isoSearchBuffer.size() + filebuffer.size();
        }
        else{
            isoOutFile.write(isoSearchBuffer);
        }
    }
    qDebug() << "location: " << location;
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
    MainWindow.messageSuccess("ISO Saved.");
    qDebug() << "Done.";
}

qint64 ProgWindow::byteWrite(QFile& file, int8_t var) {
  qint64 toWrite = sizeof(decltype (var));
  qint64  written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug () << "write error";
  }
   //qDebug () << "out: " << written;
  return written;
}

qint64 ProgWindow::shortWrite( QFile& file, int16_t var ) {
  qint64 toWrite = sizeof(decltype (var));
  qint64 written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug () << "write error";
  }
   //qDebug () << "out: " << written;
  return written;
}

qint64 ProgWindow::intWrite( QFile& file, int32_t var ) {
  qint64 toWrite = sizeof(decltype (var));
  qint64  written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug () << "write error";
  }
   //qDebug () << "out: " << written;
  return written;
}

qint64 ProgWindow::longWrite( QFile& file, int64_t var ) {
  qint64 toWrite = sizeof(decltype (var));
  qint64  written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug () << "write error";
  }
   //qDebug () << "out: " << written;
  return written;
}
