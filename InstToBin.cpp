#include "mainwindow.h"

QBitArray intToBit(int v, int length)
{
    QBitArray ba(length);
    for (int i=0; i<length; i++){
        ba.setBit(i, v>>i & 1);
    }
    return ba;
}

QByteArray bitToByte(QBitArray bits) {
    QByteArray bytes;
    bytes.resize(bits.count()/8);
    bytes.fill(0);
    int i = 0;
    // Convert from QBitArray to QByteArray
    for(int b=bits.count()-1; b>=0; --b){
        bytes[i/8] = ( bytes.at(i/8) | ((bits[b]?1:0)<<(b%8)));
        ++i;}
    return bytes;
}

int InstToBin::fI(QString seek) //Find Instruction
{
    QString listOfInst[299] = {/*CPU Instructions 0-103*/

                               /*CPU REGIMM Insructions + ADDI*/
                               /*i21m	6 5 5 16    The Weird One*/ "ADDI",
                               /*i1m	6 5 5 16    REGIMM*/ "BGEZ", "BGEZAL", "BGEZALL", "BGEZL", "BLTZ", "BLTZAL", "BLTZALL", "BLTZL", "TEQI", "TGEI", "TGEIU", "TLTI", "TLTIU", "TNEI", "MTSAB", "MTSAH",
                               /*CPU Special Instructions*/
                               /*i21	26 special*/ "JALR",
                               /*i213	6 5 5 5 5 6 special*/ "DSLL", "DSLL32", "DSRA", "DSRA32", "DSRL", "DSRL32", "SLL", "SRA", "SRL",
                               /*i312	6 5 5 5 5 6 special*/ "ADD", "ADDU", "AND", "DADD", "DADDU", "DSUB", "DSUBU", "MOVN", "MOVZ", "NOR", "OR", "SLT", "SLTU", "SUB", "SUBU", "XOR",
                               /*i321	6 5 5 5 5 6 special*/ "DSLLV", "DSRAV", "DSRLV", "SLLV", "SRAV", "SRLV",
                               /*i12	6 5 5 10 6  special*/ "MULT", "MULTU", "TEQ", "TGE", "TGEU", "TLT", "TLTU", "TNE", "DIV", "DIVU",
                               /*i1     6 5 15 6    special*/ "JR", "MTHI", "MTLO",
                               /*i		6 20 6      special*/ "BREAK", "SYSCALL",
                               /*SYNC*/ "SYNC",
                               /*i1     6 15 5 6    special*/"MFHI", "MFLO",
                               /*CPU Standard Instructions*/
                               /*i12m	6 5 5 16    normal*/ "BEQ", "BEQL", "BNE", "BNEL",
                               /*i12m	6 5 5 16    normal, but different than the above*/"BGTZ", "BGTZL", "BLEZ", "BLEZL",
                               /*i12m	6 5 5 16    normal, but its own thing*/ "LUI",
                               /*i21m	6 5 5 16    normal*/ "ADDIU", "ANDI", "DADDI", "DADDIU", "ORI", "SLTI", "SLTIU", "XORI",
                               /*i2m1	6 5 5 16    normal*/ "LB", "LBU", "LD", "LDL", "LDR", "LH", "LHU", "LW", "LWL", "LWR", "LWU", "PREF", "SB", "SD", "SDL", "SDR", "SH", "SW", "SWL", "SWR", "LWC1", "SWC1",
                               /*im     6 26        normal*/ "J", "JAL",

                               /*EE CORE Instructions 106-216*/
                               /*i2m1	6 5 5 16 normal*/ "LQ", "SQ",
                               /*i312	6 5 5 5 5 6 special*/ "MULT", "MULTU",
                               /*i12	6 5 5 10 6 MMI*/ "DIV1", "DIVU1",
                               /*i213	6 5 5 5 5 6 MMI*/ "PSLLH", "PSLLW", "PSRAH", "PSRAW", "PSRLH", "PSRLW",
                               /*i1     6 5 15 6 MMI*/ "MTHI1", "MTLO1", "MTSA", "PMTHL.LW", "MFHI1", "MFLO1", "MFSA",
                               /*i1     6 5 15 6 MMI PMFHL*/ "PMFHL.LH", "PMFHL.LW", "PMFHL.SH", "PMFHL.SLW", "PMFHL.UW",
                               /*i21	6 5 5 5 5 6 MMI0*/ "PEXT5", "PPAC5",
                               /*i312	6 5 5 5 5 6 MMI0*/  "PADDB", "PADDH", "PADDSB", "PADDSH", "PADDSW", "PADDW", "PCGTB", "PCGTH", "PCGTW", "PEXTLB", "PEXTLH", "PEXTLW", "PMAXH", "PMAXW", "PPACB", "PPACH", "PPACW", "PSUBB", "PSUBH", "PSUBSB", "PSUBSH", "PSUBSW", "PSUBW",
                               /*i21	6 5 5 5 5 6 MMI1*/ "PABSH", "PABSW", "PLZCW",
                               /*i312	6 5 5 5 5 6 MMI1*/ "MADD", "MADD1", "MADDU", "MADDU1", "MULT1", "MULTU1", "PADDUB", "PADDUH", "PADDUW", "PADSBH", "PCEQB", "PCEQH", "PCEQW", "PEXTUB", "PEXTUH", "PEXTUW", "PMINH", "PMINW", "PSUBUB", "PSUBUH", "PSUBUW", "QFSRV",
                               /*i12	6 5 5 5 5 6 MMI2*/ "PDIVBW", "PDIVW",
                               /*i21	6 5 5 5 5 6 MMI2*/ "PEXEH", "PEXEW", "PREVH", "PROT3W",
                               /*i312	6 5 5 5 5 6 MMI2*/ "PAND", "PCPYLD", "PHMADH", "PHMSBH", "PINTH", "PMADDH", "PMADDW", "PMSUBH", "PMSUBW", "PMULTH", "PMULTW", "PXOR",
                               /*i321	6 5 5 5 5 6 MMI2*/ "PSLLVW", "PSRLVW",
                               /*i1     6 5 15 6 MMI2*/ "PMFHI", "PMFLO",
                               /*i12	6 5 5 5 5 6 MMI3*/ "PDIVUW",
                               /*i21	6 5 5 5 5 6 MMI3*/ "PCPYH", "PEXCH", "PEXCW",
                               /*i312	6 5 5 5 5 6 MMI3*/ "PCPYUD", "PINTEH", "PMADDUW", "PMULTUW", "PNOR", "POR",
                               /*i321	6 5 5 5 5 6 MMI3*/ "PSRAVW",
                               /*i1     6 5 15 6 MMI3*/ "PMTHI", "PMTLO",

                               /*COP1 Instructions 217-250*/
                               /*Normal instructions*/
                               /*i12	6 5 5 5 11*/ "CFC1", "CTC1", "MFC1", "MTC1",
                               /*S Instructions*/
                               /*i21	6 5 5 5 5 6*/ "ABS.S", "ADDA.S", "C.EQ.S", "C.F.S", "C.LE.S", "C.LT.S", "CVT.S.W", "CVT.W.S", "MADDA.S", "MOV.S", "MSUBA.S", "MULA.S", "NEG.S", "SQRT.S", "SUBA.S",
                               /*i321	6 5 5 5 5 6*/ "ADD.S", "DIV.S", "MADD.S", "MAX.S", "MIN.S", "MSUB.S", "MUL.S", "RSQRT.S", "SUB.S",
                               /*BC1 inst (Normal value of 8)*/
                               /*im     6 5 5 16*/ "BC1F", "BC1FL", "BC1T", "BC1TL",
                               /*COP0 Instructions 251-277*/
                               /*i12	6 5 5 5 5 5 1 MF0*/ "MFPC", "MFPS",
                               /*i12	6 5 5 5 11 MF0*/ "MFC0",
                               /*i1     6 5 5 5 11 MF0*/ "MFBPC", "MFDAB", "MFDABM", "MFDVB", "MFDVBM", "MFIAB", "MFIABM",
                               /*i12	6 5 5 5 5 5 1 MT0*/"MTPC", "MTPS",
                               /*i12	6 5 5 5 11 MT0*/ "MTC0",
                               /*il     6 5 5 5 11 MT0*/ "MTBPC", "MTDAB", "MTDABM", "MTDVB", "MTDVBM", "MTIAB", "MTIABM",
                               /*BC0 instructions*/
                               /*im     6 5 5 16*/ "BC0F", "BC0FL", "BC0T", "BC0TL",
                               /*i		6 5 15 6*/ "DI", "EI", "ERET",
                               /*gotta have NOP*/ "NOP"
        };


    for (int i = 0; i < 299; ++i)
    {
        if (listOfInst[i] == seek) return i;
    }
    return -1;
}

