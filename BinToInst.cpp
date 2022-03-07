#include "mainwindow.h"
#include "MIPSReader.h"
#include "BinChanger.h"

QString BinToInst::GGR(int regIndex){
    //Get General Register
    QString retReg = genRegList[regIndex];
    return retReg;
}

QString BinToInst::GFR(int regIndex){
    //Get Float Register
    QString retReg = floatRegList[regIndex];
    return retReg;
}

inline QString BinToInst::special_inst(int setlist[]) {
    switch(setlist[5]){
        case 0: if(setlist[1] != 0 or setlist[2] != 0 or setlist[3] != 0){
            return "SLL " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", 0x" + QString::number(setlist[4], 16);
        }
        else return "NOP";
        case 2: return "SRL " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[4]);
        case 3: return "SRA " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[4]);
        case 4: return "SLLV " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
        case 6: return "SRLV " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
        case 7: return "SRAV " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
        case 8: return "JR " + GGR(setlist[1]);
        case 9: return "JALR " + GGR(setlist[1]);
        case 10: return "MOVZ " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 11: return "MOVN " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 12: return "SYSCALL";
        case 13: return "BREAK";
        case 15: return "SYNC " + GGR(setlist[4]);
        case 16: return "MFHI " + GGR(setlist[3]);
        case 17: return "MTHI " + GGR(setlist[1]);
        case 18: return "MFLO " + GGR(setlist[3]);
        case 19: return "MTLO " + GGR(setlist[1]);
        case 20: return "DSLLV " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
        case 22: return "DSRLV " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
        case 23: return "DSRAV " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
        case 24: return "MULT " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 25: return "MULTU " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 26: return "DIV " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 27: return "DIVU " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 32: return "ADD " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 33: return "ADDU " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 34: return "SUB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 35: return "SUBU " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 36: return "AND " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 37: return "OR " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 38: return "XOR " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 39: return "NOR " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 40: return "MFSA " + GGR(setlist[3]);
        case 41: return "MTSA " + GGR(setlist[1]);
        case 42: return "SLT " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 43: return "SLTU " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 44: return "DADD " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 45: return "DADDU " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 46: return "DSUB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 47: return "DSUBU " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 48: return "TGE " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 49: return "TGEU " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 50: return "TLT " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 51: return "TLTU " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 52: return "TEQ " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 54: return "TNE " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 56: return "DSLL " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[4]);
        case 58: return "DSRL " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[4]);
        case 59: return "DSRA " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[4]);
        case 60: return "DSLL32 " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[4]);
        case 62: return "DSRL32 " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[4]);
        case 63: return "DSRA32 " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[4]);
    }
    return "SPECIAL INSTRUCTION";
}

inline QString BinToInst::REGIMM_inst(int setlist[]) {
    switch (setlist[2]) {
    case 0: return "BLTZ " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 1: return "BGEZ " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 2: return "BLTZL " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 3: return "BGEZL " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 8: return "TGEI " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 9: return "TGEIU " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 10: return "TLTI " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 11: return "TLTIU " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 12: return "TEQI " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 14: return "TNEI " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 16: return "BLTZAL " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 17: return "BGEZAL " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 18: return "BLTZALL " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 19: return "BGEZALL " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 24: return "MTSAB " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    case 25: return "MTSAH " + GGR(setlist[1]) + ", " + QString::number(setlist[6]);
    }
    return "REGIMM INSTRUCTION";
}

inline QString BC0_inst(int setlist[]) {
    switch (setlist[2]) {
    case 0: return "BC0F " + QString::number(setlist[6]);
    case 1: return "BC0T " + QString::number(setlist[6]);
    case 2: return "BC0FL " + QString::number(setlist[6]);
    case 3: return "BC0TL " + QString::number(setlist[6]);
    }
    return "COP0 - BC0 INSTRUCTION";
}

inline QString C0_inst(int setlist[]) {
    switch (setlist[5]) {
    case 1: return "TLBR";
    case 2: return "TLBWI";
    case 6: return "TLBWR";
    case 8: return "TLBP";
    case 24: return "ERET";
    case 56: return "EI";
    case 57: return "DI";
    }
    return "COP0 - C0 INSTRUCTION";
}

