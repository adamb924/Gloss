#include "interlinearlinelabel.h"

#include <QApplication>
#include <QClipboard>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMessageBox>

#include "phrase.h"
#include "text.h"
#include "generictextinputdialog.h"

InterlinearLineLabel::InterlinearLineLabel(Text * text, Phrase * phrase, int lineNumber, QWidget *parent) :
    QLabel(parent),
    mText(text),
    mPhrase(phrase),
    mLineNumber(lineNumber)
{
    setText(  QString("%1").arg(lineNumber+1) );
    setToolTip( mPhrase->interval()->summaryString() );
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(30, 30);
}

void InterlinearLineLabel::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    menu.addAction(tr("Approve all"), this, SLOT(approveAll()) );
    menu.addAction(tr("Edit baseline text"), this, SLOT(editBaselineText()) );
    menu.addAction(tr("Copy baseline text"), this, SLOT(copyBaselineTextToClipboard()));

    // this seems like the most intuitive thing to me, from a user interface standpoint
    QAction *playSound = menu.addAction(tr("Play sound"), this, SLOT(playSound()) );
    playSound->setEnabled( mPhrase->interval()->isValid() );

    menu.addAction(tr("New paragraph here"), this, SLOT(newParagraphAt()) );

    menu.addAction(tr("Remove line"), this, SLOT(removePhrase()) );

    menu.exec(event->globalPos());
}

void InterlinearLineLabel::mouseDoubleClickEvent ( QMouseEvent * event )
{
    Q_UNUSED(event);
    playSound();
}

void InterlinearLineLabel::approveAll()
{
    mPhrase->setApproval(GlossItem::Approved);
}

void InterlinearLineLabel::playSound()
{
    mText->playSoundForLine(mLineNumber);
}

void InterlinearLineLabel::editBaselineText()
{
    // Launch a dialog requesting input
    GenericTextInputDialog dialog( TextBit( mPhrase->equivalentBaselineLineText(), mText->baselineWritingSystem() ) , this);
    dialog.setWindowTitle(tr("Edit baseline text - Line %1").arg(mLineNumber+1));
    if( dialog.exec() == QDialog::Accepted )
    {
        mPhrase->setBaselineText( dialog.textBit() );
    }
}

void InterlinearLineLabel::removePhrase()
{
    if( QMessageBox::Yes == QMessageBox::question(this, tr("Confirm deletion"), tr("Are you sure you want to remove this line? This cannot be undone."), QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) )
        mText->removePhrase(mLineNumber);
}

void InterlinearLineLabel::newParagraphAt()
{
    mText->addParagraphDivision(mLineNumber);
}

void InterlinearLineLabel::copyBaselineTextToClipboard()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( mPhrase->equivalentBaselineLineText() );
}
