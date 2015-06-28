#include "paragraphmarkwidget.h"

#include <QtWidgets>

#include "paragraph.h"
#include "immutablelabel.h"
#include "generictextinputdialog.h"

ParagraphMarkWidget::ParagraphMarkWidget(Paragraph * paragraph, QWidget *parent) :
    QWidget(parent),
    mParagraph(paragraph),
    mHeaderLabel(0)
{
    setLayoutDirection(mParagraph->header().writingSystem().layoutDirection());

    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget( new QLabel(tr("¶")) );
    layout->addSpacing(10);

    mHeaderLabel = new ImmutableLabel(mParagraph->header(), false, this);
    layout->addWidget( mHeaderLabel );
    connect( mHeaderLabel, SIGNAL(doubleClick(TextBit&)), this, SLOT(editHeader()) );

    layout->addStretch(1);
    setLayout(layout);
}

void ParagraphMarkWidget::editHeader()
{
    GenericTextInputDialog dlg(mParagraph->header());
    if( dlg.exec() )
    {
        mHeaderLabel->setTextBit(dlg.textBit());
        mParagraph->setHeader(dlg.textBit());
    }
}

void ParagraphMarkWidget::removeParagraphDivision()
{
    emit removeParagraphDivision(mParagraph);
}

void ParagraphMarkWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    editHeader();
}

void ParagraphMarkWidget::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event);
    QMenu *menu = new QMenu(this);
    menu->addAction(tr("Remove paragraph division"), this, SLOT(removeParagraphDivision()));
    menu->exec(event->globalPos());
}
