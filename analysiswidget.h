/*!
  \class AnalysisWidget
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
class Project;

#include "writingsystem.h"

class AnalysisWidget : public QWidget
{
    Q_OBJECT
public:
    AnalysisWidget(GlossItem *glossItem, const WritingSystem & analysisWs, Qt::LayoutDirection layoutDirection, Project *project, QWidget *parent = 0);

    void createAndDisplayAnalysis(qlonglong lexicalEntryId);
signals:
    void morphologicalAnalysisChanged(const GlossItem * originator, const MorphologicalAnalysis * analysis);
    void requestAlternateInterpretation();

public slots:
    void setupLayout();

private slots:
    void allomorphDoubleClick(TextBit & bit);

private:
    Project *mProject;
    GlossItem *mGlossItem;
    WritingSystem mWritingSystem;
    Qt::LayoutDirection mLayoutDirection;

    const DatabaseAdapter *mDbAdapter;

    QVBoxLayout *mLayout;

    void createInitializedLayout(const MorphologicalAnalysis * analysis);
    void createUninitializedLayout();

    void clearWidgetsFromLayout(QLayout *layout);

    qlonglong selectCandidateLexicalEntry();

    inline TextBit textBit() const { return mGlossItem->textForms()->value(mWritingSystem); }

protected:
    void contextMenuEvent ( QContextMenuEvent * event );

private slots:
    void enterAnalysis();
    void createMonomorphemicLexicalEntry();
    void createQuickMonomorphemicLexicalEntry();
    void linkToOther();
    void editLexicalEntry(QAction * action);
    void editLexicalEntry(qlonglong allomorphId);
};

#endif // ANALYSISWIDGET_H
