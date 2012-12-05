/*!
 \defgroup GUI User interface classes
 \defgroup Data Data interface classes
 \defgroup DataStructures Data structure classes
 */

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
