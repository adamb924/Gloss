#include "syntacticparsingwidget.h"
#include "ui_syntacticparsingwidget.h"

#include <QGraphicsScene>
#include <QGraphicsLinearLayout>
#include <QGraphicsTextItem>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>

#include "text.h"
#include "tab.h"
#include "project.h"
#include "worddisplaywidget.h"

SyntacticParsingWidget::SyntacticParsingWidget(Text *text,  const Tab * tab, const Project * project, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyntacticParsingWidget),
    mText(text),
    mTab(tab),
    mProject(project)
{
    ui->setupUi(this);

    mScene = new QGraphicsScene;
    ui->graphicsView->setInteractive(true);
    ui->graphicsView->setScene(mScene);
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);

    connect( mScene, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

    setupLayout();
}

SyntacticParsingWidget::~SyntacticParsingWidget()
{
    delete ui;
}

void SyntacticParsingWidget::setupLayout()
{
    /// @todo Support RTL layout
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout;

    for(int i=0; i<mText->phrases()->count(); i++)
    {
        for(int j=0; j<mText->phrases()->at(i)->glossItems()->count(); j++)
        {
            WordDisplayWidget *wdw = new WordDisplayWidget( mText->phrases()->at(i)->glossItemAt(j) , mText->baselineWritingSystem().layoutDirection() == Qt::LeftToRight ? Qt::AlignLeft : Qt::AlignRight, mTab, mProject, 0 );
            QGraphicsWidget *graphicsWdw = mScene->addWidget( wdw );
            layout->addItem( graphicsWdw );
            graphicsWdw->setFlag(QGraphicsItem::ItemIsSelectable, true);
        }
    }

    QGraphicsWidget *form = new QGraphicsWidget;
    form->setLayout(layout);
    mScene->addItem(form);

}

void SyntacticParsingWidget::selectionChanged()
{
    QList<QGraphicsItem*> selectedItems = mScene->selectedItems();

    QSet<QGraphicsItem*> current = selectedItems.toSet();
    QSet<QGraphicsItem*> previous = mPreviouslySelectedItems.toSet();
    QSet<QGraphicsItem*> deselected = previous.subtract(current);
    QSet<QGraphicsItem*> selected = current.subtract(previous);

    QSetIterator<QGraphicsItem*> i(deselected);
    while (i.hasNext())
    {
        QGraphicsProxyWidget *pw = qgraphicsitem_cast<QGraphicsProxyWidget*>( i.next() );
        if( pw != 0 )
        {
            WordDisplayWidget *wdw = qobject_cast<WordDisplayWidget*>(pw->widget());
            if( wdw != 0 )
            {
                wdw->setSelected(false);
            }
        }
    }

    QSetIterator<QGraphicsItem*> j(selected);
    while (j.hasNext())
    {
        QGraphicsProxyWidget *pw = qgraphicsitem_cast<QGraphicsProxyWidget*>( j.next() );
        if( pw != 0 )
        {
            WordDisplayWidget *wdw = qobject_cast<WordDisplayWidget*>(pw->widget());
            if( wdw != 0 )
            {
                wdw->setSelected(true);
            }
        }
    }

    mPreviouslySelectedItems = selectedItems;
}
