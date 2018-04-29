/*!
  \class InterlinearLineLabel
  \ingroup Interlinear
  \brief This is a QLabel subclass for displaying the line numbers in an interlinear text. It adds double-click sensitivity and a context menu. It is instantiated in InterlinearDisplayWidget.
*/

#ifndef INTERLINEARLINELABEL_H
#define INTERLINEARLINELABEL_H

#include <QLabel>

class Phrase;
class Text;

class InterlinearLineLabel : public QLabel
{
    Q_OBJECT
public:
    InterlinearLineLabel(Text * text, Phrase * phrase, int lineNumber, QWidget *parent = 0);

private:
    void contextMenuEvent ( QContextMenuEvent * event );
    void mouseDoubleClickEvent ( QMouseEvent * event );

    Text * mText;
    Phrase * mPhrase;
    int mLineNumber;

private slots:
    void approveAll();
    void playSound();
    void editBaselineText();
    void removePhrase();
    void newParagraphAt();
    void copyBaselineTextToClipboard();
    void mergePhraseWithPrevious();
};

#endif // INTERLINEARLINELABEL_H
