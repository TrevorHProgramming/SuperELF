#include "mainwindow.h"
#include "BinChanger.h"

void ModdedData::adjustAddresses(ModdedData& modLines, long newAddress, bool changeType){
    for (int i = 0; i<modLines.modLineList.length(); i++){
        if (changeType){
            if (modLines.modLineList[i].address > newAddress) {
                modLines.modLineList[i].address -= 4;
            }
        } else {
            if (modLines.modLineList[i].address >= newAddress) {
                modLines.modLineList[i].address += 4;
            }
        }
    }
}

void ModdedData::clear(){
    modLineList.clear();
    modSections.clear();
}

QVector<ModLine> ModdedData::sortModAddresses(QVector<ModLine> modLineList){
    QVector<ModLine> sortedList = modLineList;
    std::sort(sortedList.begin(), sortedList.end());
    return sortedList;
}

QVector<ModSection> ModdedData::getModSections(QVector<ModLine> modLineList){
    QVector<ModSection> modSectionAddresses;
    int modSectionCount = 0;
    long lastAddress =0;
    int sectionLineCount = 1;

    for (int i = 0; i < modLineList.length(); i++) {
        if (modLineList[i].address > lastAddress+4) {
            sectionLineCount = 1;
            modSectionCount += 1;
            modSectionAddresses.resize(modSectionCount);
            modSectionAddresses[modSectionCount-1].startAddress = modLineList[i].address;
            modSectionAddresses[modSectionCount-1].sectionLines = sectionLineCount;
        } else {
            sectionLineCount += 1;
            modSectionAddresses[modSectionCount-1].sectionLines = sectionLineCount;
        }
        lastAddress = modLineList[i].address;
    }

    return modSectionAddresses;
}

ModLine ModLine::create(bool lineType, long modIndex, long address, QByteArray code){
    ModLine retValue;
    retValue.lineType = lineType;
    retValue.modIndex = modIndex;
    retValue.address = address;
    retValue.code = code;
    qDebug() << "Mod line type: " << lineType << " index: " << modIndex << " address: " << address << " code " << code;
    return retValue;
}

//mod files are arranged as follows:
//"modType", 1 byte: bool, whether the mod is a type 0 (same length as original) or type 1 (changed length) mod
//"modSections", 4 bytes: int, how many changed sections are in the file
//"startAddress", 8 bytes: long, starting address of the changed section
//"modLines", 4 bytes: int, how many lines ("words") are changed
//code, 4 bytes, plus type, 1 byte, repeating modLines times: "word", modded code to replace or insert into the original

bool ModdedData::checkCompatibility(QStringList modFiles, ProgWindow& MainWindow){
    int currentRead = 0;
    long startAddress = 0;
    long endAddress = 0;
    int modLines = 0;
    int modSections = 0;
    bool modType;

    QByteArray modBuffer;
    std::vector<std::vector<std::vector<long>>> modRanges;
    modRanges.resize(modFiles.size());
    //qDebug() << "mod files: " << modFiles.size();
    for (int i = 0; i < modFiles.size(); i++){
        //"But Trevor, this section is basically the same as loadModList, why not just do this there?"
        //there's probably a way to optimize it so that works but for now I'm going with this
        currentRead = 0;
        QFile currentMod(modFiles.at(i));
        if (currentMod.open(QIODevice::ReadOnly)){
            modBuffer = currentMod.readAll();
            modType = MainWindow.binChanger.reverse_input(modBuffer.mid(0, 1).toHex(), 2).toInt(nullptr, 16);
            if (modType & (modFiles.size()>1)){
                MainWindow.messageError("File " + modFiles.at(i) + " is a Type 1 mod, which is incompatible with other mods.");
                return false;
            }
            modSections = MainWindow.binChanger.reverse_input(modBuffer.mid(1, 4).toHex(), 2).toInt(nullptr, 16);
            currentRead += 5;
            //qDebug() << "mod sections for " << i <<": " << modSections;
            modRanges[i].resize(modSections);
            for (int j = 0; j < modSections; j++){
                startAddress = MainWindow.binChanger.reverse_input(modBuffer.mid(currentRead,8).toHex(), 2).toLong(nullptr, 16);
                currentRead += 8;
                modLines = MainWindow.binChanger.reverse_input(modBuffer.mid(currentRead, 4).toHex(), 2).toInt(nullptr, 16);
                currentRead += (5*modLines)+4; //+4 to include itself, * 5 since each modline is 4 byte code + 1 byte type
                endAddress = startAddress + (modLines*4);
                qDebug() << "addresses for file " << i << " section " << j << " are: " << startAddress << " to " << endAddress;
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
                        MainWindow.messageError("File " + modFiles.at(i) + " (section " + QString::number(j) + ") is incompatible with file " + modFiles.at(x) + " (section " + QString::number(y) + ")");
                        //qDebug() << "File " << modFiles.at(i) << " (section " << j << ") is incompatible with file " << modFiles.at(x) << " (section " << y << ")";
                        return false;
                    }
                    else if (modRanges[i][j][1] >= modRanges[x][y][0] and modRanges[i][j][1] <= modRanges[x][y][1]){
                        MainWindow.messageError("File " + modFiles.at(i) + " (section " + QString::number(j) + ") is incompatible with file " + modFiles.at(x) + " (section " + QString::number(y) + ")");
                        //qDebug() << "File " << modFiles.at(i) << " (section " << j << ") is incompatible with file " << modFiles.at(x) << " (section " << y << ")";
                        return false;
                    }
                }
            }
        }
    }
    return true;
}

