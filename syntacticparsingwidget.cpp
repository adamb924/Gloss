#include "syntacticparsingwidget.h"
#include "ui_syntacticparsingwidget.h"

#include <QGraphicsScene>
#include <QGraphicsLinearLayout>
#include <QGraphicsTextItem>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QInputDialog>
#include <QMessageBox>

#include "text.h"
#include "tab.h"
#include "project.h"
#include "worddisplaywidget.h"
#include "morphemegraphicsitem.h"
#include "allomorph.h"
#include "textbit.h"
#include "syntacticanalysis.h"
#include "constituentgraphicsitem.h"
#include "syntacticanalysiselement.h"

SyntacticParsingWidget::SyntacticParsingWidget(Text *text,  const Tab * tab, const Project * project, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SyntacticParsingWidget),
    mText(text),
    mTab(tab),
    mProject(project),
    mAnalysis(0),
    mInterMorphemeDistance(5),
    mInterWordDistance(15),
    mVerticalDistance(5)
{
    ui->setupUi(this);

    mScene = new QGraphicsScene;
    ui->graphicsView->setInteractive(true);
    ui->graphicsView->setScene(mScene);
    ui->graphicsView->setDragMode(QGraphicsView::RubberBandDrag);

    connect( ui->comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(analysisSelectionChanged(QString)) );

    ui->comboBox->insertItems(0, text->syntacticAnalyses()->keys() );

    connect( ui->addButton, SIGNAL(clicked()), this, SLOT(newAnalysis()));
    connect( ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteAnalysis()) );

    setupBaseline();
}

SyntacticParsingWidget::~SyntacticParsingWidget()
{
    delete ui;
}

void SyntacticParsingWidget::setupBaseline()
{
    mGraphicsItemAllomorphHash.clear();
    qreal x = mInterWordDistance;
    for(int i=0; i<mText->phrases()->count(); i++) /// for each phrase
    {
        for(int j=0; j<mText->phrases()->at(i)->glossItems()->count(); j++) /// for each gloss item
        {
            int longestLine = 0;
            qreal y=0;

            GlossItem *glossItem = mText->phrases()->at(i)->glossItems()->at(j);
            InterlinearItemTypeList *lines = mTab->interlinearLines().value( glossItem->baselineWritingSystem() );
            for(int k=0; k<lines->count(); k++) /// for each interlinear line
            {
                qreal lineLength = 0;
                qreal lineHeight = 0;
                if( lines->at(k).type() == InterlinearItemType::Analysis )
                {
                    MorphologicalAnalysis *ma = glossItem->morphologicalAnalysis( lines->at(k).writingSystem() );
                    for(int m=0; m<ma->allomorphCount(); m++)
                    {
                        MorphemeGraphicsItem *item = new MorphemeGraphicsItem( ma->allomorph(m) );
                        item->setPos(x + lineLength, y);
                        mScene->addItem(item);
                        lineLength += item->boundingRect().width();
                        if( m < ma->allomorphCount()-1 )
                        {
                            lineLength += mInterMorphemeDistance;
                        }
                        lineHeight = item->boundingRect().height();

                        mGraphicsItemAllomorphHash.insert(ma->allomorph(m), item);
                    }
                }
                else if ( lines->at(k).type() == InterlinearItemType::ImmutableText )
                {
                    QGraphicsSimpleTextItem *item = new QGraphicsSimpleTextItem( glossItem->textForm( lines->at(k).writingSystem() ).text() );
                    item->setFont( lines->at(k).writingSystem().font() );
                    item->setPos(x, y);
                    mScene->addItem(item);
                    lineHeight = item->boundingRect().height();
                    lineLength = item->boundingRect().width();
                }
                else if ( lines->at(k).type() == InterlinearItemType::ImmutableGloss )
                {
                    QGraphicsSimpleTextItem *item = new QGraphicsSimpleTextItem( glossItem->gloss( lines->at(k).writingSystem() ).text() );
                    item->setFont( lines->at(k).writingSystem().font() );
                    item->setPos(x, y);
                    mScene->addItem(item);
                    lineHeight = item->boundingRect().height();
                    lineLength = item->boundingRect().width();
                }
                longestLine = lineLength > longestLine ? lineLength : longestLine;
                y += lineHeight + mVerticalDistance;
            } /// for each interlinear line

            x += longestLine + mInterWordDistance;
        } /// for each gloss item
    } /// for each phrase
}

