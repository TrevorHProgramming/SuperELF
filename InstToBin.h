#ifndef INSTTOBIN_H
#define INSTTOBIN_H

#include <QString>

class InstToBin{
 public:

    QByteArray convFromInst(QString instruction);
 private:
    int fI(QString seek);
    int fG(QString seek);
    int fF(QString seek);
    int fID(int seek);
};

#endif // INSTTOBIN_H
