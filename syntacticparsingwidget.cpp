#include "syntacticparsingwidget.h"
#include "ui_syntacticparsingwidget.h"

#include <QGraphicsScene>
#include <QGraphicsLinearLayout>
#include <QGraphicsTextItem>
#include <QGraphicsWidget>
#include <QGraphicsProxyWidget>
#include <QInputDialog>
#include <QMessageBox>
#include <QMenu>

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
#include "createsyntacticanalysisdialog.h"
#include "databaseadapter.h"
#include "linenumbergraphicsitem.h"

#include "ui_createsyntacticanalysisdialog.h"

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

    setupBaseline();

    connect( ui->comboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(analysisSelectionChanged(QString)) );

    ui->comboBox->insertItems(0, text->syntacticAnalyses()->keys() );

    connect( ui->addButton, SIGNAL(clicked()), this, SLOT(newAnalysis()));
    connect( ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteAnalysis()) );
    connect( ui->editButton, SIGNAL(clicked()), this, SLOT(editAnalysis()) );

    ui->editButton->setEnabled( ui->comboBox->count() > 0 );
}

SyntacticParsingWidget::~SyntacticParsingWidget()
{
    delete ui;
}

void SyntacticParsingWidget::setupBaseline()
{
    if( mAnalysis == 0 ) return;

    mScene->clear();
    mGraphicsItemAllomorphHash.clear();
    qreal x = mInterWordDistance;
    for(int i=0; i<mText->phrases()->count(); i++) /// for each phrase
    {
        LineNumberGraphicsItem *lineNumberItem = new LineNumberGraphicsItem(i+1);
        lineNumberItem->setPos(x,0);
        mScene->addItem(lineNumberItem);
        x += lineNumberItem->boundingRect().width() + mInterWordDistance;
        connect(lineNumberItem, SIGNAL(requestPlayLine(int)), mText, SLOT(playSoundForLine(int)) );

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
                        SyntacticAnalysisElement * element = mAnalysis->elementFromGuid( ma->allomorph(m)->guid() );
                        MorphemeGraphicsItem *item = new MorphemeGraphicsItem( ma->allomorph(m)->textBitForConcatenation(), element );
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
        addElementToScene( mAnalysis->elements()->at(i) );
    }
}

QGraphicsItem *SyntacticParsingWidget::addElementToScene(SyntacticAnalysisElement *element)
{
    QList<QGraphicsItem*> daughters;
    for(int i=0; i<element->elements()->count(); i++)
    {
        if( element->elements()->at(i)->isTerminal() ) /// terminal
        {
            daughters << mGraphicsItemAllomorphHash.value(element->elements()->at(i)->allomorph());
        }
        else /// constituent
        {
            daughters << addElementToScene( element->elements()->at(i) );
        }
    }
    ConstituentGraphicsItem * item = new ConstituentGraphicsItem( element->label(), daughters, element );
    connect(item, SIGNAL(reparentElement(SyntacticAnalysisElement*,SyntacticAnalysisElement*)), mAnalysis, SLOT(reparentElement(SyntacticAnalysisElement*,SyntacticAnalysisElement*)) );
    connect(item, SIGNAL(reparentElement(SyntacticAnalysisElement*,SyntacticAnalysisElement*)), this, SLOT(redrawSyntacticAnnotations()) );
    mConstiuencyItems << (QGraphicsItem*)item;
    mScene->addItem(item);
    return item;
}

void SyntacticParsingWidget::keyReleaseEvent(QKeyEvent *event)
{
    if( mAnalysis == 0) return;

    if( event->key() == Qt::Key_Delete )
    {
        removeConstituent();
    }
    else if ( event->key() == Qt::Key_Insert )
    {
        createConstituent();
    }

    QKeySequence key = QKeySequence( event->modifiers() | event->key() );
    if( mAnalysis->closedVocabulary() )
    {
        createConstituent( mProject->dbAdapter()->syntacticType( key ) );
    }
    else
    {
        if( event->key() == Qt::Key_A )
        {
            createConstituent();
        }
        else if ( event->key() == Qt::Key_X )
        {
            removeConstituent();
        }
    }
}

