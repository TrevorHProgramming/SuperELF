#include "mainwindow.h"

MIPSReader::MIPSReader(ProgWindow *passedParent){
    parent = passedParent;
    MipsBuffer = " ";
    filebuffer = QByteArray(0);
}

void MIPSReader::updateFileBuffer(){
    //Pull data from the full file buffer and translate it
    //much more efficient than translating the entire file and holding that in RAM
    int BufferEnd = bufferStart + 256;
    int BufferNow = bufferStart;
    WindowBuffer = "";
    MipsBuffer = "";
    WindowBuffer.append(filebuffer.mid(bufferStart, BufferEnd));
    linelist->clear();
    addresslist->clear();
    QString temp = "";
    QString realInst;
    QString realHex;
    QByteArray tempBuffer;
    int j = 0;
    if (parent->radioInst->isChecked()){
        for (int i = 0; i < 256; i+=4){
            addresslist[j] = QString::number(BufferNow + addressOffset, 16).rightJustified(8, '0');
            temp = " | ";
            //actual code will translate here and append to line
            realInst = parent->binChanger.hex_to_bin(WindowBuffer.mid(i,4));
            temp += binToInst->convToInstruction(realInst, *parent);

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
            linelist[j] = temp;
            BufferNow += 4;
            j += 1;
        }
    }
    updateWindowBuffer();
}

void MIPSReader::updateWindowBuffer(){
    QStringList itemList;
    parent->MipsList->clear();
    MipsBuffer = "";
    for (int i = 0; i < 64; ++i){
        MipsBuffer += addresslist[i];
        MipsBuffer += linelist[i];
        itemList.push_back(addresslist[i]+linelist[i]);
    }
    parent->MipsList->insertItems(0,itemList);
    parent->MipsList->setItemAlignment(Qt::AlignVCenter);
}

void MIPSReader::scrollMIPS(int amount){
    bufferStart += amount;
    updateFileBuffer();
}

void MIPSReader::jumpAddress(){
    QString entry = parent->AddressBox -> text();
    if (entry.length() != 0){
        QString jumpaddress = entry;
        long long numjumpaddress = jumpaddress.toLongLong(nullptr, 16) - addressOffset;
        bufferStart = numjumpaddress;
        updateFileBuffer();
    }
    else {
        qDebug() << "Not a valid address.";
        parent->messageError("Not a valid address.");
    }
}

void MIPSReader::loadFile(){
    addressOffset = parent->setW->settingsValues[2].toLong(nullptr, 16);
    bufferStart = parent->setW->settingsValues[3].toLong(nullptr, 16);
    qDebug() << Q_FUNC_INFO << "address offset:" << addressOffset << parent->setW->settingsValues[2] << "buffer start:" << bufferStart << parent->setW->settingsValues[3];
    QString entry = QFileDialog::getOpenFileName(parent, parent->tr("Load ELF"), QDir::currentPath(), parent->tr("All Files (*.*)"));
    //qDebug() << Q_FUNC_INFO << "entry value is: " << entry;
    if (entry.length() != 0){
        QFile filein(entry);
        if (!filein.open(QIODevice::ReadOnly)){
            parent->MessagePopup->setText("Could not open file " + fileInPath);
            parent->MessagePopup->setWindowTitle("File error!");
            parent->MessagePopup->open();
            return;
        }
        filebuffer = QByteArray(filein.readAll());
        parent->modList->clear();
        updateFileBuffer();
    }
}

void MIPSReader::saveFile(){
    if (filebuffer.isEmpty()) {
        parent->messageError("No file to save. Load an ELF first.");
        return;
    }
    QString entry = QFileDialog::getSaveFileName(parent, parent->tr("Save ELF"), QDir::currentPath(), parent->tr("Binary Files (*.bin)"));
    if (entry.length() != 0){
        fileOutPath = entry;
        QFile fileout(fileOutPath);
        if(!fileout.open(QIODevice::WriteOnly)) {
            parent->messageError("Could not open file " + fileOutPath);
            return;
        }
        fileout.write(filebuffer);
        parent->messageSuccess("ELF Saved.");
    }
}

void MIPSReader::handleInsert(){
    //step 1: check to see if anything has been typed in the entry line
    //step 2: check to see if something is selected in MipsWindow
    //step 3: insert new line after selected line into the buffer
    //step 4: update MipsWindow and clear entry
    QString entry = parent->InstructionBox->text();
    //QString selection = parent->MipsWindow->textCursor().QTextCursor::selectedText();
    if(parent->MipsList->selectedItems().size() == 0){
        parent->messageError("Please select a line to insert at.");
        return;
    }
    QString selection = parent->MipsList->currentItem()->text();
    int currentLine = parent->MipsList->currentRow();
    QByteArray convReturn;
    QByteArray revReturn;
    ModLine modLine;
    if (entry.length() != 0 and selection.length() != 0){
        QString selectaddress = selection.left(8);
        int numselectaddress = selectaddress.toInt(nullptr, 16) - addressOffset;

        //convert entry to instruction. then convert each part of the instruction into an int and put thos into bytearrays. return the bytearrays
        //ex 10000001 00000000 00000000 00000001 -> 129 0 0 1 -> loop to convert each to bytearray then combine them
        if (parent->radioInst->isChecked()){
            convReturn = instToBin->convFromInst(entry);
            filebuffer.insert(numselectaddress, convReturn);
            modLine = modLine.create(0, parent->modList->modLineList.size(), numselectaddress, convReturn);
            parent->modList->adjustAddresses(*parent->modList, modLine.address, modLine.lineType);
            parent->modList->modLineList.append(modLine);
            updateFileBuffer();
        }
        else {
            convReturn = QByteArray::fromHex(entry.toUtf8());
            for (int k = 0; k <4; ++k){
                revReturn += convReturn.mid(3-k, 1);
            }
            filebuffer.insert(numselectaddress, revReturn);
            modLine = modLine.create(0, parent->modList->modLineList.size(), numselectaddress, revReturn);
            parent->modList->adjustAddresses(*parent->modList, modLine.address, modLine.lineType);
            parent->modList->modLineList.append(modLine);
            updateFileBuffer();
        }
        parent->MipsList->setCurrentRow(currentLine);
    }
}

