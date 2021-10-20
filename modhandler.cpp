#include "mainwindow.h"

//mod files are arranged as follows:
//"modSections", 4 bytes: int, how many changed sections are in the file
//"startAddress", 8 bytes: long, starting address of the changed section
//"modLines", 4 bytes: int, how many lines ("words") are changed
//code, 4 bytes, repeating modLines times: "word", modded code to replace original

bool MainWindow::checkCompatibility(QStringList modFiles){
    int currentRead = 0;
    long startAddress = 0;
    long endAddress = 0;
    int modLines = 0;
    int modSections = 0;
    QByteArray modBuffer;
    std::vector<std::vector<std::vector<long>>> modRanges;
    modRanges.resize(modFiles.size());
    qDebug() << "mod files: " << modFiles.size();
    for (int i = 0; i < modFiles.size(); i++){
        //"But Trevor, this section is basically the same as loadModList, why not just do this there?"
        //there's probably a way to optimize it so that works but for now I'm going with this
        currentRead = 0;
        QFile currentMod(modFiles.at(i));
        if (currentMod.open(QIODevice::ReadOnly)){
            modBuffer = currentMod.readAll();
            modSections = reverse_input(modBuffer.mid(0, 4).toHex(), 2).toInt(nullptr, 16);
            currentRead += 4;
            qDebug() << "mod sections for " << i <<": " << modSections;
            modRanges[i].resize(modSections);
            for (int j = 0; j < modSections; j++){
                startAddress = reverse_input(modBuffer.mid(currentRead,8).toHex(), 2).toLong(nullptr, 16);
                currentRead += 8;
                modLines = reverse_input(modBuffer.mid(currentRead, 4).toHex(), 2).toInt(nullptr, 16);
                currentRead += 4*(modLines+1); //+1 to include itself
                endAddress = startAddress + (modLines*4);
                modRanges[i][j].push_back(startAddress);
                modRanges[i][j].push_back(endAddress);
            }
        }
    }
    //this looks so ugly but feels like the right approach
    //list is populated, now to iterate through vector for comparisons
    for(int i = 0; i < modFiles.size(); i++){
        //each modfile needs to compare to every other modfile, except itself and those before it
        //(since the files before it have already checked against the current file)
        //no, they should still be checked against previous files because of how the check works.
        for(int x = 0; x < modFiles.size(); x++){
            if(x == i){
                continue;
            }
            //each section needs to compare against every section in the other file
            //this checks to see if the starting and ending address of each range
            //are between the starting and ending address of the comparing range
            for(int j = 0; j < int(modRanges[i].size()); j++){
                for (int y = 0; y < int(modRanges[x].size()); y++){
                    //qDebug() << "Checking if " << modRanges[i][j][0] << " is greater than " << modRanges[x][y][0];
                    if(modRanges[i][j][0] >= modRanges[x][y][0] and modRanges[i][j][0] <= modRanges[x][y][1]){
                        //qDebug() << "File " << modFiles.at(i) << " (section " << j << ") is incompatible with file " << modFiles.at(x) << " (section " << y << ")";
                        return false;
                    }
                    else if (modRanges[i][j][1] >= modRanges[x][y][0] and modRanges[i][j][1] <= modRanges[x][y][1]){
                        //qDebug() << "File " << modFiles.at(i) << " (section " << j << ") is incompatible with file " << modFiles.at(x) << " (section " << y << ")";
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

void MainWindow::loadModList(){
    QStringList modFiles;
    QByteArray modBuffer;
    int modSections = 0;
    long startAddress = 0;
    int modLines = 0;
    int currentRead = 0;
    modFiles = QFileDialog::getOpenFileNames(this, tr("Select mod(s)"), QDir::currentPath() + "/mods/", tr("Mod Files (*.bin)"));
    //call compatibility check here
    if (checkCompatibility(modFiles)){
        for (int i = 0; i < modFiles.size(); i++){
            //qDebug() << "We get to this point. i = " << i;
            QFile currentMod = modFiles.at(i);
            currentRead = 0;
            if (currentMod.open(QIODevice::ReadOnly)){
                modBuffer = currentMod.readAll();
                modSections = reverse_input(modBuffer.mid(0, 4).toHex(), 2).toInt(nullptr, 16);
                currentRead += 4;
                //qDebug() << "mod sections " << modSections;
                for (int j = 0; j < modSections; j++){
                    startAddress = reverse_input(modBuffer.mid(currentRead,8).toHex(), 2).toLong(nullptr, 16);
                    //qDebug() << "start address " << startAddress;
                    currentRead += 8;
                    modLines = reverse_input(modBuffer.mid(currentRead, 4).toHex(), 2).toInt(nullptr, 16);
                    currentRead += 4;
                    for (int k = 0; k < modLines; k++){
                        filebuffer.replace(startAddress + (k*4), 4, modBuffer.mid(currentRead, 4)); //replace word in fileData with new modded word
                        currentRead += 4; //move to next word
                    }
                }
            }
        }
    }
    else {
        qDebug() << "Compatibility check failed";
    }
    updateFileBuffer();
}

//for making new mod files:
//select an address range or list of ranges within the code
//function then takes those ranges and encodes them as described above

void MainWindow::makeModList(){
    //using a table of variable length that has the start address and end address for each modded section
    QString startAddress;
    QString endAddress;
    long numStartAddress = 0;
    long numEndAddress = 0;
    int changedLines = 0;
    QString modFilePath = QFileDialog::getSaveFileName(this, tr("Create mod"), QDir::currentPath() + "/mods/", tr("Mod File (*.bin)"));
    QFile modFileOut(modFilePath);
    if(modFileOut.open(QIODevice::ReadWrite)){
        qDebug() << "row count " << TableMods->rowCount()-1;
        intWrite(modFileOut, TableMods->rowCount()-1); //write number of mod sections. 4 bytes
        for (int i = 0; i < TableMods->rowCount()-1; i++){
            qDebug() << "at row " << i;
            startAddress = TableMods->item(i, 0)->text();
            endAddress = TableMods->item(i, 1)->text();
            numStartAddress = startAddress.toInt(nullptr, 16) - addressOffset;
            numEndAddress = endAddress.toInt(nullptr, 16) - addressOffset;
            changedLines = (numEndAddress - numStartAddress)/4;
            longWrite(modFileOut, numStartAddress); //write start address. 8 bytes
            intWrite(modFileOut, changedLines); //write how many lines are changed. 4 bytes
            for(int j = numStartAddress; j < numEndAddress; j+=4){
                modFileOut.write(filebuffer.mid(j, 4)); //write code to file exactly as it is in the buffer. 4 bytes.
                //no need to alter the order if it's just gonna get written to the buffer in this order later anyway.
            }
        }
    }

}
