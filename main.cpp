#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.setGeometry(QRect(QPoint(160,90), QSize(MainWindow::hSize,MainWindow::vSize)));
    w.setWindowTitle("MIPS Editor");
    w.show();
    return a.exec();
}
