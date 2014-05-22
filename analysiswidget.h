/*!
  \class AnalysisWidget
  \ingroup GUI
  \ingroup Interlinear
  \ingroup MorphologicalAnalysis
  \brief This is a widget for displaying a morphological analysis (and associated interface items) within the interlinear display (i.e., parallel to LingEdit and LingTextEdit objects). It is instantiated in WordDisplayWidget.
*/

#ifndef ANALYSISWIDGET_H
#define ANALYSISWIDGET_H

#include <QWidget>

#include "textbit.h"
#include "glossitem.h"

class DatabaseAdapter;
class QVBoxLayout;
class MorphologicalAnalysis;

#include "writingsystem.h"

class AnalysisWidget : public QWidget
{
    Q_OBJECT
public:
    AnalysisWidget(GlossItem *glossItem, const WritingSystem & analysisWs, const DatabaseAdapter *dbAdapter, QWidget *parent = 0);

signals:
    void morphologicalAnalysisChanged(MorphologicalAnalysis * analysis);
    void requestAlternateInterpretation();

public slots:
    void setupLayout();

private:
    GlossItem *mGlossItem;


    WritingSystem mWritingSystem;

    const DatabaseAdapter *mDbAdapter;

    QVBoxLayout *mLayout;

    void createInitializedLayout(const MorphologicalAnalysis * analysis);
    void createUninitializedLayout();

    void clearWidgetsFromLayout();

    qlonglong selectCandidateLexicalEntry();

    inline TextBit textBit() const { return mGlossItem->textForms()->value(mWritingSystem); }

protected:
    void contextMenuEvent ( QContextMenuEvent * event );

private slots:
    void enterAnalysis();
    void createMonomorphemicLexicalEntry();
    void linkToOther();
    void editLexicalEntry(QAction * action);
};

#endif // ANALYSISWIDGET_H
