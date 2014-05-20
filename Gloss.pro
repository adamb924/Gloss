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
    newtextdialog.cpp \
    importflextextdialog.cpp \
    text.cpp \
    glossitem.cpp \
    phrase.cpp \
    messagehandler.cpp \
    lingtextedit.cpp \
    interlinearitemtype.cpp \
    writingsystemsdialog.cpp \
    mergetranslationdialog.cpp \
    xsltproc.cpp \
    generictextinputdialog.cpp \
    searchquerymodel.cpp \
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
    replacedialog.cpp \
    searchreplaceform.cpp \
    searchqueryview.cpp \
    mergeeafdialog.cpp \
    annotation.cpp \
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
    annotationmodel.cpp

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
    newtextdialog.h \
    importflextextdialog.h \
    text.h \
    phrase.h \
    messagehandler.h \
    lingtextedit.h \
    glossitem.h \
    interlinearitemtype.h \
    writingsystemsdialog.h \
    mergetranslationdialog.h \
    xsltproc.h \
    generictextinputdialog.h \
    searchquerymodel.h \
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
    replacedialog.h \
    searchreplaceform.h \
    searchqueryview.h \
    mergeeafdialog.h \
    annotation.h \
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
    annotationmodel.h

FORMS    += mainwindow.ui \
    newtextdialog.ui \
    importflextextdialog.ui \
    writingsystemsdialog.ui \
    mergetranslationdialog.ui \
    generictextinputdialog.ui \
    xqueryinputdialog.ui \
    databasequerydialog.ui \
    createanalysisdialog.ui \
    lexicalentryform.ui \
    createlexicalentrydialog.ui \
    replacedialog.ui \
    searchreplaceform.ui \
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
    annotationform.ui

LIBS += -L./ \
    -lquazip \
    -llibexslt \
    -llibxslt \
    -llibxml2 \
    -liconv

RESOURCES +=

QMAKE_CXXFLAGS_WARN_OFF -= -Wunused-parameter
