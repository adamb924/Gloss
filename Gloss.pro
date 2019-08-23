#-------------------------------------------------
#
# Project created by QtCreator 2012-11-24T13:13:00
#
#-------------------------------------------------

QT       += widgets sql xml xmlpatterns multimedia

TARGET = Gloss
TEMPLATE = app

SOURCES += main.cpp\
    keyboardshortcuts.cpp \
    keystrokedetectinglineedit.cpp \
        mainwindow.cpp \
    flowlayout.cpp \
    project.cpp \
    interlineardisplaywidget.cpp \
    shortcuteditordialog.cpp \
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
    viewconfigurationdialog.cpp \
    viewsmodel.cpp \
    tabsmodel.cpp \
    itemsmodel.cpp \
    phrasalglossesmodel.cpp \
    writingsystemdialog.cpp \
    itemeditdialog.cpp \
    projectoptionsdialog.cpp \
    writingsystemlistmodel.cpp \
    annotationtypelistmodel.cpp \
    exporttextsdialog.cpp \
    syntacticparsingwidget.cpp \
    morphemegraphicsitem.cpp \
    syntacticanalysis.cpp \
    syntacticanalysiselement.cpp \
    constituentgraphicsitem.cpp \
    linenumbergraphicsitem.cpp \
    syntacticanalysiselementmime.cpp \
    syntactictype.cpp \
    syntacticanalysisoptionsdialog.cpp \
    writingsystemseditdialog.cpp \
    tabeditdialog.cpp \
    syntacticvocabularydialog.cpp \
    lexiconlineform.cpp \
    importplaintextdialog.cpp \
    annotationtypeeditordialog.cpp \
    annotation.cpp \
    annotationeditordialog.cpp \
    lingtextedit.cpp \
    punctuationdisplaywidget.cpp \
    texttabwidget.cpp \
    textlistmodel.cpp \
    textmetadatadialog.cpp \
    paragraph.cpp \
    paragraphmarkwidget.cpp \
    segmentationoptionbutton.cpp \
    partofspeechdialog.cpp \
    partofspeech.cpp \
    poswidget.cpp

HEADERS  += mainwindow.h \
    flowlayout.h \
    keyboardshortcuts.h \
    keystrokedetectinglineedit.h \
    project.h \
    interlineardisplaywidget.h \
    shortcuteditordialog.h \
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
    viewconfigurationdialog.h \
    viewsmodel.h \
    tabsmodel.h \
    itemsmodel.h \
    phrasalglossesmodel.h \
    writingsystemdialog.h \
    itemeditdialog.h \
    projectoptionsdialog.h \
    writingsystemlistmodel.h \
    annotationtypelistmodel.h \
    exporttextsdialog.h \
    syntacticparsingwidget.h \
    morphemegraphicsitem.h \
    syntacticanalysis.h \
    syntacticanalysiselement.h \
    constituentgraphicsitem.h \
    linenumbergraphicsitem.h \
    syntacticanalysiselementmime.h \
    syntactictype.h \
    syntacticanalysisoptionsdialog.h \
    writingsystemseditdialog.h \
    tabeditdialog.h \
    syntacticvocabularydialog.h \
    lexiconlineform.h \
    importplaintextdialog.h \
    annotationtypeeditordialog.h \
    annotation.h \
    annotationeditordialog.h \
    lingtextedit.h \
    punctuationdisplaywidget.h \
    texttabwidget.h \
    textlistmodel.h \
    textmetadatadialog.h \
    paragraph.h \
    paragraphmarkwidget.h \
    segmentationoptionbutton.h \
    partofspeechdialog.h \
    partofspeech.h \
    poswidget.h

FORMS    += mainwindow.ui \
    importflextextdialog.ui \
    mergetranslationdialog.ui \
    generictextinputdialog.ui \
    shortcuteditordialog.ui \
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
    viewconfigurationdialog.ui \
    writingsystemdialog.ui \
    itemeditdialog.ui \
    projectoptionsdialog.ui \
    exporttextsdialog.ui \
    syntacticparsingwidget.ui \
    syntacticanalysisoptionsdialog.ui \
    writingsystemseditdialog.ui \
    tabeditdialog.ui \
    syntacticvocabularydialog.ui \
    lexiconlineform.ui \
    importplaintextdialog.ui \
    annotationtypeeditordialog.ui \
    annotationeditordialog.ui \
    textmetadatadialog.ui \
    partofspeechdialog.ui \
    poswidget.ui

unix:INCLUDEPATH += "/usr/include/libxml2"

# Approximately every other Qt update, I have switch back
# and forth between these two sets of library includes. I
# am certain there is a logic to it, but I do not understand
# it at all.
LIBS += -L./ \
    -lquazip \
#    -llibexslt \
#    -llibxslt \
#    -llibxml2
    -llibexslt \
    -lxslt \
    -lxml2

RC_ICONS = icons/Gloss.ico
