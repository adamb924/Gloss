#ifndef INTERLINEARDISPLAYWIDGET_H
#define INTERLINEARDISPLAYWIDGET_H

#include <QWidget>
#include <QList>
#include <QMultiHash>

class QVBoxLayout;

#include "flowlayout.h"
#include "textbit.h"
#include "textlocation.h"
#include "worddisplaywidget.h"

class Text;

class InterlinearDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InterlinearDisplayWidget(Text *text, Project *project, QWidget *parent = 0);
    ~InterlinearDisplayWidget();

signals:

public slots:

private slots:
    void updateGloss( const TextBit & bit );
    void updateTranscription( const TextBit & bit );
    void updateOrthography( const TextBit & bit );
    void updateMorphologicalAnalysis( const TextBit & bit , const QString & splitString );

private:
    Text *mText;
    Project *mProject;

    void setLayoutFromText();

    void clearData();

    QVBoxLayout *mLayout;

    QList<FlowLayout*> mLineLayouts;
    QList<QWidget*> mWordDisplayWidgets;
    QMultiHash<qlonglong, WordDisplayWidget*> mConcordance;

    FlowLayout* addLine();
    WordDisplayWidget* addWordDisplayWidget(TextBit *bit);

private slots:
    void updateConcordance( WordDisplayWidget *w, qlonglong oldId, qlonglong newId );
    void baselineTextUpdated(const QString & baselineText);
};

#endif // INTERLINEARDISPLAYWIDGET_H