int InstToBin::fG(QString seek) //Find General Register
{
    QString listOfGenReg[32] = { "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
                                 "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
                                 "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
                                 "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"};
    for (int i = 0; i < 32; ++i)
    {
        if (listOfGenReg[i] == seek) return i;
    }
    return -1;
}

int InstToBin::fF(QString seek) //Find Float Register
{
    QString listOfFltReg[32] = { "f0", "f1", "f2", "f3", "f4", "f5", "f6", "f7",
                                 "f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",
                                 "f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
                                 "f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31"};
    for (int i = 0; i < 32; ++i)
    {
        if (listOfFltReg[i] == seek) return i;
    }
    return -1;
}

int InstToBin::fID(int seek){
    int listOfInstID[299] = {0,
                             1,17,19,3,0,16,18,2,12,8,9,10,11,14,24,25,
                             9,
                             56,60,59,63,58,62,0,3,2,
                             32,33,36,44,45,46,47,11,10,39,37,42,43,34,35,38,
                             20,23,22,4,7,6,
                             24,25,52,48,49,50,51,54,
                             26,27,
                             8,17,19,
                             14,12,
                             15,16,18,
                             4,20,5,21,
                             7,23,6,22,15,
                             9,12,24,25,13,10,11,14,
                             32,36,55,26,27,33,37,35,34,38,39,51,40,63,44,45,41,43,42,46,49,57,
                             2,3,

                             30,31,
                             24,25,
                             26,28,
                             52,60,55,63,54,62,
                             28,17,41,49,18,40,
                             3,0,4,2,1,
                             30,31,
                             8,4,24,20,16,0,10,6,2,26,22,18,7,3,27,23,19,9,5,25,21,17,1,
                             5,1,4,
                             0,32,1,33,24,25,24,20,16,4,18,6,2,26,22,18,7,3,25,21,17,27,
                             29,13,
                             26,30,27,31,
                             18,14,17,21,10,16,0,20,4,28,12,19,
                             2,3,
                             8,9,
                             13,
                             27,26,30,
                             14,10,0,12,19,18,
                             3,
                             8,9,

                             2,6,0,4,
                             5,24,50,48,54,52,32,36,30,6,31,26,7,4,1,
                             0,3,28,40,48,29,2,22,1,
                             0,2,1,3,

                             1,0,
                             0,
                             0,4,5,6,7,2,3,
                             1,0,
                             0,
                             0,4,5,6,7,2,3,
                             0,2,1,3,
                             57,56,24,
                             0
    };

    return listOfInstID[seek];
}

