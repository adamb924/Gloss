#-------------------------------------------------
#
# Project created by QtCreator 2012-11-24T13:13:00
#
#-------------------------------------------------

QT       += core gui sql xml xmlpatterns

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
    glossdisplaywidget.cpp

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
    glossdisplaywidget.h

FORMS    += mainwindow.ui \
    textdisplaywidget.ui \
    newtextdialog.ui \
    importflextextdialog.ui \
    interpretationsearchdialog.ui \
    writingsystemsdialog.ui \
    mergetranslationdialog.ui \
    generictextinputdialog.ui \
    xqueryinputdialog.ui \
    databasequerydialog.ui

LIBS += -L../quazip
LIBS += -lquazip

LIBS += -L./ \
    -lexslt \
    -llibxslt \
    -llibxml2 \
    -liconv