void SyntacticParsingWidget::createConstituent(const SyntacticType & type)
{
    if( mAnalysis == 0 ) return;

    QList<SyntacticAnalysisElement *> elements = selectedElements();
    if( !elements.isEmpty() )
    {
        if( type.isNull() )
        {
            bool ok;
            QString label = QInputDialog::getText(this, tr("New constituent"),
                                                 tr("Label:"), QLineEdit::Normal,tr(""), &ok);
            if (ok && !label.isEmpty())
            {
                mAnalysis->createConstituent( label , elements );
                redrawSyntacticAnnotations();
            }
        }
        else
        {
            mAnalysis->createConstituent( type.abbreviation() , elements );
            redrawSyntacticAnnotations();
        }
    }
    mScene->clearSelection();
}

void SyntacticParsingWidget::removeConstituent()
{
    QList<SyntacticAnalysisElement *> elements = selectedElements();
    foreach( SyntacticAnalysisElement * e, elements)
    {
        mAnalysis->removeElement( e );
    }
    redrawSyntacticAnnotations();
}

QList<SyntacticAnalysisElement *> SyntacticParsingWidget::selectedElements()
{
    QList<SyntacticAnalysisElement *> elements;
    QListIterator<QGraphicsItem*> iter(mScene->selectedItems());
    while( iter.hasNext() )
    {
        QGraphicsItem * item = iter.next();
        MorphemeGraphicsItem * mgi = qgraphicsitem_cast<MorphemeGraphicsItem*>(item);

        if( mgi != 0 ) /// then it's a morpheme graphics item (with an associated element)
        {
            elements << mgi->element();
        }
        else
        {
            ConstituentGraphicsItem * con = qgraphicsitem_cast<ConstituentGraphicsItem*>(item);
            if( con != 0 )
            {
                elements << con->element();
            }
        }
    }
    return elements;
}

void SyntacticParsingWidget::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    menu.addAction(tr("Save this text"), this, SLOT(saveText()), QKeySequence("Ctrl+Shift+S"));
    menu.addAction(tr("Save this text, verbose output"), this, SLOT(saveTextVerbose()) );
    menu.exec(event->globalPos());
}

void SyntacticParsingWidget::analysisSelectionChanged(const QString &newSelection)
{
    if( mText->syntacticAnalyses()->contains(newSelection) )
    {
        mAnalysis = mText->syntacticAnalyses()->value(newSelection);
        setupBaseline();
        redrawSyntacticAnnotations();
    }
}

void SyntacticParsingWidget::newAnalysis()
{
    CreateSyntacticAnalysisDialog dlg(mProject->dbAdapter()->writingSystems());
    if( dlg.exec() )
    {
        mAnalysis = new SyntacticAnalysis(dlg.name(), dlg.writingSystem(), mText, dlg.closedVocabulary() );
        mText->syntacticAnalyses()->insert( dlg.name() , mAnalysis );
        ui->comboBox->insertItem(0, dlg.name());
        ui->comboBox->setCurrentIndex(0);
        redrawSyntacticAnnotations();
        ui->editButton->setEnabled( true );
    }
}

void SyntacticParsingWidget::editAnalysis()
{
    CreateSyntacticAnalysisDialog dlg(mAnalysis, mProject->dbAdapter()->writingSystems());
    if( dlg.exec() )
    {
        mAnalysis->setName( dlg.name() );
        mAnalysis->setWritingSystem( dlg.writingSystem() );
        mAnalysis->setClosedVocabulary( dlg.closedVocabulary() );
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
        ui->editButton->setEnabled( ui->comboBox->count() > 0 );
    }
}

void SyntacticParsingWidget::saveText()
{
    mText->saveText(false, true, true);
}

void SyntacticParsingWidget::saveTextVerbose()
{
    mText->saveText(true, true, true);
}


