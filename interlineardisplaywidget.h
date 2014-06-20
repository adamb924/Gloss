/*!
  \class InterlinearDisplayWidget
  \ingroup Interlinear
  \brief A widget for an interlinear text display. The widget's layout is populated with a series of FlowLayout layouts corresponding to phrases.
*/

#ifndef INTERLINEARDISPLAYWIDGET_H
#define INTERLINEARDISPLAYWIDGET_H

#include <QScrollArea>
#include <QList>
#include <QSet>

class QVBoxLayout;
class QScrollArea;
class Text;
class Project;
class FlowLayout;
class LingEdit;
class TextBit;
class InterlinearLineLabel;
class WordDisplayWidget;
class GlossItem;
class Phrase;
class Focus;
class Tab;

#include "annotationmarkwidget.h"
#include "interlinearitemtype.h"

class InterlinearDisplayWidget : public QScrollArea
{
    Q_OBJECT
public:
    InterlinearDisplayWidget(const Tab * tab, Text *text, Project *project, QWidget *parent = 0);
    ~InterlinearDisplayWidget();

    void setPhrasalGloss(int lineNumber, const TextBit & bit);

public slots:
    void scrollToLine(int line);
    void requestLineRefresh( int line );
    void setLayoutFromText();
    void setLines( const QList<int> lines );
    void setFocus( const QList<Focus> & foci );
    void resetGui();

protected:
    void scrollContentsBy ( int dx, int dy );
    void clearWidgetsFromLine(int lineNumber);
    void setLinesToDefault();

private slots:
    void saveText();
    void saveTextVerbose();


    void approveAll(int lineNumber);
    void playSound(int lineNumber);
    void editLine(int lineNumber);

    void approveAll( WordDisplayWidget * wdw );
    void playSound( WordDisplayWidget * wdw );
    void leftGlossItem( WordDisplayWidget * wdw );
    void rightGlossItem( WordDisplayWidget * wdw );

protected:
    const Tab * mTab;
    Text *mText;
    Project *mProject;
    int mCurrentLine;

private:
    void contextMenuEvent ( QContextMenuEvent * event );

    QSpacerItem * mBottomSpacing;

    //! \brief Add the word display widgets for phrase \a i to \a flowLayout
    void addWordWidgets( int i , QLayout * flowLayout );
    WordDisplayWidget* addWordDisplayWidget(GlossItem *item, Phrase *phrase);

protected:
    QHash<int, QLayout*> mLineLayouts;
    QHash<int, InterlinearLineLabel*> mLineLabels;
    QList<int> mLines;
    QList<Focus> mFoci;
    QSet<int> mLineRefreshRequests;

    // WordDisplayWidget objects, keyed to line number
    QMultiHash<int, WordDisplayWidget*> mWordDisplayWidgets;
    QMultiHash<int, LingEdit*> mPhrasalGlossEdits;

    QLayout* addLine(int lineNumber);

    QVBoxLayout *mLayout;

    QList<InterlinearItemType> mPhrasalGlossLines;

    LingEdit* addPhrasalGlossLine(  const TextBit & gloss );

    //! \brief Add the phrasal gloss lines for phrase \a i
    void addPhrasalGlossLines( int i );

    //! \brief Add a line label for phrase \a i
    void addLineLabel( int i , QLayout * flowLayout  );

    void maybeFocus(WordDisplayWidget * wdw);
};

#endif // INTERLINEARDISPLAYWIDGET_H
