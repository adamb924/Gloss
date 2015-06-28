/*!
  \class InterlinearDisplayWidget
  \ingroup Interlinear
  \brief A widget for an interlinear text display. The widget's layout is populated with a series of FlowLayout layouts corresponding to phrases.

  This widget will be contained by TextTabWidget.
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
class PunctuationDisplayWidget;
class GlossItem;
class Phrase;
class Focus;
class Tab;
class Paragraph;
class ParagraphMarkWidget;

#include "annotationmarkwidget.h"
#include "interlinearitemtype.h"

class InterlinearDisplayWidget : public QScrollArea
{
    Q_OBJECT
public:
    enum MouseMode { Normal, ChangeBaseline };

    InterlinearDisplayWidget(const Tab * tab, Text *text, Project *project, QWidget *parent = 0);
    ~InterlinearDisplayWidget();

    void setPhrasalGloss(int lineNumber, const TextBit & bit);

public slots:
    void scrollToLine(int line);
    void requestLineRefresh( int line );
    void setLayoutFromText();
    void setLines( const QList<int> lines );
    void setFocus( const QList<Focus> & foci );

    // Returns the line number of \a wdw. If \a wdw is not a WordDisplayWidget, returns -1. If \a wdw cannot be found, returns -1.
    int lineNumberOfWdw(QWidget *wdw) const;
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

    void moveToNextGlossItem( WordDisplayWidget * wdw );
    void moveToPreviousGlossItem( WordDisplayWidget * wdw );

    void findPreviousWdw(int &lineNumber, int &position);
    void findNextWdw(int &lineNumber, int &position);

    void enterChangeBaselineMode(QAction * action);

    void wdwClicked( WordDisplayWidget * wdw );

protected:
    const Tab * mTab;
    Text *mText;
    Project *mProject;
    InterlinearDisplayWidget::MouseMode mMouseMode;
    int mCurrentLine;
    WritingSystem mChangeWritingSystemTo;

private:
    void contextMenuEvent ( QContextMenuEvent * event );
    void keyPressEvent ( QKeyEvent * event );

    //! \brief Add the word display widgets for phrase \a i to \a flowLayout
    void addWordWidgets(int i , Phrase *phrase, QLayout * flowLayout );
    WordDisplayWidget* addWordDisplayWidget(GlossItem *item, Phrase *phrase);
    PunctuationDisplayWidget* addPunctuationDisplayWidget(GlossItem *item);

protected:
    QHash<int, QLayout*> mLineLayouts;
    QHash<int, QVBoxLayout*> mPhrasalGlossLayouts;

    QHash<int, InterlinearLineLabel*> mLineLabels;
    QList<int> mLines;
    QList<Focus> mFoci;
    QSet<int> mLineRefreshRequests;

    // WordDisplayWidget objects, keyed to line number
    QList< QList<QWidget*> > mWordDisplayWidgets;
    QMultiHash<int, LingEdit*> mPhrasalGlossEdits;
    QMultiHash<int, ParagraphMarkWidget*> mParagraphMarkWidgets;


    QLayout* addLine(int lineNumber);
    QVBoxLayout* addPhrasalGlossLayout(int lineNumber);

    QVBoxLayout *mLayout;

    QList<InterlinearItemType> mPhrasalGlossLines;

    //! \brief Add the phrasal gloss lines for phrase \a i
    void addPhrasalGlossLines(int i, Phrase *phrase,  QVBoxLayout *phrasalGlossLayout);

    //! \brief Add a paragraph marker
    void addParagraphMarker(int lineIndex, const Paragraph *paragraph);

    //! \brief Add a line label for phrase \a i
    void addLineLabel(int i, Phrase *phrase, QLayout * flowLayout  );

    bool maybeFocus(QWidget *wdw);
};

#endif // INTERLINEARDISPLAYWIDGET_H
