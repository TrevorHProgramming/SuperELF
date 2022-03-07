QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    BinChanger.cpp \
    BinToInst.cpp \
    BranchFinder.cpp \
    InstToBin.cpp \
    MIPSReader.cpp \
    ModManager.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    BinChanger.h \
    BinToInst.h \
    BranchFinder.h \
    InstToBin.h \
    MIPSReader.h \
    ModManager.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    CODE.TXT \
    DefaultText.txt \
    SLUS_206.68
