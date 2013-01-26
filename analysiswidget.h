/*!
  \class AnalysisWidget
  \ingroup GUI
  \brief This is a widget for displaying a morphological analysis (and associated interface items). It is intended to be used in an WordDisplayWidget.
*/

#ifndef ANALYSISWIDGET_H
#define ANALYSISWIDGET_H

#include <QWidget>

class GlossItem;
class DatabaseAdapter;
class QVBoxLayout;
class MorphologicalAnalysis;

#include "writingsystem.h"

class AnalysisWidget : public QWidget
{
    Q_OBJECT
public:
    AnalysisWidget(const GlossItem *glossItem, const WritingSystem & analysisWs, const DatabaseAdapter *dbAdapter, QWidget *parent = 0);

signals:
    void morphologicalAnalysisChanged(const MorphologicalAnalysis & analysis);

public slots:
    void setupLayout();

private:
    const GlossItem *mGlossItem;


    WritingSystem mWritingSystem;

    const DatabaseAdapter *mDbAdapter;

    QVBoxLayout *mLayout;

    void createInitializedLayout(const MorphologicalAnalysis & analysis);
    void createUninitializedLayout();

    void clearWidgetsFromLayout();

protected:
    void contextMenuEvent ( QContextMenuEvent * event );

private slots:
    void enterAnalysis();
    void createMonomorphemicLexicalEntry();
};

#endif // ANALYSISWIDGET_H
