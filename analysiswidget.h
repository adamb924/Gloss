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

#include "writingsystem.h"

class AnalysisWidget : public QWidget
{
    Q_OBJECT
public:
    AnalysisWidget(GlossItem *glossItem, const WritingSystem & analysisWs, DatabaseAdapter *dbAdapter, QWidget *parent = 0);

signals:
    void morphologicalAnalysisChanged(qlonglong textFormId);

public slots:
    void createInitializedLayout();

private:
    GlossItem *mGlossItem;

    void setupLayout();

    WritingSystem mWritingSystem;

    DatabaseAdapter *mDbAdapter;

    QVBoxLayout *mLayout;

    void createUninitializedLayout();
    void clearWidgetsFromLayout();

protected:
    void contextMenuEvent ( QContextMenuEvent * event );

private slots:
    void enterAnalysis();
    void createMonomorphemicLexicalEntry();
};

#endif // ANALYSISWIDGET_H
