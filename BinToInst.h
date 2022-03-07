#ifndef BINTOINST_H
#define BINTOINST_H

#include <QString>

class ProgWindow;

class BinToInst{
    public:
        QString convToInstruction(QString input, ProgWindow& MainWindow);
    private:
        QString GGR(int regIndex);
        QString GFR(int regIndex);
        QString special_inst(int setlist[]);
        QString REGIMM_inst(int setlist[]);
        QString S_inst(int setlist[]);
        QString MF0_inst(int setlist[]);
        QString MT0_inst(int setlist[]);
        QString COP0_inst(int setlist[]);
        QString COP1_inst(int setlist[]);
        QString MMI_inst(int setlist[]);

};

#endif // BINTOINST_H
