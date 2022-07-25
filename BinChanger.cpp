#include "mainwindow.h"

long FileData::readLong(int length, long location){
    long readValue = parent->binChanger.reverse_input(parent->fileData.dataBytes.mid(currentPosition + location, length).toHex(),2).toLong(nullptr, 16);
    currentPosition += length;
    return readValue;
}

int FileData::readInt(int length, long location){
    int readValue = parent->binChanger.reverse_input(parent->fileData.dataBytes.mid(currentPosition + location, length).toHex(),2).toInt(nullptr, 16);
    currentPosition += length;
    return readValue;
}

int FileData::readUInt(int length, long location){
    int readValue = parent->binChanger.reverse_input(parent->fileData.dataBytes.mid(currentPosition + location, length).toHex(),2).toUInt(nullptr, 16);
    currentPosition += length;
    return readValue;
}

bool FileData::readBool(int length, long location){
    bool readValue = parent->fileData.dataBytes.mid(currentPosition + location, length).toHex().toInt(nullptr, 16);
    currentPosition += length;
    return readValue;
}

float FileData::readFloat(int length, long location){
    float readValue = parent->binChanger.hex_to_float(parent->binChanger.reverse_input(parent->fileData.dataBytes.mid(currentPosition + location, length).toHex(), 2));
    currentPosition += length;
    return readValue;
}

QByteArray FileData::readHex(int length, long location){
    QByteArray readValue = dataBytes.mid(currentPosition + location, length);
    currentPosition += length;
    return readValue;
}

QByteArray FileData::mid(long location, int length){
    QByteArray readValue = dataBytes.mid(location, length);
    currentPosition += length;
    return readValue;
}

void FileData::readFile(QString filePath){
    dataBytes.clear();

    QFile inputFile(filePath);
    inputFile.open(QIODevice::ReadOnly);
    dataBytes = inputFile.readAll();
    qDebug() << Q_FUNC_INFO << "file data read length:" << dataBytes.size();
    currentPosition = 0;
}

QString BinChanger::signExtend(QString input, int length){
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

QByteArray BinChanger::remakeImm(QString tempRead, long immediate){
    QString replacement;
    QByteArray replacementArray;
    QString paddedImm;
    tempRead = tempRead.mid(0,16); //get the unaltered part of the instruction
    paddedImm = QString::number(immediate, 2); //get a binary string of the immediate
    while (paddedImm.length()<16) {
        paddedImm = "0"+paddedImm; //pad out to 16 bits. done differently for negative immediates. figure out a fix for that.
    }
    tempRead += paddedImm;
    tempRead = reverse_input(tempRead,8); //reverse the whole instruction
    replacement = QString("%1").arg(tempRead.toULong(nullptr,2),8,16,QChar('0')); //convert binary string into hex string
    replacementArray = QByteArray::fromHex(replacement.toUtf8());
    return replacementArray;
}

int BinChanger::twosCompConv(QString input, int length){
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

QString BinChanger::twosCompConv(int intput, int length){
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

QString BinChanger::reverse_input(QString input, int unitLength) {
    QString part;
    QString output = "";
    int bytes = int(input.length()/unitLength);
    for (int i = bytes; i >= 0; --i){
        part = input.mid(i*unitLength, unitLength);
        output += part;
    }
    return output;
}

QByteArray BinChanger::reverse_input(QByteArray input, int unitLength) {
    QByteArray part;
    QByteArray output = "";
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

QString BinChanger::hex_to_bin(QByteArray arrhex)
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

float BinChanger::hex_to_float(QByteArray array){
    bool ok;
    int sign = 1;
    long long zeroCheck = 0;
    zeroCheck = array.toLongLong(nullptr, 16);
    //qDebug() << Q_FUNC_INFO << "Starting array: " << array;
    //qDebug() << Q_FUNC_INFO << "zerocheck: " << zeroCheck << " for array: " << array;
    if (zeroCheck == 0){
        return 0;
    }
    array = QByteArray::number(array.toLongLong(&ok,16),2); //convert hex to binary
    if(array.length()==32) {
        if(array.at(0)=='1')  sign =-1;                       // if bit 0 is 1 number is negative
        array.remove(0,1);                                     // remove sign bit
    }
    QByteArray fraction =array.right(23);   //get the fractional part
    double mantissa = 0;
    for(int i=0;i<fraction.length();i++)     // iterate through the array to claculate the fraction as a decimal.
        if(fraction.at(i)=='1')     mantissa += 1.0/(pow(2,i+1));
    int exponent = array.left(array.length()-23).toLongLong(&ok,2);
    //qDebug() << Q_FUNC_INFO << "binary read: " << QString(array.left(array.length()-23)) << "exponent: " << exponent;
    exponent -= 127;     //claculate the exponent
    //qDebug() << Q_FUNC_INFO << "number= "<< QString::number( sign*pow(2,exponent)*(mantissa+1.0),'f', 5 );
    return sign*pow(2,exponent)*(mantissa+1.0);
}
