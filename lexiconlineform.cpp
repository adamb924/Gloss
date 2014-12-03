#include "lexiconlineform.h"
#include "ui_lexiconlineform.h"

#include <QtDebug>

LexiconLineForm::LexiconLineForm(const TextBit & guess, bool autoFill, bool hideGuessButton, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LexiconLineForm),
    mGuess(guess)
{
    ui->setupUi(this);
    ui->edit->setWritingSystem(guess.writingSystem());

    if(autoFill)
    {
        fillInGuess();
    }

    if(hideGuessButton)
    {
        ui->guessButton->hide();
    }

    connect( ui->guessButton, SIGNAL(clicked()), this, SLOT(fillInGuess()) );
}

LexiconLineForm::~LexiconLineForm()
{
    delete ui;
}

TextBit LexiconLineForm::textBit() const
{
    return ui->edit->textBit();
}

void LexiconLineForm::fillInGuess()
{
    if( !mGuess.text().isEmpty() )
    {
        ui->edit->setTextBit( mGuess );
        ui->edit->setFocus( Qt::OtherFocusReason );
        ui->edit->setCursorPosition( ui->edit->text().length() );
    }
}