inline QString BC1_inst(int setlist[]) {
    switch (setlist[2]) {
    case 0: return "BC1F " + QString::number(setlist[6]);
    case 1: return "BC1T " + QString::number(setlist[6]);
    case 2: return "BC1FL " + QString::number(setlist[6]);
    case 3: return "BC1TL " + QString::number(setlist[6]);
    }
    return "COP1 - BC1 INSTRUCTION";
}

inline QString BinToInst::S_inst(int setlist[]) {
    switch (setlist[5]) {
    case 0: return "ADD " + GFR(setlist[4]) + ", " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 1: return "SUB " + GFR(setlist[4]) + ", " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 2: return "MUL " + GFR(setlist[4]) + ", " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 3: return "DIV " + GFR(setlist[4]) + ", " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 4: return "SQRT " + GFR(setlist[4]) + ", " + GFR(setlist[2]);
    case 5: return "ABS " + GFR(setlist[4]) + ", " + GFR(setlist[3]);
    case 6: return "MOV " + GFR(setlist[4]) + ", " + GFR(setlist[3]);
    case 7: return "NEG " + GFR(setlist[4]) + ", " + GFR(setlist[3]);
    case 22: return "RSQRT " + GFR(setlist[4]) + ", " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 24: return "ADDA " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 25: return "SUBA " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 26: return "MULA " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 28: return "MADD " + GFR(setlist[4]) + ", " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 29: return "MSUB " + GFR(setlist[4]) + ", " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 30: return "MADDA " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 31: return "MSUBA " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 36: return "CVTW " + GFR(setlist[4]) + ", " + GFR(setlist[3]);
    case 40: return "MAX " + GFR(setlist[4]) + ", " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 41: return "MIN " + GFR(setlist[4]) + ", " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 48: return "C.F " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 50: return "C.EQ " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 52: return "C.LT " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    case 54: return "C.LE " + GFR(setlist[3]) + ", " + GFR(setlist[2]);
    }
    return "COP1 - S INSTRUCTION";
}

inline QString BinToInst::MF0_inst(int setlist[]) {
    switch (setlist[3]) {
        case 24:
        switch(setlist[8]){
            case 0: return "MFBPC " + GGR(setlist[2]);
            case 4: return "MFDAB " + GGR(setlist[2]);
            case 5: return "MFDABM " + GGR(setlist[2]);
            case 6: return "MFDVB " + GGR(setlist[2]);
            case 7: return "MFDVBM " + GGR(setlist[2]);
            case 2: return "MFIAB " + GGR(setlist[2]);
            case 3: return "MFIABM " + GGR(setlist[2]);
            }
        case 25:
        switch(setlist[5]){
            case 0: return "MFPS " + GGR(setlist[2]);
            default: return "MFPC " + GGR(setlist[2]);
            }
        default: return "MFC0 " + GGR(setlist[2]) + ", " + GGR(setlist[3]);
    }
    return "COP0 MF0 INSTRUCTION";
}

inline QString BinToInst::MT0_inst(int setlist[]) {
    switch (setlist[3]) {
    case 24: switch(setlist[8]){
        case 0: return "MTBPC " + GGR(setlist[2]);
        case 4: return "MTDAB " + GGR(setlist[2]);
        case 5: return "MTDABM " + GGR(setlist[2]);
        case 6: return "MTDVB " + GGR(setlist[2]);
        case 7: return "MTDVBM " + GGR(setlist[2]);
        case 2: return "MTIAB " + GGR(setlist[2]);
        case 3: return "MTIABM " + GGR(setlist[2]);
        }
    case 25: switch(setlist[5]){
        case 0: return "MTPS " + GGR(setlist[2]);
        default: return "MTPC " + GGR(setlist[2]);
        }
    default: return "MTC0 " + GGR(setlist[2]) + ", " + GGR(setlist[3]);
    }
    return "COP0 MT0 INSTRUCTION";
}

inline QString BinToInst::COP0_inst(int setlist[]) {
    switch (setlist[1]) {
    case 0: return MF0_inst(setlist);
    case 4: return MT0_inst(setlist);
    case 8: return BC0_inst(setlist);
    case 16: return C0_inst(setlist);
    }
    return "COP0 INSTRUCTION";
}

