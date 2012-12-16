/*!
  \class InterlinearDisplayWidget
  \ingroup GUI
  \brief A widget for an interlinear text display. The widget's layout is populated with a series of FlowLayout layouts corresponding to phrases. The widget also keeps a concordance of LingEdit objects, to update their fields when the values are changed.
*/

#ifndef INTERLINEARDISPLAYWIDGET_H
#define INTERLINEARDISPLAYWIDGET_H

#include <QScrollArea>
#include <QList>
#include <QMultiHash>
#include <QSet>

class QVBoxLayout;
class QScrollArea;
class GlossItem;

#include "flowlayout.h"
#include "textbit.h"
#include "worddisplaywidget.h"

class Text;

class InterlinearDisplayWidget : public QScrollArea
{
    Q_OBJECT
public:
    explicit InterlinearDisplayWidget(Text *text, Project *project, QWidget *parent = 0);
    ~InterlinearDisplayWidget();

signals:
    //! \brief Emitted whenever the top line number of the widget is changed. \a line is 0-indexed.
    // TODO emit this at some point
    void lineNumberChanged(int line);

public slots:
    void updateGloss( const TextBit & bit );
    void updateText( const TextBit & bit );
    void updateMorphologicalAnalysis( const TextBit & bit , const QString & splitString );

    void removeGlossFromConcordance( LingEdit * edit );
    void removeTextFormFromConcordance( LingEdit * edit );

    void scrollToLine(int line);

protected:
    void scrollContentsBy ( int dx, int dy );

private slots:
    void baselineTextUpdated(const QString & baselineText);

    void updateTextFormConcordance(LingEdit * edit, qlonglong newId);
    void updateGlossFormConcordance(LingEdit * edit, qlonglong newId);

    void saveText();

private:
    Text *mText;
    Project *mProject;
    int mCurrentLine;

    void contextMenuEvent ( QContextMenuEvent * event );

    void setLayoutFromText();

    //! \brief Removes and deletes all widgets from the given \a layout
    void clearWidgets(QLayout * layout);

    //! \brief Add the phrasal gloss lines for phrase \a i
    void addPhrasalGlossLines( int i );

    //! \brief Add a line label for phrase \a i
    void addLineLabel( int i , QLayout * flowLayout  );

    //! \brief Add the word display widgets for phrase \a i to \a flowLayout
    void addWordDisplayWidgets( int i , QLayout * flowLayout );

    LingEdit* addPhrasalGlossLine(  const TextBit & gloss );

    void clearData();

    QVBoxLayout *mLayout;

    QList<QLayout*> mLineLayouts;
    QList<LingEdit*> mPhrasalGlossEdits;
    QSet<QWidget*> mWordDisplayWidgets;

    QList<InterlinearItemType> mPhrasalGlossLines;

    QMultiHash<qlonglong,LingEdit*> mTextFormConcordance;
    QMultiHash<qlonglong,LingEdit*> mGlossConcordance;

    QLayout* addLine();
    WordDisplayWidget* addWordDisplayWidget(GlossItem *item);

};

#endif // INTERLINEARDISPLAYWIDGET_H