void ModdedData::loadModList(ProgWindow& MainWindow){
    if (MainWindow.mainReader->filebuffer.isEmpty()) {
        MainWindow.messageError("No file to modify. Load an ELF first.");
        return;
    }


    QStringList modFiles;
    QByteArray modBuffer;
    QByteArray modLine;
    int modSections = 0;
    long startAddress = 0;
    int modLines = 0;
    bool modType = 0;
    int currentRead = 0;
    int lineType = 0; //set to 0 by default for now. 0 for replaced, 1 for insterted, 2 for removed.
    int removeAdjustment = 0;
    modFiles = QFileDialog::getOpenFileNames(&MainWindow, MainWindow.tr("Select mod(s)"), QDir::currentPath() + "/mods/", MainWindow.tr("Mod Files (*.bin)"));
    //call compatibility check here
    if (checkCompatibility(modFiles, MainWindow)){
        for (int i = 0; i < modFiles.size(); i++){
            //qDebug() << "We get to this point. i = " << i;
            QFile currentMod = modFiles.at(i);
            currentRead = 1;
            if (currentMod.open(QIODevice::ReadOnly)){
                modBuffer = currentMod.readAll();
                //not reading the mod type since that's only relevant to the compatibility check
                modSections = MainWindow.binChanger.reverse_input(modBuffer.mid(1, 4).toHex(), 2).toInt(nullptr, 16);
                currentRead += 4;
                //qDebug() << "mod sections " << modSections;
                for (int j = 0; j < modSections; j++){
                    startAddress = MainWindow.binChanger.reverse_input(modBuffer.mid(currentRead,8).toHex(), 2).toLong(nullptr, 16);
                    currentRead += 8;
                    modLines = MainWindow.binChanger.reverse_input(modBuffer.mid(currentRead, 4).toHex(), 2).toInt(nullptr, 16);
                    currentRead += 4;
                    //qDebug() << "start address for section " << j << " is: " << startAddress << " with " << modLines << " modded line(s).";
                    for (int k = 0; k < modLines; k++){
                        modLine = modBuffer.mid(currentRead, 4);
                        currentRead += 4; //move to line type
                        lineType = modBuffer.mid(currentRead, 1).toHex().toInt(nullptr, 16);
                        currentRead += 1;
                        //qDebug() << "line " << k << " of secion " << j << " is line type " << lineType << " with code: " << modLine;
                        if (lineType == 1){ //insert
                            MainWindow.mainReader->filebuffer.insert(startAddress + (k*4) + removeAdjustment, modLine);
                        }
                        else if (lineType == 2){ //delete
                            removeAdjustment += 4; //doing this means that we have to go through the mods in order of address which is a whole other beast
                            //no it won't because that's accounted for in the mod file creation
                            MainWindow.mainReader->filebuffer.remove(startAddress + (k*4) + removeAdjustment, 4);
                        }
                        else { //replace
                            MainWindow.mainReader->filebuffer.replace(startAddress + (k*4) + removeAdjustment, 4, modLine);
                            //replace word in fileData with new modded word
                        }
                    }
                }
            }
            else{
                qDebug() << "Failed to open file " << modFiles.at(i);
            }
        }
    }
    else {
        qDebug() << "Compatibility check failed";
    }
    MainWindow.mainReader->updateFileBuffer();
}