void MIPSReader::handleDelete(){
    //get address of line and remove from buffer
    if(parent->MipsList->selectedItems().size() == 0){
        parent->messageError("Please select a line to delete.");
        return;
    }
    QString selection = parent->MipsList->currentItem()->text();
    int currentLine = parent->MipsList->currentRow();
    //qDebug() << selection;
    QString selectaddress = selection.left(8);
    int numselectaddress = selectaddress.toInt(nullptr, 16) - addressOffset;


    ModLine modLine;
    modLine = modLine.create(1, parent->modList->modLineList.size(), numselectaddress, 0);
    parent->modList->adjustAddresses(*parent->modList, modLine.address, modLine.lineType);
    parent->modList->modLineList.append(modLine);

    //qDebug() << MainWindow.binChanger.reverse_input(MainWindow.binChanger.hex_to_bin(filebuffer.mid(numselectaddress, 4)), 8);
    filebuffer.replace(numselectaddress, 4, "");
    updateFileBuffer();
    parent->MipsList->setCurrentRow(currentLine);
}

void MIPSReader::handleReplace(){
    QString entry = parent->InstructionBox -> text();
    if(parent->MipsList->selectedItems().size() == 0){
        parent->messageError("Please select a line to replace.");
        return;
    }
    QString selection = parent->MipsList->currentItem()->text();
    int currentLine = parent->MipsList->currentRow();
    QByteArray convReturn;
    QByteArray revReturn;
    ModLine modLine;
    if (entry.length() != 0 and selection.length() != 0){
        QString selectaddress = selection.left(8);
        int numselectaddress = selectaddress.toInt(nullptr, 16) - addressOffset;
        //convert entry to instruction. then convert each part of the instruction into an int and put those into bytearrays. return the bytearrays
        //ex 10000001 00000000 00000000 00000001 -> 129 0 0 1 -> loop to convert each to bytearray then combine them
        if (parent->radioInst->isChecked()){
            convReturn = instToBin->convFromInst(entry);
            filebuffer.replace(numselectaddress, 4, convReturn);
            modLine = modLine.create(1, parent->modList->modLineList.size(), numselectaddress, 0);
            parent->modList->adjustAddresses(*parent->modList, modLine.address, modLine.lineType);
            parent->modList->modLineList.append(modLine);
            modLine = modLine.create(0, parent->modList->modLineList.size(), numselectaddress, convReturn);
            parent->modList->adjustAddresses(*parent->modList, modLine.address, modLine.lineType);
            parent->modList->modLineList.append(modLine);
        }
        else {
            convReturn = QByteArray::fromHex(entry.toUtf8());
            /*for (int k = 0; k <4; ++k){
                revReturn += convReturn.mid(3-k, 1);
            }*/ //removing the reversal to make copying from IDA easier
            filebuffer.replace(numselectaddress, 4, revReturn);
            modLine = modLine.create(1, parent->modList->modLineList.size(), numselectaddress, 0);
            parent->modList->adjustAddresses(*parent->modList, modLine.address, modLine.lineType);
            parent->modList->modLineList.append(modLine);
            modLine = modLine.create(0, parent->modList->modLineList.size(), numselectaddress, revReturn);
            parent->modList->adjustAddresses(*parent->modList, modLine.address, modLine.lineType);
            parent->modList->modLineList.append(modLine);
        }
        updateFileBuffer();
    }
    parent->MipsList->setCurrentRow(currentLine+1);
}

//move to BinChanger
QByteArray makeNullArray(int length){
    QByteArray partFiller;
    for (int i = 0; i < length; i++){
        partFiller.append(QByteArray::fromHex("00"));
    }
    return partFiller;
}

void MIPSReader::isoSearcher(){
    if (filebuffer.isEmpty()) {
        parent->messageError("No ELF data to write to an ISO. Load an ELF first.");
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
    QString isoInPath = QFileDialog::getOpenFileName(parent, parent->tr("Load Base ISO"), QDir::currentPath(), parent->tr("ISO Files (*.iso)"));
    QString isoOutPath = QFileDialog::getSaveFileName(parent, parent->tr("Save New ISO"), QDir::currentPath(), parent->tr("ISO Files (*.iso)"));

    QFile isoInFile(isoInPath);
    QFile isoOutFile(isoOutPath);
    long long currentAddress = 0;
    //if (!isoInFile){
    if(!isoInFile.open(QIODevice::ReadOnly)){
        qDebug() << "Could not open input iso";
        parent->messageError("Could not open input file " + isoInPath);
        return;
    }
    if(!isoOutFile.open(QIODevice::WriteOnly)){
        qDebug() << "Could not open output iso";
        parent->messageError("Could not open input file " + isoOutPath);
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
    parent->messageSuccess("ISO Saved.");
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
