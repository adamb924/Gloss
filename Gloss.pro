#-------------------------------------------------
#
# Project created by QtCreator 2012-11-24T13:13:00
#
#-------------------------------------------------

QT       += widgets sql xml xmlpatterns multimedia

TARGET = Gloss
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    flowlayout.cpp \
    project.cpp \
    textdisplaywidget.cpp \
    interlineardisplaywidget.cpp \
    worddisplaywidget.cpp \
    writingsystem.cpp \
    lingedit.cpp \
    textbit.cpp \
    databaseadapter.cpp \
    importflextextdialog.cpp \
    text.cpp \
    glossitem.cpp \
    phrase.cpp \
    messagehandler.cpp \
    interlinearitemtype.cpp \
    mergetranslationdialog.cpp \
    xsltproc.cpp \
    generictextinputdialog.cpp \
    xquerymodel.cpp \
    xqueryinputdialog.cpp \
    databasequerydialog.cpp \
    immutablelabel.cpp \
    analysiswidget.cpp \
    createanalysisdialog.cpp \
    lexicalentryform.cpp \
    allomorph.cpp \
    createlexicalentrydialog.cpp \
    chooselexicalentriesdialog.cpp \
    morphologicalanalysis.cpp \
    interlinearlinelabel.cpp \
    searchqueryview.cpp \
    mergeeafdialog.cpp \
    sound.cpp \
    concordance.cpp \
    flextextwriter.cpp \
    flextextreader.cpp \
    lexicalentrysearchdialog.cpp \
    indexsearchmodel.cpp \
    lexiconedit.cpp \
    lexiconview.cpp \
    allomorphmodel.cpp \
    morphologicalanalysismodel.cpp \
    sqltabledialog.cpp \
    tagmodel.cpp \
    tagview.cpp \
    alltagsmodel.cpp \
    lexiconmodel.cpp \
    sqlquerywriter.cpp \
    focus.cpp \
    dealwithspacesdialog.cpp \
    tab.cpp \
    view.cpp \
    flextextimporter.cpp \
    interlinearchunkeditor.cpp \
    baselinesearchreplacedialog.cpp \
    writingsystemcombo.cpp \
    annotationmarkwidget.cpp \
    annotationtype.cpp \
    opentextdialog.cpp \
    editwithsuggestionsdialog.cpp \
    searchform.cpp \
    annotationform.cpp \
    annotationmodel.cpp \
    interval.cpp \
    writingsystemsform.cpp \
    viewconfigurationdialog.cpp \
    viewsmodel.cpp \
    tabsmodel.cpp \
    itemsmodel.cpp \
    phrasalglossesmodel.cpp \
    writingsystemdialog.cpp \
    itemeditdialog.cpp \
    projectoptionsdialog.cpp \
    writingsystemlistmodel.cpp \
    annotationeditordialog.cpp \
    annotationtypelistmodel.cpp \
    exporttextsdialog.cpp \
    syntacticparsingwidget.cpp

HEADERS  += mainwindow.h \
    flowlayout.h \
    project.h \
    textdisplaywidget.h \
    interlineardisplaywidget.h \
    worddisplaywidget.h \
    writingsystem.h \
    lingedit.h \
    textbit.h \
    databaseadapter.h \
    importflextextdialog.h \
    text.h \
    phrase.h \
    messagehandler.h \
    glossitem.h \
    interlinearitemtype.h \
    mergetranslationdialog.h \
    xsltproc.h \
    generictextinputdialog.h \
    xquerymodel.h \
    xqueryinputdialog.h \
    databasequerydialog.h \
    immutablelabel.h \
    analysiswidget.h \
    createanalysisdialog.h \
    lexicalentryform.h \
    allomorph.h \
    createlexicalentrydialog.h \
    chooselexicalentriesdialog.h \
    morphologicalanalysis.h \
    interlinearlinelabel.h \
    searchqueryview.h \
    mergeeafdialog.h \
    sound.h \
    concordance.h \
    flextextwriter.h \
    flextextreader.h \
    lexicalentrysearchdialog.h \
    indexsearchmodel.h \
    lexiconedit.h \
    lexiconview.h \
    allomorphmodel.h \
    morphologicalanalysismodel.h \
    sqltabledialog.h \
    tagmodel.h \
    tagview.h \
    alltagsmodel.h \
    lexiconmodel.h \
    sqlquerywriter.h \
    focus.h \
    dealwithspacesdialog.h \
    tab.h \
    view.h \
    flextextimporter.h \
    interlinearchunkeditor.h \
    baselinesearchreplacedialog.h \
    writingsystemcombo.h \
    annotationmarkwidget.h \
    annotationtype.h \
    opentextdialog.h \
    editwithsuggestionsdialog.h \
    searchform.h \
    annotationform.h \
    annotationmodel.h \
    interval.h \
    writingsystemsform.h \
    viewconfigurationdialog.h \
    viewsmodel.h \
    tabsmodel.h \
    itemsmodel.h \
    phrasalglossesmodel.h \
    writingsystemdialog.h \
    itemeditdialog.h \
    projectoptionsdialog.h \
    writingsystemlistmodel.h \
    annotationeditordialog.h \
    annotationtypelistmodel.h \
    exporttextsdialog.h \
    syntacticparsingwidget.h

FORMS    += mainwindow.ui \
    importflextextdialog.ui \
    mergetranslationdialog.ui \
    generictextinputdialog.ui \
    xqueryinputdialog.ui \
    databasequerydialog.ui \
    createanalysisdialog.ui \
    lexicalentryform.ui \
    createlexicalentrydialog.ui \
    replacedialog.ui \
    mergeeafdialog.ui \
    lexicalentrysearchdialog.ui \
    lexiconedit.ui \
    sqltabledialog.ui \
    dealwithspacesdialog.ui \
    interlinearchunkeditor.ui \
    baselinesearchreplacedialog.ui \
    opentextdialog.ui \
    editwithsuggestionsdialog.ui \
    searchform.ui \
    annotationform.ui \
    writingsystemsform.ui \
    viewconfigurationdialog.ui \
    writingsystemdialog.ui \
    itemeditdialog.ui \
    projectoptionsdialog.ui \
    annotationeditordialog.ui \
    exporttextsdialog.ui \
    syntacticparsingwidget.ui

LIBS += -L./ \
    -lquazip \
    -llibexslt \
    -llibxslt \
    -llibxml2 \
    -liconv

RESOURCES +=

QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter
QMAKE_CFLAGS_WARN_OFF -= -Wunused-parameter