void SyntacticParsingWidget::redrawSyntacticAnnotations()
{
    if( mAnalysis == 0 ) return;
    qDeleteAll(mConstiuencyItems);
    mConstiuencyItems.clear();

    for(int i=0; i < mAnalysis->elements()->count(); i++ ) // for each element of the analysis
    {
        QList<QGraphicsItem*> daughters;
        for(int j=0; j < mAnalysis->elements()->at(i)->elements()->count(); j++ )
        {
            if( mAnalysis->elements()->at(i)->elements()->at(j)->allomorph() != 0 )
            {
                daughters << mGraphicsItemAllomorphHash.value(mAnalysis->elements()->at(i)->elements()->at(j)->allomorph());
            }
        }
        QGraphicsItem * item = new ConstituentGraphicsItem( mAnalysis->elements()->at(i)->label(), daughters );
        mScene->addItem(item);
        mConstiuencyItems << item;
    }
}

void SyntacticParsingWidget::createConstituent()
{
    if( mAnalysis == 0 ) return;

    QList<SyntacticAnalysisElement *> terminals;
    QListIterator<const Allomorph*> iter(selectedAllmorphs());

    if( iter.hasNext() ) /// i.e., if the selection is not empty
    {
        bool ok;
        QString label = QInputDialog::getText(this, tr("New constituent"),
                                             tr("Label:"), QLineEdit::Normal,tr(""), &ok);
        if (ok && !label.isEmpty())
        {
            while( iter.hasNext() )
            {
                terminals << new SyntacticAnalysisElement(iter.next());
            }
            mAnalysis->createConstituent( label , terminals );
            redrawSyntacticAnnotations();
        }
    }

    mScene->clearSelection();
}

QList<const Allomorph *> SyntacticParsingWidget::selectedAllmorphs()
{
    QList<const Allomorph *> allomorphs;
    QListIterator<QGraphicsItem*> iter(mScene->selectedItems());
    while( iter.hasNext() )
    {
        MorphemeGraphicsItem * item = qgraphicsitem_cast<MorphemeGraphicsItem*>(iter.next());
        if( item != 0 ) /// should never happen since only MorphemeGraphicsItem objects are selectable, but just in case...
        {
            allomorphs << item->allomorph();
        }
    }
    return allomorphs;
}

void SyntacticParsingWidget::keyReleaseEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_A )
    {
        createConstituent();
    }
}

void SyntacticParsingWidget::analysisSelectionChanged(const QString &newSelection)
{
    if( mText->syntacticAnalyses()->contains(newSelection) )
    {
        mAnalysis = mText->syntacticAnalyses()->value(newSelection);
        redrawSyntacticAnnotations();
    }
}

void SyntacticParsingWidget::newAnalysis()
{
    bool ok;
    QString name = QInputDialog::getText(this, tr("Create a new syntactical analysis"),
                                         tr("Name:"), QLineEdit::Normal,tr(""), &ok);
    if (ok && !name.isEmpty())
    {
        mAnalysis = new SyntacticAnalysis(name);
        mText->syntacticAnalyses()->insert( name , mAnalysis );
        ui->comboBox->insertItem(0, name);
        ui->comboBox->setCurrentIndex(0);
        redrawSyntacticAnnotations();
    }
}

void SyntacticParsingWidget::deleteAnalysis()
{
    QString name = ui->comboBox->currentText();
    if( QMessageBox::question(this, tr("Delete this syntactical analysis"), tr("Are you sure you want to delete %1?").arg(name) ) == QMessageBox::Yes )
    {
        delete mText->syntacticAnalyses()->take(name);
        ui->comboBox->removeItem( ui->comboBox->currentIndex() );
    }
}


