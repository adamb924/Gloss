#include "interlineardisplaywidget.h"

#include <QtDebug>
#include <QtGui>
#include <QStringList>

#include "flowlayout.h"
#include "textbit.h"
#include "worddisplaywidget.h"

InterlinearDisplayWidget::InterlinearDisplayWidget(WritingSystem *baselineWs, Project::BaselineMode baselineMode, Project *project, QWidget *parent) :
    QWidget(parent)
{
    mProject = project;

    mWritingSystem = baselineWs;

    mBaselineMode = baselineMode;

    mLayout = new QVBoxLayout(this);
    setLayout(mLayout);
}

InterlinearDisplayWidget::~InterlinearDisplayWidget()
{
    clearData();
}

void InterlinearDisplayWidget::setText(const QString& text)
{
    if( mText != text )
    {
        mText = text;
        setLayoutFromText();
    }
}

void InterlinearDisplayWidget::clearData()
{
    qDeleteAll(mWordDisplayWidgets);
    mWordDisplayWidgets.clear();
    qDeleteAll(mLineLayouts);
    mLineLayouts.clear();

    QList<TextBit*> *line;
    foreach( line , mTextBits )
    {
        qDeleteAll(*line);
        line->clear();
    }
    qDeleteAll(mTextBits);
    mTextBits.clear();

    // does this belong here or would that defeat the purpose?
    mConcordance.clear();
}

void InterlinearDisplayWidget::setLayoutFromText()
{
    clearData();

    QStringList lines = mText.split(QRegExp("[\\n\\r]+"),QString::SkipEmptyParts);
    for(int i=0; i<lines.count(); i++)
    {
        FlowLayout *flowLayout = new FlowLayout;

        mTextBits << new QList<TextBit*>;

        QStringList words = lines.at(i).split(QRegExp("[ \\t]+"),QString::SkipEmptyParts);
        for(int j=0; j<words.count(); j++)
        {
//            mConcordance.insert(mTextBits.last()->last())

            *(mTextBits.last()) << new TextBit(words.at(j),mWritingSystem);

            // once this object is constructed, it will have an id
            WordDisplayWidget *wdw = new WordDisplayWidget( mTextBits.last()->last() , mBaselineMode , mProject );
            connect(wdw,SIGNAL(idChanged(WordDisplayWidget*,qlonglong,qlonglong)),this,SLOT(updateConcordance(WordDisplayWidget*,qlonglong,qlonglong)));
            // this line is necessary because the signal from the constructor is emitted before the connection is made
            mConcordance.insert( wdw->textBit()->id() , wdw );

            // update concordant words
            connect(wdw,SIGNAL(glossChanged(TextBit)), this, SLOT(updateGloss(TextBit)));
            connect(wdw,SIGNAL(orthographyChanged(TextBit)), this, SLOT(updateOrthography(TextBit)));
            connect(wdw,SIGNAL(transcriptionChanged(TextBit)), this, SLOT(updateTranscription(TextBit)));

            flowLayout->addWidget(wdw);
            mWordDisplayWidgets << wdw;
        }

        mLineLayouts << flowLayout;
        mLayout->addLayout(flowLayout);
    }
    mLayout->addStretch(100);

//    qDebug() << mConcordance;
}

WritingSystem* InterlinearDisplayWidget::writingSystem() const
{
    return mWritingSystem;
}

void InterlinearDisplayWidget::setWritingSystem(WritingSystem *ws)
{
    mWritingSystem = ws;
}

void InterlinearDisplayWidget::updateConcordance( WordDisplayWidget *w, qlonglong oldId, qlonglong newId )
{
    mConcordance.remove(oldId,w);
    mConcordance.insert(newId,w);
}

void InterlinearDisplayWidget::updateGloss( const TextBit & bit )
{
    WordDisplayWidget* wdw;
    QList<WordDisplayWidget*> wdwList = mConcordance.values(bit.id());
    foreach(wdw, wdwList)
        wdw->updateEdit(bit,Project::Gloss);
}

void InterlinearDisplayWidget::updateTranscription( const TextBit & bit )
{
    WordDisplayWidget* wdw;
    QList<WordDisplayWidget*> wdwList = mConcordance.values(bit.id());
    foreach(wdw, wdwList)
        wdw->updateEdit(bit,Project::Transcription);
}

void InterlinearDisplayWidget::updateOrthography( const TextBit & bit )
{
    WordDisplayWidget* wdw;
    QList<WordDisplayWidget*> wdwList = mConcordance.values(bit.id());
    foreach(wdw, wdwList)
        wdw->updateEdit(bit,Project::Orthography);
}

void InterlinearDisplayWidget::updateMorphologicalAnalysis( const TextBit & bit , const QString & splitString )
{

}