inline QString BinToInst::COP1_inst(int setlist[]) {
    switch (setlist[1]) {
    case 0: return "MFC1 " + GGR(setlist[2]) + ", " + GFR(setlist[3]);
    case 2: return "CFC1 " + GGR(setlist[2]) + ", " + GFR(setlist[3]);
    case 4: return "MTC1 " + GGR(setlist[2]) + ", " + GFR(setlist[3]);
    case 6: return "CTC1 " + GGR(setlist[2]) + ", " + GFR(setlist[3]);
    case 8: return BC1_inst(setlist);
    case 16: return S_inst(setlist);
    case 20: return "CVTS " + GFR(setlist[4]) + ", " + GFR(setlist[3]);
    }
    return "COP1 INSTRUCTION";
}

inline QString COP2_inst(int setlist[]) {
    //call separate file, VU manual
    return "VU INSTRUCTION";
}

inline QString BinToInst::MMI_inst(int setlist[]) {
    switch (setlist[5]) {
        case 8: //"MMI0"
            switch(setlist[4]){
                case 0: return "PADDW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 1: return "PSUBW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 2: return "PCGTW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 3: return "PMAXW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 4: return "PADDH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 5: return "PSUBH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 6: return "PCGTH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 7: return "PMAXH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 8: return "PADDB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 9: return "PSUBB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 10: return "PCGTB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 16: return "PADDSW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 17: return "PSUBSW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 18: return "PEXTLW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 19: return "PPACW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 20: return "PADDSH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 21: return "PSUBSH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 22: return "PEXTLH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 23: return "PPACH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 24: return "PADDSB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 25: return "PSUBSB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 26: return "PEXTLB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 27: return "PPACB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 30: return "PEXT5 " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
                case 31: return "PPAC5 " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
            }
        case 9: //"MMI2"
            switch (setlist[4]) {
                case 0: return "PMADDW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 2: return "PSLLVW " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
                case 3: return "PSRLVW " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
                case 4: return "PMSUBW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 8: return "PMFHI";
                case 9: return "PMFLO";
                case 10: return "PINTH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 12: return "PMULTW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 13: return "PDIVW " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 14: return "PCPYLD " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 16: return "PMADDH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 17: return "PHMADH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 18: return "PAND " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 19: return "PXOR " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 20: return "PMSUBH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 21: return "PHMSBH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 26: return "PEXEH " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
                case 27: return "PREVH " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
                case 28: return "PMULTH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 29: return "PDIVBW " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 30: return "PEXEW " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
                case 31: return "PROT3W " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
            }
        case 40: // "MMI1"
            switch (setlist[4]) {
                case 1: return "PABSW " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
                case 2: return "PCEQW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 3: return "PMINW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 4: return "PADSBH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 5: return "PABSH " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
                case 6: return "PCEQH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 7: return "PMINH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 10: return "PCEQB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 16: return "PADDUW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 17: return "PSUBUW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 18: return "PEXTUW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 20: return "PADDUH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 21: return "PSUBUH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 22: return "PEXTUH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 24: return "PADDUB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 25: return "PSUBUB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 26: return "PEXTUB " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 27: return "QFSRV " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
            }
        case 41: // "MMI3"
            switch(setlist[4]){
                case 0: return "PMADDUW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 3: return "PSRAVW " + GGR(setlist[3]) + ", " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
                case 8: return "PMTHI";
                case 9: return "PMTLO";
                case 10: return "PINTEH " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 12: return "PMULTUW " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 13: return "PDIVUW " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 14: return "PCPYUD " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 18: return "POR " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 19: return "PNOR " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
                case 26: return "PEXCH " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
                case 27: return "PCPYH " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
                case 30: return "PEXCW " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
            }
        case 0: return "MADD " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 1: return "MADDU " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 5: return "PLZCW " + GGR(setlist[2]) + ", " + GGR(setlist[1]);
        case 16: return "MFHI1";
        case 17: return "MTHI1";
        case 18: return "MFLO1";
        case 19: return "MTLO1";
        case 24: return "MULT1 " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 25: return "MULTU1 " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 26: return "DIV1 " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 27: return "DIVU1 " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 32: return "MADD1 " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 33: return "MADDU1 " + GGR(setlist[3]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[2]);
        case 48: return "PMFHL";
        case 49: return "PMTHL";
        case 52: return "PSLLH " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[3]);
        case 54: return "PSRLH " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[3]);
        case 55: return "PSRAH " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[3]);
        case 60: return "PSLLW " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[3]);
        case 62: return "PSRLW " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[3]);
        case 63: return "PSRAW " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + GGR(setlist[3]);
    }
    return "MMI INSTRUCTION";
}

