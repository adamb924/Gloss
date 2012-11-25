#include "abstractworddisplaywidget.h"
#include "textbit.h"
#include "project.h"
#include "lingedit.h"
#include "glossline.h"

#include <QtGui>
#include <QtDebug>

AbstractWordDisplayWidget::AbstractWordDisplayWidget( TextBit *bit, Project *project)
{
    mTextBit = bit;
    mProject = project;

    mGlossLines = mProject->glossLines();

    setupLayout();

    setMinimumSize(128,128);

//    qDebug() << "Size Policy" << this->sizePolicy();

    setSizePolicy(QSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed));

//    qDebug() << mLayout->geometry();
//    qDebug() << this->geometry();

    fillData();
}

void AbstractWordDisplayWidget::setupLayout()
{
    mLayout = new QVBoxLayout;
    setLayout(mLayout);

    mEdits.clear();

    mLayout->addWidget(new QLabel(mTextBit->text()));
    for(int i=0; i<mGlossLines.count(); i++)
    {
        //        qDebug() << lines.at(i).mType << lines.at(i).mWritingSystem.keyboardCommand();
        qDebug() << "Creating edit for id:" << mTextBit->id();
        LingEdit *edit = new LingEdit( TextBit( QString("") , mGlossLines.at(i).writingSystem(), mTextBit->id() ), this);
        mLayout->addWidget(edit);
        mEdits << edit;

        switch( mGlossLines.at(i).type() )
        {
        case Project::Orthographic:
            break;
        case Project::Gloss:
            connect(edit,SIGNAL(stringChanged(TextBit)), mProject, SLOT(updateInterpretationGloss(TextBit)));
            break;
        case Project::Transcription:
            connect(edit,SIGNAL(stringChanged(TextBit)), mProject, SLOT(updateInterpretationTranscription(TextBit)));
            break;
        }
    }
}


void AbstractWordDisplayWidget::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);
    menu.addAction(tr("New gloss..."),this,SLOT(newGloss()));
    menu.exec(event->globalPos());
}

void AbstractWordDisplayWidget::newGloss()
{
}

QSize AbstractWordDisplayWidget::sizeHint() const
{
    return QSize(128,128);
}

void AbstractWordDisplayWidget::fillData()
{
    qDebug() << mTextBit->text() << mTextBit->id();
    if( mTextBit->id() != -1 )
    {
        for(int i=0; i<mGlossLines.count();i++)
        {
            switch( mGlossLines.at(i).type() )
            {
            case Project::Orthography:
                mEdits[i]->setText( mProject->getInterpretationOrthography(mTextBit->id(), mGlossLines.at(i).writingSystem() ) );
                break;
            case Project::Transcription:
                qDebug() << "Transcription" << mProject->getInterpretationTranscription(mTextBit->id(), mGlossLines.at(i).writingSystem() );
                qDebug() << "textbit id" << mTextBit->id();
                qDebug() << "glossline ws" << mGlossLines.at(i).writingSystem()->flexString();
                mEdits[i]->setText( mProject->getInterpretationTranscription(mTextBit->id(), mGlossLines.at(i).writingSystem() ) );
                break;
            case Project::Gloss:
                qDebug() << "Gloss" << mProject->getInterpretationTranscription(mTextBit->id(), mGlossLines.at(i).writingSystem() );
                qDebug() << "textbit id" << mTextBit->id();
                qDebug() << "glossline ws" << mGlossLines.at(i).writingSystem()->flexString();
                mEdits[i]->setText( mProject->getInterpretationGloss(mTextBit->id(), mGlossLines.at(i).writingSystem() ) );
                break;
            }
        }
    }
}
