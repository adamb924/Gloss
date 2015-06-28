#include "paragraphmarkwidget.h"

#include <QtWidgets>

#include "immutablelabel.h"
#include "generictextinputdialog.h"

ParagraphMarkWidget::ParagraphMarkWidget(const TextBit & header, QWidget *parent) :
    QWidget(parent),
    mHeader(header),
    mHeaderLabel(0)
{
    setLayoutDirection(mHeader.writingSystem().layoutDirection());

    QHBoxLayout * layout = new QHBoxLayout;
    layout->addWidget( new QLabel(tr("¶")) );
    layout->addSpacing(10);

    mHeaderLabel = new ImmutableLabel(mHeader, false, this);
    layout->addWidget( mHeaderLabel );
    connect( mHeaderLabel, SIGNAL(doubleClick(TextBit&)), this, SLOT(editHeader()) );

    layout->addStretch(1);
    setLayout(layout);
}

void ParagraphMarkWidget::editHeader()
{
    GenericTextInputDialog dlg(mHeader);
    if( dlg.exec() )
    {
        mHeader = dlg.textBit();
        mHeaderLabel->setTextBit(mHeader);
        emit headerChanged(mHeader);
    }
}

void ParagraphMarkWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    editHeader();
}
