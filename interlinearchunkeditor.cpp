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

    connect( ui->previousButton , SIGNAL(clicked()), this, SLOT(previous()) );
    connect( ui->nextButton, SIGNAL(clicked()), this, SLOT(next()) );
    connect( ui->buttonGoTo, SIGNAL(clicked()), this, SLOT(goTo()) );
    connect( ui->beginningButton, SIGNAL(clicked()), this, SLOT(beginning()));
    connect( ui->endButton, SIGNAL(clicked()), this, SLOT(end()) );

    connect( text, SIGNAL(guiRefreshRequest()), this, SLOT(setButtonActivation()));

    // make the ranges
    QList<int> lines;
    for( int i=mPosition; i < mPosition + mChunkSize && i < mText->phrases()->count(); i++ )
        lines << i;

    setButtonActivation();

    mTextDisplayWidget = new TextDisplayWidget( mText, mProject, mType, makeLines(), QList<Focus>(), this );
    ui->ildLayout->addWidget(mTextDisplayWidget);

    setWindowTitle(tr("%1 [%2]").arg(mText->name()).arg(mProject->view(type)->name()));
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
    int newPosition = QInputDialog::getInt(this, tr("Go to"), tr("Go to the chunk with line... (1-%1)").arg(mText->phrases()->count()), mPosition, 1, mText->phrases()->count(), 1, &ok );
    if( ok )
        moveToLine( newPosition );
}

void InterlinearChunkEditor::beginning()
{
    moveToLine( 0 );
}

void InterlinearChunkEditor::end()
{
    moveToLine( mText->phrases()->count() );
}

void InterlinearChunkEditor::moveToLine(int line)
{
    line--; // zero-index it
    moveToPosition( (line / mChunkSize) * mChunkSize );
}

void InterlinearChunkEditor::moveToPosition(int position)
{
    // out-of-bounds checks
    if( position < 0 )
        position = 0;

    if( position >= mText->phrases()->count() )
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

    if( mPosition + mChunkSize >= mText->phrases()->count() )
        ui->nextButton->setEnabled(false);
    else
        ui->nextButton->setEnabled(true);
}

QList<int> InterlinearChunkEditor::makeLines()
{
    QList<int> lines;
    for( int i=mPosition; i < mPosition + mChunkSize && i < mText->phrases()->count(); i++ )
        lines << i;
    return lines;
}

void InterlinearChunkEditor::closeEvent(QCloseEvent *event)
{
    mTextDisplayWidget->saveText();
    event->accept();
}

void InterlinearChunkEditor::keyReleaseEvent(QKeyEvent *event)
{
    if( event->key() == Qt::Key_PageDown )
    {
        next();
    } else if ( event->key() == Qt::Key_PageUp )
    {
        previous();
    }
    QWidget::keyReleaseEvent(event);
}

Text * InterlinearChunkEditor::text()
{
    return mText;
}

void InterlinearChunkEditor::setFocus( const QList<Focus> & foci )
{
    mTextDisplayWidget->setFocus(foci);
}
