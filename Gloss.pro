#-------------------------------------------------
#
# Project created by QtCreator 2012-11-24T13:13:00
#
#-------------------------------------------------

QT       += core gui sql xml xmlpatterns multimedia

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
    interpretationsearchdialog.cpp \
    writingsystemsdialog.cpp \
    mergetranslationdialog.cpp \
    xsltproc.cpp \
    generictextinputdialog.cpp \
    searchquerymodel.cpp \
    xqueryinputdialog.cpp \
    databasequerydialog.cpp \
    analysisdisplaywidget.cpp \
    glossdisplaywidget.cpp \
    immutablelabel.cpp \
    analysiswidget.cpp \
    createanalysisdialog.cpp \
    lexicalentryform.cpp \
    allomorph.cpp \
    genericlexicalentryform.cpp \
    createlexicalentrydialog.cpp \
    chooselexicalentriesdialog.cpp \
    morphologicalanalysis.cpp \
    interlinearlinelabel.cpp \
    replacedialog.cpp \
    searchreplaceform.cpp \
    singlephraseeditdialog.cpp \
    searchqueryview.cpp \
    phrasedisplaywidget.cpp \
    mergeeafdialog.cpp \
    annotation.cpp \
    sound.cpp \
    concordance.cpp

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
    interpretationsearchdialog.h \
    writingsystemsdialog.h \
    mergetranslationdialog.h \
    xsltproc.h \
    generictextinputdialog.h \
    searchquerymodel.h \
    xqueryinputdialog.h \
    databasequerydialog.h \
    analysisdisplaywidget.h \
    glossdisplaywidget.h \
    immutablelabel.h \
    analysiswidget.h \
    createanalysisdialog.h \
    lexicalentryform.h \
    allomorph.h \
    genericlexicalentryform.h \
    createlexicalentrydialog.h \
    chooselexicalentriesdialog.h \
    morphologicalanalysis.h \
    interlinearlinelabel.h \
    replacedialog.h \
    searchreplaceform.h \
    singlephraseeditdialog.h \
    searchqueryview.h \
    phrasedisplaywidget.h \
    mergeeafdialog.h \
    annotation.h \
    sound.h \
    concordance.h

FORMS    += mainwindow.ui \
    textdisplaywidget.ui \
    newtextdialog.ui \
    importflextextdialog.ui \
    interpretationsearchdialog.ui \
    writingsystemsdialog.ui \
    mergetranslationdialog.ui \
    generictextinputdialog.ui \
    xqueryinputdialog.ui \
    databasequerydialog.ui \
    createanalysisdialog.ui \
    lexicalentryform.ui \
    genericlexicalentryform.ui \
    createlexicalentrydialog.ui \
    replacedialog.ui \
    searchreplaceform.ui \
    singlephraseeditdialog.ui \
    mergeeafdialog.ui

LIBS += -L../quazip
LIBS += -lquazip

LIBS += -L./ \
    -lexslt \
    -llibxslt \
    -llibxml2 \
    -liconv

RESOURCES +=
