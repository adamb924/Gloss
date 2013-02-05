/*!
  \class InterlinearDisplayWidget
  \ingroup GUI
  \brief A widget for an interlinear text display. The widget's layout is populated with a series of FlowLayout layouts corresponding to phrases. The widget also keeps a concordance of LingEdit objects, to update their fields when the values are changed.
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

#include "interlinearitemtype.h"

class InterlinearDisplayWidget : public QScrollArea
{
    Q_OBJECT
public:
    InterlinearDisplayWidget(const QList<InterlinearItemType> & interlinearDisplayLines, const QList<InterlinearItemType> & phrasalGlossLines, Text *text, Project *project, QWidget *parent = 0);
    ~InterlinearDisplayWidget();


signals:
    //! \brief Emitted whenever the top line number of the widget is changed. \a line is 0-indexed.
    // TODO emit this at some point
    void lineNumberChanged(int line);

public slots:
    void scrollToLine(int line);
    void requestLineRefresh( int line );
    void setLayoutFromText();
    void setLines( const QList<int> lines );
    void setFocus( const QList<Focus> & foci );

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

protected:
    Text *mText;
    Project *mProject;
    int mCurrentLine;

private:
    void contextMenuEvent ( QContextMenuEvent * event );


    //! \brief Add the word display widgets for phrase \a i to \a flowLayout
    void addWordWidgets( int i , QLayout * flowLayout );
    WordDisplayWidget* addWordDisplayWidget(GlossItem *item, Phrase *phrase);

protected:
    QHash<int, QLayout*> mLineLayouts;
    QHash<int, InterlinearLineLabel*> mLineLabels;
    QList<int> mLines;
    QList<Focus> mFoci;

    QMultiHash<int, QWidget*> mWordDisplayWidgets;

    QLayout* addLine(int lineNumber);

    QVBoxLayout *mLayout;

    QList<InterlinearItemType> mInterlinearDisplayLines;
    QList<InterlinearItemType> mPhrasalGlossLines;

    LingEdit* addPhrasalGlossLine(  const TextBit & gloss );

    QSet<int> mLineRefreshRequests;

    //! \brief Add the phrasal gloss lines for phrase \a i
    void addPhrasalGlossLines( int i );

    //! \brief Add a line label for phrase \a i
    void addLineLabel( int i , QLayout * flowLayout  );
};

#endif // INTERLINEARDISPLAYWIDGET_H