QString BinToInst::convToInstruction(QString input, ProgWindow& MainWindow) {
    //refer to page 370 of the EE core instruction manual
    input = MainWindow.binChanger->reverse_input(input, 8);
    QString set1 = input.mid(0,6);
    QString set2 = input.mid(6,5);
    QString set3 = input.mid(11,5);
    QString set4 = input.mid(16,5);
    QString set5 = input.mid(21,5);
    QString set6 = input.mid(26,6);
    QString setm = input.mid(16,16);
    QString setj = input.mid(6, 26);
    QString setc = input.mid(21, 11);
    int set1i = set1.toInt(nullptr, 2);
    int set2i = set2.toInt(nullptr, 2);
    int set3i = set3.toInt(nullptr, 2);
    int set4i = set4.toInt(nullptr, 2);
    int set5i = set5.toInt(nullptr, 2);
    int set6i = set6.toInt(nullptr, 2);
    int setmi = setm.toInt(nullptr, 2);
    int setji = setj.toInt(nullptr, 2) << 2;
    int setci = setc.toInt(nullptr, 2);
    int setlist[] = {set1i, set2i, set3i, set4i, set5i, set6i, setmi, setji, setci};
    switch (set1i) {
    case 0: return special_inst(setlist);
    case 1: return REGIMM_inst(setlist);
    case 16: return COP0_inst(setlist);
    case 17: return COP1_inst(setlist);
    case 18: return COP2_inst(setlist);
    case 28: return MMI_inst(setlist);
    case 2: return "J " + QString::number(setlist[7], 16);
    case 3: return "JAL " + QString::number(setlist[7], 16);
    case 4: return "BEQ " + GGR(setlist[1]) + ", " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6] << 2, 16);
    case 5: return "BNE " + GGR(setlist[1]) + ", " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6] << 2, 16);
    case 6: return "BLEZ " + GGR(setlist[1]) + ", " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6] << 2, 16);
    case 7: return "BGTZ " + GGR(setlist[1]) + ", " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6] << 2, 16);
    case 8: return "ADDI " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16);
    case 9: return "ADDIU " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16);
    case 10: return "SLTI " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16);
    case 11: return "SLTIU " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16);
    case 12: return "ANDI " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16);
    case 13: return "ORI " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16);
    case 14: return "XORI " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16);
    case 15: return "LUI " + GGR(setlist[1]) + ", " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16);
    case 20: return "BEQL " + GGR(setlist[1]) + ", " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6] << 2, 16);
    case 21: return "BNEL " + GGR(setlist[1]) + ", " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6] << 2, 16);
    case 22: return "BLEZL " + GGR(setlist[1]) + ", " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6] << 2, 16);
    case 23: return "BGTZL " + GGR(setlist[1]) + ", " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6] << 2, 16);
    case 24: return "DADDI " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16);
    case 25: return "DADDIU " + GGR(setlist[2]) + ", " + GGR(setlist[1]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16);
    case 26: return "LDL " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 27: return "LDR " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 30: return "LQ " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 31: return "SQ " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 32: return "LB " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 33: return "LH " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 34: return "LWL " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 35: return "LW " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 36: return "LBU " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 37: return "LHU " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 38: return "LWR " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 39: return "LWU " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 40: return "SB " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 41: return "SH " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 42: return "SWL " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 43: return "SW " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 44: return "SDL " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 45: return "SDR " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 46: return "SWR " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 47: return "CACHE";
    case 49: return "LWC1 " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 51: return "PREF " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 54: return "LQC2";
    case 55: return "LD " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 57: return "SWC1 " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    case 62: return "SQC2";
    case 63: return "SD " + GGR(setlist[2]) + ", " + MainWindow.binChanger->twosCompConv(setlist[6], 16) + "(" + GGR(setlist[1]) +")";
    default: return "UNKNOWN INSTRUCTION ";
    }
}
