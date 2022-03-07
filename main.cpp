#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ProgWindow w;
    w.setGeometry(QRect(QPoint(160,90), QSize(ProgWindow::hSize,ProgWindow::vSize)));
    w.setWindowTitle("MIPS Editor");
    w.show();
    return a.exec();
}
