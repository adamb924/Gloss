#include "interlinearchunkeditor.h"
#include "ui_interlinearchunkeditor.h"

#include "text.h"
#include "textdisplaywidget.h"

#include <QInputDialog>
#include <QLabel>
#include <QtDebug>
#include <QCloseEvent>

InterlinearChunkEditor::InterlinearChunkEditor(Text *text, Project *project, View::Type type, int chunkSize, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InterlinearChunkEditor)
{
    ui->setupUi(this);

    mTextDisplayWidget = 0;

    mText = text;
    mProject = project;
    mType = type;
    mChunkSize = chunkSize;
    mPosition = 0;
    mNPhrases = mText->phrases()->count();

    connect( ui->previousButton , SIGNAL(clicked()), this, SLOT(previous()) );
    connect( ui->nextButton, SIGNAL(clicked()), this, SLOT(next()) );
    connect( ui->buttonGoTo, SIGNAL(clicked()), this, SLOT(goTo()) );

    // make the ranges
    QList<int> lines;
    for( int i=mPosition; i < mPosition + mChunkSize && i < mNPhrases; i++ )
        lines << i;

    setButtonActivation();

    mTextDisplayWidget = new TextDisplayWidget( mText, mProject, mType, makeLines(), QList<Focus>(), this );
    ui->ildLayout->addWidget(mTextDisplayWidget);
}

InterlinearChunkEditor::~InterlinearChunkEditor()
{
    delete ui;
}

void InterlinearChunkEditor::next()
{
    moveToPosition( mPosition + mChunkSize );
}

void InterlinearChunkEditor::previous()
{
    moveToPosition( mPosition - mChunkSize );
}

void InterlinearChunkEditor::goTo()
{
    bool ok;
    int newPosition = QInputDialog::getInt(this, tr("Go to"), tr("Go to the chunk with line..."), mPosition, 1, mNPhrases, 1, &ok );
    if( ok )
    {
        newPosition--; // zero-index it
        moveToPosition( (newPosition / mChunkSize) * mChunkSize );
    }
}

void InterlinearChunkEditor::moveToPosition(int position)
{
    // out-of-bounds checks
    if( position < 0 )
        position = 0;

    if( position >= mNPhrases )
        return;

    mPosition = position;

    setButtonActivation();

    mTextDisplayWidget->setLines( makeLines() );
}

void InterlinearChunkEditor::setButtonActivation()
{
    if( mPosition - mChunkSize < 0 )
        ui->previousButton->setEnabled(false);
    else
        ui->previousButton->setEnabled(true);

    if( mPosition + mChunkSize >= mNPhrases )
        ui->nextButton->setEnabled(false);
    else
        ui->nextButton->setEnabled(true);
}

QList<int> InterlinearChunkEditor::makeLines()
{
    QList<int> lines;
    for( int i=mPosition; i < mPosition + mChunkSize && i < mNPhrases; i++ )
        lines << i;
    return lines;
}

void InterlinearChunkEditor::closeEvent(QCloseEvent *event)
{
    mTextDisplayWidget->saveText();
    event->accept();
}
