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

public slots:

private:
    GlossItem *mGlossItem;

    void setupLayout();

    WritingSystem mWritingSystem;

    DatabaseAdapter *mDbAdapter;

    QVBoxLayout *mLayout;

    void createUninitializedLayout();
    void createInitializedLayout();
    void clearWidgetsFromLayout();

protected:
    void contextMenuEvent ( QContextMenuEvent * event );

private slots:
    void enterAnalysis();
    void createMonomorphemicLexicalEntry();
};

#endif // ANALYSISWIDGET_H
