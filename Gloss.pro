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
    glossline.cpp \
    textbit.cpp \
    databaseadapter.cpp \
    newtextdialog.cpp \
    importflextextdialog.cpp \
    text.cpp \
    glossitem.cpp \
    phrase.cpp \
    messagehandler.cpp \
    lingtextedit.cpp

HEADERS  += mainwindow.h \
    flowlayout.h \
    project.h \
    textdisplaywidget.h \
    interlineardisplaywidget.h \
    worddisplaywidget.h \
    writingsystem.h \
    lingedit.h \
    glossline.h \
    textbit.h \
    databaseadapter.h \
    newtextdialog.h \
    importflextextdialog.h \
    text.h \
    glossitem.h \
    phrase.h \
    messagehandler.h \
    lingtextedit.h

FORMS    += mainwindow.ui \
    textdisplaywidget.ui \
    newtextdialog.ui \
    importflextextdialog.ui

LIBS += -L../quazip
LIBS += -lquazip
