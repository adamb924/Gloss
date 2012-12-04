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

private slots:
    void updateGloss( const TextBit & bit );
    void updateText( const TextBit & bit );
    void updateMorphologicalAnalysis( const TextBit & bit , const QString & splitString );

private:
    Text *mText;
    Project *mProject;

    void setLayoutFromText();

    void clearData();

    QVBoxLayout *mLayout;

    QList<QLayout*> mLineLayouts;
    QList<QWidget*> mWordDisplayWidgets;
    QMultiHash<qlonglong, WordDisplayWidget*> mConcordance;

    QLayout* addLine();
    WordDisplayWidget* addWordDisplayWidget(GlossItem *item);

private slots:
    void updateConcordance( WordDisplayWidget *w, qlonglong oldId, qlonglong newId );
    void baselineTextUpdated(const QString & baselineText);
};

#endif // INTERLINEARDISPLAYWIDGET_H
