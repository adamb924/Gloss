#include "punctuationdisplaywidget.h"

#include <QMenu>
#include <QContextMenuEvent>

#include "generictextinputdialog.h"
#include "databaseadapter.h"

PunctuationDisplayWidget::PunctuationDisplayWidget(GlossItem *glossItem, const DatabaseAdapter *dbAdapter, QWidget *parent) : ImmutableLabel( glossItem->baselineText(), false, parent ) , mGlossItem(glossItem) , mDbAdapter(dbAdapter)
{
    connect(glossItem, SIGNAL(fieldsChanged()), this, SLOT(updateDisplay()) );
}

void PunctuationDisplayWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Edit baseline text"), this, SLOT(editBaselineText()));
    menu->addAction(tr("Edit Text Form %1 (%2)").arg( mGlossItem->baselineText().id() ).arg( mGlossItem->baselineText().text() ), this, SLOT(editBaselineTextForm()));
    menu->addAction(tr("Begin new line here"), this, SLOT(beginNewLineHere()));
    menu->addAction(tr("Merge this line with previous"), this, SLOT(noNewLineHere()));
    menu->exec(event->globalPos());
}

void PunctuationDisplayWidget::updateDisplay()
{
    setTextBit( mGlossItem->baselineText() );
}

void PunctuationDisplayWidget::beginNewLineHere()
{
    emit requestNewLineFromHere( mGlossItem );
}

void PunctuationDisplayWidget::noNewLineHere()
{
    emit requestNoLineFromHere( mGlossItem );
}

void PunctuationDisplayWidget::editBaselineTextForm()
{
    GenericTextInputDialog dialog( mGlossItem->baselineText() , this );
    dialog.setWindowTitle(tr("Edit Text Form %1").arg( mGlossItem->baselineText().id() ));
    if( dialog.exec() == QDialog::Accepted )
    {
        mDbAdapter->updateTextForm( dialog.textBit() );
        mGlossItem->setTextFormText( dialog.textBit() );
    }
}

void PunctuationDisplayWidget::editBaselineText()
{
    GenericTextInputDialog dialog( mGlossItem->baselineText() , this );
    dialog.setWindowTitle(tr("Edit baseline text (%1)").arg(mGlossItem->baselineText().writingSystem().name()));
    if( dialog.exec() == QDialog::Accepted )
    {
        mGlossItem->resetBaselineText( TextBit( dialog.text().trimmed() , mGlossItem->baselineWritingSystem() ) );
    }
}