QByteArray InstToBin::convFromInst(QString instruction)
{
    QString findInst = instruction;
    int spacePos;
    int instLength;
    QString II[4]; //Instruction Items
    for (int i=0; i<4;++i){
        instLength = findInst.length();
        spacePos = findInst.indexOf(" ");
        II[i] = findInst.left(spacePos);
        findInst = findInst.mid(spacePos+1, instLength);
        //qDebug() << Q_FUNC_INFO << "modsplit: II[" << i <<"]:" << II[i];
    }


    long IR[7] = {0,0,0,0,0,0,0}; //Instruction Return, default is NOP
    int BL[7] = {0,0,0,0,0,0,0}; //Bit Length
    uint16_t x = fI(II[0].toUpper()); //Find instruction index
    uint16_t i0 = fID(x);
    uint16_t i1 = fG(II[1]);
    uint16_t i2 = fG(II[2]);
    uint16_t i3 = fG(II[3]);

    uint32_t j0 = II[1].toInt(nullptr, 16) >> 2;

    /*CPU section needs to be reorganized to match the now-divided CPU instructions*/
    if (x == 0){
        qDebug() << i0 << " ADDI	6 5 5 16 ";
        IR[0] = 0, IR[1] = i2, IR[2] = i1, IR[3] = i3;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 16;

    } else if (x >= 1 && x <= 16){
        qDebug() << i0 << " CPU REGIMM i1m	6 5 5 16 ";
        IR[0] = 1, IR[1] = i1, IR[2] = i0, IR[3] = i2;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 16;

    } else if (x ==17){
        qDebug() << i0 << " CPU SPEC i21	6 26 ";
        IR[0] = i0, IR[1] = i1;
        BL[0] = 6, BL[1] = 26;

    } else if (x >= 18 && x <=26){
        qDebug() << i0 << " CPU SPEC i213	6 5 5 5 5 6 ";
        //IR[0] = 0, IR[1] = 0, IR[2] = i2, IR[3] = i1, IR[4] = i3, IR[5] = i0;
        IR[0] = 0, IR[1] = 0, IR[2] = i2, IR[3] = i1, IR[4] = II[3].toInt(nullptr), IR[5] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 27 && x <=42){
        qDebug() << i0 << " CPU SPEC i312	6 5 5 5 5 6 ";
        IR[0] = 0, IR[1] = i2, IR[2] = i3, IR[3] = i1, IR[4] = 0, IR[5] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 43 && x <=48){
        qDebug() << i0 << " CPU SPEC i321	6 5 5 5 5 6 ";
        IR[0] = 0, IR[1] = i3, IR[2] = i2, IR[3] = i1, IR[4] = 0, IR[5] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 49 && x <=58){
        qDebug() << i0 << " CPU SPEC i12	6 5 5 10 6 ";
        IR[0] = 0, IR[1] = i1, IR[2] = i2, IR[3] = 0, IR[4] = 0, IR[5] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 10, BL[4] = 6;

    } else if (x >= 59 && x <=61){
        qDebug() << i0 << " CPU SPEC i1	6 5 15 6 ";
        IR[0] = 0, IR[1] = i1, IR[2] = 0, IR[3] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 15, BL[3] = 6;

    } else if (x >= 62 && x <=63){
        qDebug() << i0 << " CPU SPEC i	6 20 6 ";
        IR[0] = 0, IR[1] = i1, IR[2] = i0;
        BL[0] = 6, BL[1] = 20, BL[2] = 6;

    } else if (x == 64){
        qDebug() << i0 << " CPU SPEC SYNC ";
        IR[0] = 0, IR[1] = 0, IR[2] = II[1].toInt(nullptr), IR[3] = i0;
        BL[0] = 6, BL[1] = 15, BL[2] = 5, BL[3] = 6;

    } else if (x >= 65 && x <=66){
        qDebug() << i0 << " CPU SPEC i1	6 15 5 6 ";
        IR[0] = 0, IR[1] = 0, IR[2] = i0, IR[3] = i0;
        BL[0] = 6, BL[1] = 15, BL[2] = 5, BL[3] = 6;

    } else if (x >= 67 && x <=70){
        qDebug() << i0 << " CPU STD i12m	6 5 5 16 ";
        qDebug() << "Branch instruction";
        i3 = II[3].toInt(nullptr, 16) >> 2;
        IR[0] = i0, IR[1] = i1, IR[2] = i2, IR[3] = i3;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 16;

    } else if (x >= 71 && x <=74){
        qDebug() << i0 << " CPU STD i12m	6 5 5 16 ";
        qDebug() << "Branch instruction";
        i2 = II[2].toInt(nullptr, 16) >> 2;
        IR[0] = i0, IR[1] = i1, IR[2] = 0, IR[3] = i2;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 16;

    } else if (x ==75){
        qDebug() << i0 << " CPU STD i12m	6 5 5 16 ";
        qDebug() << "LUI";
        i2 = II[2].toInt(nullptr, 16);
        IR[0] = i0, IR[1] = 0, IR[2] = i1, IR[3] = i2;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 16;

    } else if (x >= 76 && x <=83){
        qDebug() << i0 << " CPU STD i21m	6 5 5 16 ";
        qDebug() << "\"IU\" commands";
        i3 = II[3].toInt(nullptr, 16);
        IR[0] = i0, IR[1] = i2, IR[2] = i1, IR[3] = i3;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 16;

    } else if (x >= 84 && x <=105){
        qDebug() << i0 << " CPU STD i2m1	6 5 5 16 ";
        qDebug() << "load instruction or store instruction";
        spacePos = II[2].indexOf("(");
        qDebug() << "before passes: " << II[2] << " and location: " << spacePos;
        II[3] = II[2].left(spacePos);
        II[2] = II[2].mid(spacePos+1, 2);
        qDebug() << "first pass II3: " << II[3] << " first pass II2: " << II[2];
        spacePos = II[2].indexOf(")");
        II[2] = II[2].left(spacePos);
        qDebug() << "offset: " << II[3] << " register: " << II[2];
        i3 = II[3].toInt(nullptr, 16);
        i2 = fG(II[2]);
        qDebug() << "offset converted: " << i3 << " register converted: " << i2;
        IR[0] = i0, IR[1] = i2, IR[2] = i1, IR[3] = i3;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 16;

    } else if (x >= 106 && x <=107){
        qDebug() << i0 << " CPU STD im	6 26 ";
        qDebug() << "IR0: " << i0 << "IR1: " << j0;
        IR[0] = i0, IR[1] = j0;
        BL[0] = 6, BL[1] = 26;

    } else if (x >= 108 && x <=109){
        qDebug() << i0 << " CORE i12ml	6 5 5 16 ";
        IR[0] = i0, IR[1] = i1, IR[2] = i3, IR[3] = i2;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 16;

    } else if (x >= 110 && x <=111){
        qDebug() << i0 << " CORE i312	6 5 5 5 5 6 ";
        IR[0] = 0, IR[1] = i2, IR[2] = i3, IR[3] = i1, IR[4] = 0, IR[5] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 112 && x <=113){
        qDebug() << i0 << " CORE i12	6 5 5 10 6 ";
        IR[0] = 28, IR[1] = i1, IR[2] = i2, IR[3] = 0, IR[4] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 10, BL[4] = 6;

    } else if (x >= 114 && x <=119){
        qDebug() << i0 << " CORE i213	6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = 0, IR[2] = i2, IR[3] = i1, IR[4] = i3, IR[5] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 120 && x <= 126){
        qDebug() << i0 << " CORE MMI i1	6 5 15 6 ";
        IR[0] = 28, IR[1] = i1, IR[2] = 0, IR[3] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 15, BL[3] = 6;

    } else if (x >= 127 && x <= 131){
        qDebug() << i0 << " CORE MMI PMFHL i1	6 5 15 6 ";
        IR[0] = 28, IR[1] = i1, IR[2] = i2, IR[3] = i1, IR[4] = i0;
        BL[0] = 6, BL[1] = 10, BL[2] = 5, BL[3] = 5, BL[4] = 6;

    } else if (x >= 132 && x <=133){
        qDebug() << i0 << " CORE i21	6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = 0, IR[2] = i2, IR[3] = i1, IR[4] = i0, IR[5] = 8;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 134 && x <=156){
        qDebug() << i0 << " CORE MMI 0 i312  6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = i2, IR[2] = i3, IR[3] = i1, IR[4] = i0, IR[5] = 8;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >=157 && x <=159){
        qDebug() << i0 << " CORE MMI1 i21	6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = 0, IR[2] = i2, IR[3] = i1, IR[4] = i0, IR[5] = 40;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 160 && x <=181){
        qDebug() << i0 << " CORE MMI1 i312	6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = i2, IR[2] = i3, IR[3] = i1, IR[4] = i0, IR[5] = 40;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 182 && x <=183){
        qDebug() << i0 << " CORE MMI2 i12	6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = i1, IR[2] = i2, IR[3] = 0, IR[4] = i0, IR[5] = 9;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 184 && x <=187){
        qDebug() << i0 << " CORE MMI2 i21	6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = 0, IR[2] = i2, IR[3] = i1, IR[4] = i0, IR[5] = 9;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 188 && x <=199){
        qDebug() << i0 << " CORE MMI2 i312	6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = i2, IR[2] = i3, IR[3] = i1, IR[4] = i0, IR[5] = 9;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 200 && x <=201){
        qDebug() << i0 << " CORE MMI2 i321	6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = i3, IR[2] = i2, IR[3] = i1, IR[4] = i0, IR[5] = 9;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 202 && x <=203){
        qDebug() << i0 << " CORE MMI2 i1	6 5 15 6 ";
        IR[0] = 28, IR[1] = 0, IR[2] = i1, IR[3] = i0, IR[4] = 9;
        BL[0] = 6, BL[1] = 10, BL[2] = 5, BL[3] = 5, BL[4] = 6;

    } else if (x == 204){
        qDebug() << i0 << " CORE MMI3 i12	6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = i1, IR[2] = i2, IR[3] = 0, IR[4] = i0, IR[5] = 41;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 205 && x <=207){
        qDebug() << i0 << " CORE MMI3 i21	6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = 0, IR[2] = i2, IR[3] = i1, IR[4] = i0, IR[5] = 41;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 208 && x <=213){
        qDebug() << i0 << " CORE i312	6 5 5 5 5 6 ";
        IR[0] = 28, IR[1] = i2, IR[2] = i3, IR[3] = i1, IR[4] = i0, IR[5] = 41;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x == 214){
        qDebug() << i0 << " CORE MMI3 i321	6 5 5 16 ";
        IR[0] = 28, IR[1] = i3, IR[2] = i2, IR[3] = i1, IR[4] = i0, IR[5] = 41;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 16;

    } else if (x >= 215 && x <=216){
        qDebug() << i0 << " CORE MMI3 il     6 5 15 6";
        IR[0] = 0, IR[1] = i1, IR[2] = i2, IR[3] = i3, IR[4] = 0, IR[5] = 32;
        BL[0] = 6, BL[1] = 5, BL[2] = 10, BL[4] = 5, BL[5] = 6;

    } else if (x >= 217 && x <=220){
        qDebug() << i0 << " COP1 i12     6 5 5 5 11";
        IR[0] = 17, IR[1] = i0, IR[2] = i1, IR[3] = i2, IR[4] = 0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 11;

    } else if (x >= 221 && x <=235){
        qDebug() << i0 << " COP1 S i21     6 5 5 5 5 6";
        IR[0] = 17, IR[1] = 16, IR[2] = 0, IR[3] = i2, IR[4] = i1, IR[5] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 236 && x <=244){
        qDebug() << i0 << " COP1 S i321     6 5 5 5 5 6";
        IR[0] = 17, IR[1] = 16, IR[2] = i3, IR[3] = i2, IR[4] = i1, IR[5] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6;

    } else if (x >= 245 && x <=248){
        qDebug() << i0 << " COP1 BC1 im     6 5 5 16";
        IR[0] = 17, IR[1] = 8, IR[2] = i0, IR[3] = i1;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 16;

    } else if (x >= 249 && x <=250){
        qDebug() << i0 << " COP0 MF0 i12     6 5 5 5 5 5 1";
        IR[0] = 16, IR[1] = 0, IR[2] = i1, IR[3] = 25, IR[4] = 0, IR[5] = i2, IR[6] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 6, BL[6] = 1;

    } else if (x == 251){
        qDebug() << i0 << " COP0 MF0 i12     6 5 5 5 11";
        IR[0] = 16, IR[1] = 0, IR[2] = i1, IR[3] = i2, IR[4] = 0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 11;

    } else if (x >= 252 && x <=258){
        qDebug() << i0 << " COP0 MF0 i11     6 5 5 5 11";
        IR[0] = 16, IR[1] = 0, IR[2] = i1, IR[3] = 24, IR[4] = 0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 11;

    } else if (x >= 259 && x <=260){
        qDebug() << i0 << " COP0 MT0 i12     6 5 5 5 5 5 1";
        IR[0] = 16, IR[1] = 4, IR[2] = i1, IR[3] = 25, IR[4] = 0, IR[5] = i2, IR[6] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 5, BL[5] = 5, BL[6] = 1;

    } else if (x == 261){
        qDebug() << i0 << " COP0 MT0 i12     6 5 5 5 11";
        IR[0] = 16, IR[1] = 4, IR[2] = i1, IR[3] = i2, IR[4] = 0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 11;

    } else if (x >= 262 && x <=268){
        qDebug() << i0 << " COP0 MT0 i1     6 5 5 5 11";
        IR[0] = 16, IR[1] = 4, IR[2] = i1, IR[3] = 24, IR[4] = 0;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 5, BL[4] = 11;

    } else if (x >= 269 && x <=272){
        qDebug() << i0 << " COP0 BC0 im     6 5 5 16";
        IR[0] = 16, IR[1] = 8, IR[2] = i0, IR[3] = i1;
        BL[0] = 6, BL[1] = 5, BL[2] = 5, BL[3] = 16;

    } else if (x >= 273 && x <=275){
        qDebug() << i0 << " COP0 BC0 i     6 5 15 6";
        IR[0] = 16, IR[1] = 16, IR[2] = 0, IR[3] = i0;
        BL[0] = 6, BL[1] = 5, BL[2] = 15, BL[4] = 6;

    } else {
        qDebug() << i0 << " Not a valid instruction name. Defaulting to NOP";
    }

    QByteArray testReturn;
    QByteArray finalReturn;
    QBitArray tempReturn;
    QBitArray tempArray;
    int positionNow = 0;
    tempReturn.resize(32);
    for (int i = 5; i>=0;--i) {
        //qDebug() << "IR: " << QString::number(i) << " " << IR[i];
        //qDebug() << "BL: " << QString::number(i) << "" << BL[i];
        tempArray = intToBit(IR[i], BL[i]);
        //qDebug() << tempArray;

        for (int i = 0;i<tempArray.size(); ++i) {
            tempReturn.setBit(positionNow, tempArray.at(i));
            positionNow += 1;
        }
        //qDebug() << tempReturn;
    }
    testReturn = bitToByte(tempReturn);
    //reverse the bytes for correct endian
    for(int i = testReturn.size()-1;i>=0;--i){
        finalReturn.append(testReturn.at(i));
    }
    return finalReturn;
}