//for making new mod files:
//select an address range or list of ranges within the code
//function then takes those ranges and encodes them as described above

void ModdedData::makeModList(ProgWindow& MainWindow){
    //TO-DO: re-write this to use a ModdedData::modList instead of the mod table, requiring less work from the user.

    //check if the overall length of the file has changed to determine type 0 or type 1. Done.

    //if type 0, determine the address ranges like the user would enter in the table, then make mods as normal
    //normal being copy the data from filebuffer in those ranges

    //if type 1, determine the modded sections the same way, but find how many lines were added in each section
    //as the mod file is written, x lines will have the "replace" flag where x is the number of original lines
    //and y lines will have the "insert" flag, were y is the number of added lines

    // me from a few hours later @ ^ and v: what about removed lines. what if I take a function out, huh? nice one.

    //this SHOULD result in the same number and placement of lines after the mod file is parsed.
    //the easy route is to just require that type 1 mods are saved as a new ELF since they aren't compatible with other mods anyway.


    QString modFilePath = QFileDialog::getSaveFileName(&MainWindow, MainWindow.tr("Create mod"), QDir::currentPath() + "/mods/", MainWindow.tr("Mod File (*.bin)"));
    QFile modFileOut(modFilePath);
    int addedLinesCount = 0;
    int removedLinesCount = 0;
    bool modType = 0;

    //if the total number of lines added is not the same as the number of lines removed, then the total length of the file has changed.
    //so we can use this to determine if a mod is Type 0 or Type 1



    //now we have all of the modified lines in the file that we need.
    if(modFileOut.open(QIODevice::ReadWrite)){
        for (int i = 0; i < MainWindow.modList->modLineList.length(); i++){
            if (MainWindow.modList->modLineList[i].lineType == 0){
                addedLinesCount += 1;
            }
            else {
                removedLinesCount += 1;
            }
        }
        if (addedLinesCount != removedLinesCount) {
            modType = 1;
        }
        qDebug() << "mod type: " << modType;

        //I feel like this sequence is inefficient but if it works it works, I guess
        //calling these two here just to call them again later is cringe tho
        MainWindow.modList->modLineList = MainWindow.modList->sortModAddresses(MainWindow.modList->modLineList);
        MainWindow.modList->modSections = MainWindow.modList->getModSections(MainWindow.modList->modLineList);

        //if this is a mod type 1, run branch and JJAL checks here
        if (modType) {
            for (int i = 0; i < MainWindow.modList->modSections.length(); i++) {
                //send the mod sections to the branch and JJAL checkers, which will modify the moddedLineList with their changes
                //once we have those changes, we'll need to re-sort the addresses and make a new list of mod sections
                //MainWindow.branchFinder->findBranch(MainWindow.modList->modSections[i].startAddress, MainWindow.modList->modSections[i].sectionLines, MainWindow);
                MainWindow.branchFinder->findJJAL(MainWindow.modList->modSections[i].startAddress, MainWindow.modList->modSections[i].sectionLines, MainWindow);
            }
            //pain
            MainWindow.modList->modLineList = MainWindow.modList->sortModAddresses(MainWindow.modList->modLineList);
            MainWindow.modList->modSections = MainWindow.modList->getModSections(MainWindow.modList->modLineList);
        }

        MainWindow.byteWrite(modFileOut, modType); //write the mod type. 1 byte.
        //qDebug() << "row count " << MainWindow.modList->modSections.length();
        MainWindow.intWrite(modFileOut, MainWindow.modList->modSections.length()); //write number of mod sections. 4 bytes
        for (int i = 0; i < MainWindow.modList->modSections.length(); i++){
            //qDebug() << "at row " << i;
            MainWindow.longWrite(modFileOut, MainWindow.modList->modSections[i].startAddress); //write start address. 8 bytes
            MainWindow.intWrite(modFileOut, MainWindow.modList->modSections[i].sectionLines); //write how many lines are changed. 4 bytes
            for(int j = MainWindow.modList->modSections[i].startAddress; j < MainWindow.modList->modSections[i].startAddress+(MainWindow.modList->modSections[i].sectionLines*4); j+=4){
                modFileOut.write(MainWindow.mainReader->filebuffer.mid(j, 4)); //write code to file exactly as it is in the buffer. 4 bytes.
                MainWindow.byteWrite(modFileOut, MainWindow.modList->modLineList[i].lineType);
            }
        }

    }

}
