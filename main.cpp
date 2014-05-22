/*!
 \defgroup Data Data classes

 \defgroup Interlinear Interlinear text display

 \defgroup MorphologicalAnalysis Creating morphological analyses

 \defgroup LexiconEdit Editing the lexicon

 \defgroup GUI Other GUI classes

 \defgroup GUIModel Model classes (that are used in producing graphical user interfaces)

 \defgroup RawAccess Providing the user with raw access to the database or to the XML

 \defgroup TextImport Importing texts (and merging data from external files) into the project

 \defgroup Search Performing searches

 \defgroup IO Input/output functions
 */

#include <QtWidgets/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
