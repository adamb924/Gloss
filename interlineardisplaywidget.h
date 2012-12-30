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

#include "interlinearitemtype.h"

class InterlinearDisplayWidget : public QScrollArea
{
    Q_OBJECT
public:
    InterlinearDisplayWidget(Text *text, Project *project, QWidget *parent = 0);
    ~InterlinearDisplayWidget();

signals:
    //! \brief Emitted whenever the top line number of the widget is changed. \a line is 0-indexed.
    // TODO emit this at some point
    void lineNumberChanged(int line);

public slots:
    void scrollToLine(int line);

protected:
    void scrollContentsBy ( int dx, int dy );

private slots:

    void saveText();

protected:
    Text *mText;
    Project *mProject;
    int mCurrentLine;

private:
    void contextMenuEvent ( QContextMenuEvent * event );

    //! \brief Removes and deletes all widgets from the given \a layout
    virtual void clearWidgets(QLayout * layout) = 0;

    virtual void setLayoutFromText() = 0;

    //! \brief Add the word display widgets for phrase \a i to \a flowLayout
    virtual void addWordWidgets( int i , QLayout * flowLayout ) = 0;

protected:
    QList<QLayout*> mLineLayouts;

    QSet<QWidget*> mWordDisplayWidgets; // change to generic name

    QLayout* addLine();

    QVBoxLayout *mLayout;

    void clearData();

    QList<InterlinearItemType> mInterlinearDisplayLines;
    QList<InterlinearItemType> mPhrasalGlossLines;

    LingEdit* addPhrasalGlossLine(  const TextBit & gloss );

    QList<LingEdit*> mPhrasalGlossEdits;

    //! \brief Add the phrasal gloss lines for phrase \a i
    void addPhrasalGlossLines( int i );

    //! \brief Add a line label for phrase \a i
    void addLineLabel( int i , QLayout * flowLayout  );
};

#endif // INTERLINEARDISPLAYWIDGET_H
