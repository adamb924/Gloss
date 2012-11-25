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

void InterlinearDisplayWidget::setText(const QString& text)
{
    if( mText != text )
    {
        mText = text;
        setLayoutFromText();
    }
}

void InterlinearDisplayWidget::setLayoutFromText()
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

    QStringList lines = mText.split(QRegExp("[\\n\\r]+"),QString::SkipEmptyParts);
    for(int i=0; i<lines.count(); i++)
    {
        FlowLayout *flowLayout = new FlowLayout;

        mTextBits << new QList<TextBit*>;

        QStringList words = lines.at(i).split(QRegExp("[ \\t]+"),QString::SkipEmptyParts);
        for(int j=0; j<words.count(); j++)
        {
            *(mTextBits.last()) << new TextBit(words.at(j),mWritingSystem);

            WordDisplayWidget *word = new WordDisplayWidget( mTextBits.last()->last() , mBaselineMode , mProject );
            flowLayout->addWidget(word);
            mWordDisplayWidgets << word;
        }
        mLineLayouts << flowLayout;
        mLayout->addLayout(flowLayout);
    }
    mLayout->addStretch(100);
}

WritingSystem* InterlinearDisplayWidget::writingSystem() const
{
    return mWritingSystem;
}

void InterlinearDisplayWidget::setWritingSystem(WritingSystem *ws)
{
    mWritingSystem = ws;
}
