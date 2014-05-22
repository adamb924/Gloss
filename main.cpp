/*!
 \defgroup GUI Graphical user interface classes

 \defgroup GUIModel Model classes that are used in producing graphical user interfaces.

 \defgroup Data Data interface classes

 \defgroup DataStructures Data structure classes

 \defgroup Interlinear Classes pertaining to the interlinear text display

 \defgroup MorphologicalAnalysis Classes having to do the morphological analysis interface.

 \defgroup RawAccess Classes providing the user with raw access to the database or to the XML.

 \defgroup TextImport Classes related to importing texts into the project.

 \defgroup LexiconEdit Classes for editing the lexicon.

 \defgroup Search Classes for performing searches.
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
