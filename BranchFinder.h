#ifndef BRANCHFINDER_H
#define BRANCHFINDER_H

#include <QString>

class ProgWindow;

class BranchFinder{
public:
    void findBranch(long addressChanged, int linesAdded, ProgWindow& MainWindow);
    void findJJAL(long addressChanged, int linesAdded, ProgWindow& MainWindow);
    void addressChanger(long addressChanged, int linesAdded, ProgWindow& MainWindow);

private:
    bool checkBranch(QString input);
    bool checkJump(QString input);
    bool checkLUI(QString input);
    bool checkORI(QString input);
    bool checkReturn(QString input);
    void adjustLUI(long addressChanged, int linesAdded, QString revRead, long currentLine, ProgWindow& MainWindow);
    int findNextJJR(long addressChanged, ProgWindow& MainWindow);
};

#endif // BRANCHFINDER_H
