#ifndef INTERLINEARDISPLAYWIDGET_H
#define INTERLINEARDISPLAYWIDGET_H

#include <QWidget>
#include <QList>
#include <QMultiHash>

class QVBoxLayout;

#include "project.h"
#include "flowlayout.h"
#include "textbit.h"
#include "textlocation.h"
#include "worddisplaywidget.h"

class InterlinearDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InterlinearDisplayWidget(WritingSystem *baselineWs, Project::BaselineMode baselineMode, Project *project, QWidget *parent = 0);
    ~InterlinearDisplayWidget();

    WritingSystem* writingSystem() const;
    void setWritingSystem(WritingSystem *ws);

signals:

public slots:
    void setText(const QString& text);

private slots:
    void updateGloss( const TextBit & bit );
    void updateTranscription( const TextBit & bit );
    void updateOrthography( const TextBit & bit );
    void updateMorphologicalAnalysis( const TextBit & bit , const QString & splitString );

private:
    Project *mProject;

    WritingSystem *mWritingSystem; // for the baseline text

//    QMultiHash<qlonglong, TextLocation> mConcordance;
    QMultiHash<qlonglong, WordDisplayWidget*> mConcordance;

    QString mText;

    void setLayoutFromText();

    void clearData();

    QVBoxLayout *mLayout;

    Project::BaselineMode mBaselineMode;

    QList< QList<TextBit*>* > mTextBits;

    QList<FlowLayout*> mLineLayouts;
    QList<QWidget*> mWordDisplayWidgets;

private slots:
    void updateConcordance( WordDisplayWidget *w, qlonglong oldId, qlonglong newId );
};

#endif // INTERLINEARDISPLAYWIDGET_H
