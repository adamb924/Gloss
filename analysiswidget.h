#ifndef ANALYSISWIDGET_H
#define ANALYSISWIDGET_H

#include <QWidget>

class GlossItem;
class DatabaseAdapter;

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

private slots:
    void enterAnalysis();
};

#endif // ANALYSISWIDGET_H
