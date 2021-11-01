#include "mainwindow.h"

QString MainWindow::signExtend(QString input, int length){
    bool signBit;
    QString output = input;
    signBit = input.left(1).toInt();
    if(input.length() < length){
        if(signBit){
            while(output.length() < length){
                output = "1" + output;
            }
        } else {
            while(output.length() < length){
                output = "0" + output;
            }
        }
    }
    return output;
}

int MainWindow::twosCompConv(QString input, int length){
    bool isSet;
    bool signBit;
    QString tempValue;
    input = signExtend(input, length);
    signBit = input.left(1).toInt();
    if (signBit){
        for(int i = input.length(); i >= 0 ; i--){
            isSet = input.mid(i, 1).toInt();
            if (isSet){
                for(int j = 0; j < i; j++){
                    if (input.mid(j, 1).toInt() == 1){
                        tempValue += "0";
                    }
                    else {
                        tempValue += "1";
                    }
                }
                for(int j = i; j <= input.length(); j++){
                    tempValue += input.mid(j, 1);
                }
                //qDebug() << "twos comp convert: " << input << " goes to " << tempValue << " " << tempValue.toInt(nullptr, 2);
                return 0 - tempValue.toInt(nullptr, 2);
            }
        }
        //qDebug() << "twos comp convert: no set bits found.";
        return 0 - input.toInt(nullptr, 2);
    }
    return input.toInt(nullptr, 2);
}

QString MainWindow::twosCompConv(int intput, int length){
    bool isSet;
    bool signBit;
    QString tempValue;
    QString input = QStringLiteral("%1").arg(intput, length, 2, QLatin1Char('0'));
    //QString input = QString::number(intput, 2);
    input = signExtend(input, length);
    signBit = input.left(1).toInt();
    if (signBit){
        for(int i = input.length(); i >= 0 ; i--){
            isSet = input.mid(i, 1).toInt();
            if (isSet){
                for(int j = 0; j < i; j++){
                    if (input.mid(j, 1).toInt() == 1){
                        tempValue += "0";
                    }
                    else {
                        tempValue += "1";
                    }
                }
                for(int j = i; j <= input.length(); j++){
                    tempValue += input.mid(j, 1);
                }
                //qDebug() << "twos comp convert: " << input << " goes to " << tempValue << " " << tempValue.toInt(nullptr, 2);
                return "-0x" + QString::number(tempValue.toInt(nullptr, 2), 16);
                //return QString::number(0 - tempValue.toInt(nullptr, 2));
            }
        }
        //qDebug() << "twos comp convert: no set bits found.";
        return "-0x" + QString::number(input.toInt(nullptr, 2), 16);
        //return QString::number(0 - input.toInt(nullptr, 2));
    }
    return "0x" + QString::number(input.toInt(nullptr, 2), 16);
    //return QString::number(input.toInt(nullptr, 2));
}

QString MainWindow::reverse_input(QString input, int unitLength) {
    QString part;
    QString output = "";
    int bytes = int(input.length()/unitLength);
    for (int i = bytes; i >= 0; --i){
        part = input.mid(i*unitLength, unitLength);
        output += part;
    }
    return output;
}

QString hex_char_to_bin(QChar c)
{
    switch (c.toUpper().unicode())
    {
    case '0': return "0000";
    case '1': return "0001";
    case '2': return "0010";
    case '3': return "0011";
    case '4': return "0100";
    case '5': return "0101";
    case '6': return "0110";
    case '7': return "0111";
    case '8': return "1000";
    case '9': return "1001";
    case 'A': return "1010";
    case 'B': return "1011";
    case 'C': return "1100";
    case 'D': return "1101";
    case 'E': return "1110";
    case 'F': return "1111";
    default:  return "BR";
    }
}

QString MainWindow::hex_to_bin(QByteArray arrhex)
{
    QString hex = arrhex.toHex();
    unsigned long long hexlen = hex.length();
    QString bin;
    for (unsigned long long i = 0; i < hexlen; ++i) {
        QString binhex = hex_char_to_bin(hex[i]);
        bin += binhex;
    }
    return bin;
}

qint64 MainWindow::byteWrite( QFile& file, int8_t var ) {
  qint64 toWrite = sizeof(decltype (var));
  qint64  written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug () << "write error";
  }
   //qDebug () << "out: " << written;
  return written;
}

qint64 MainWindow::shortWrite( QFile& file, int16_t var ) {
  qint64 toWrite = sizeof(decltype (var));
  qint64 written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug () << "write error";
  }
   //qDebug () << "out: " << written;
  return written;
}

qint64 MainWindow::intWrite( QFile& file, int32_t var ) {
  qint64 toWrite = sizeof(decltype (var));
  qint64  written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug () << "write error";
  }
   //qDebug () << "out: " << written;
  return written;
}

qint64 MainWindow::longWrite( QFile& file, int64_t var ) {
  qint64 toWrite = sizeof(decltype (var));
  qint64  written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug () << "write error";
  }
   //qDebug () << "out: " << written;
  return written;
}
