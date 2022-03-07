#ifndef MODMANAGER_H
#define MODMANAGER_H

#include <QStringList>

class ProgWindow;

class ModLine{
public:
    bool operator< (const ModLine &other) const {
        return address < other.address; //shamelessly taken from stackoverflow
        //but I didn't even know you could redefine operators until this
    }
    bool lineType; //0 = added, 1 = deleted
    long modIndex; //counts the order where modded events happen
    long address; //has the address of a line.
    QByteArray code; //the content of the line. 0 if deleted.

    ModLine create(bool lineType, long modIndex, long address, QByteArray code);

};

class ModSection{
public:
    long startAddress;
    int sectionLines;
};

class ModdedData{
public:
    QVector<ModLine> modLineList;
    QVector<ModSection> modSections;
    void clear();
    void adjustAddresses(ModdedData& modLines, long newAddress, bool changeType);
private:
    bool checkCompatibility(QStringList modFiles, ProgWindow& MainWindow);
    QVector<ModLine> sortModAddresses(QVector<ModLine> modLineList);
    QVector<ModSection> getModSections(QVector<ModLine> modLineList);
public slots:
    void loadModList(ProgWindow& MainWindow);
    void makeModList(ProgWindow& MainWindow);
};

#endif // MODMANAGER_H
