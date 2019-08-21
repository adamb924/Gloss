/*!
  \class LexiconLineForm
  \ingroup MorphologicalAnalysis
  \brief A widget holding a line of text for a lexical entry, along with a question-mark button so the user can auto-fill a guess. See lexiconform.ui. It is instantiated in CreateLexicalEntryDialog only.
*/

#ifndef LEXICONLINEFORM_H
#define LEXICONLINEFORM_H

#include <QWidget>

#include "textbit.h"

namespace Ui {
class LexiconLineForm;
}

class LexiconLineForm : public QWidget
{
    Q_OBJECT

public:
    LexiconLineForm(const TextBit & guess, bool autoFill, bool hideGuessButton, QWidget *parent = nullptr);
    ~LexiconLineForm();

    TextBit textBit() const;

private slots:
    void fillInGuess();

private:
    Ui::LexiconLineForm *ui;
    TextBit mGuess;
};

#endif // LEXICONLINEFORM_H
