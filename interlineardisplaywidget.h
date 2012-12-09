/*!
  \class InterlinearDisplayWidget
  \ingroup GUI
  \brief A widget for an interlinear text display. The widget's layout is populated with a series of FlowLayout layouts corresponding to phrases. The widget also keeps a concordance of GlossItem objects, to update their fields when the values are changed.
*/

#ifndef INTERLINEARDISPLAYWIDGET_H
#define INTERLINEARDISPLAYWIDGET_H

#include <QWidget>
#include <QList>
#include <QMultiHash>

class QVBoxLayout;
class GlossItem;

#include "flowlayout.h"
#include "textbit.h"
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
    void updateGloss( const TextBit & bit );
    void updateText( const TextBit & bit );
    void updateMorphologicalAnalysis( const TextBit & bit , const QString & splitString );

private slots:
    void baselineTextUpdated(const QString & baselineText);

    void updateTextFormConcordance(LingEdit * edit, qlonglong newId);
    void updateGlossFormConcordance(LingEdit * edit, qlonglong newId);
//    void updateAllEdits(WordDisplayWidget * wdw, qlonglong newInterpretationId);

private:
    Text *mText;
    Project *mProject;

    void setLayoutFromText();

    LingEdit* addPhrasalGlossLine(  const TextBit & gloss );

    void clearData();

    QVBoxLayout *mLayout;

    QList<QLayout*> mLineLayouts;
    QList<LingEdit*> mPhrasalGlossEdits;
    QList<QWidget*> mWordDisplayWidgets;

    QList<InterlinearItemType> mPhrasalGlossLines;

    QMultiHash<qlonglong,LingEdit*> mTextFormConcordance;
    QMultiHash<qlonglong,LingEdit*> mGlossConcordance;

    QLayout* addLine();
    WordDisplayWidget* addWordDisplayWidget(GlossItem *item);

};

#endif // INTERLINEARDISPLAYWIDGET_H
