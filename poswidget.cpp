#include "poswidget.h"
#include "ui_poswidget.h"

#include <QPushButton>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>

#include <QtDebug>

#include "flowlayout.h"

PosWidget::PosWidget(const GlossItem *glossItem, const QHash<QString, PartOfSpeech> &partsOfSpeech, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PosWidget),
    mGlossItem(glossItem)
{
    ui->setupUi(this);

    FlowLayout *flowLayout = new FlowLayout( Qt::LeftToRight, this );

    QButtonGroup * buttonGroup = new QButtonGroup(this);

    QPushButton * nullButton = new QPushButton( tr("—") );
    nullButton->setCheckable(true);
    buttonGroup->addButton(nullButton);
    nullButton->setChecked(true);
    flowLayout->addWidget( nullButton );

    QHashIterator<QString, PartOfSpeech> iter(partsOfSpeech);
    while(iter.hasNext())
    {
        iter.next();

        QPushButton * button = new QPushButton( iter.key() );
        button->setCheckable(true);
        buttonGroup->addButton(button);
        flowLayout->addWidget( button );

        if( glossItem->pos() == iter.value() )
        {
            button->setChecked(true);
        }
    }

    setLayout(flowLayout);
/*
    QGroupBox *groupBox = new QGroupBox();
    groupBox->setLayout(flowLayout);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(groupBox);
    setLayout(layout);
*/
}

PosWidget::~PosWidget()
{
    delete ui;
}
