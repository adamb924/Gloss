#include "interlinearchunkeditor.h"
#include "ui_interlinearchunkeditor.h"

#include "text.h"
#include "texttabwidget.h"
#include "mainwindow.h"

#include <QInputDialog>
#include <QLabel>
#include <QtDebug>
#include <QCloseEvent>

InterlinearChunkEditor::InterlinearChunkEditor(Text *text, Project *project, View::Type type, int chunkSize, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::InterlinearChunkEditor)
{
    ui->setupUi(this);

    mTextTabWidget = nullptr;

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

    connect( text, SIGNAL(guiRefreshRequest()), this, SLOT(refreshLayout()));

    refreshLayout();

    mTextTabWidget = new TextTabWidget( mText, mProject, mType, makeLines(), QList<Focus>(), this );
    ui->ildLayout->addWidget(mTextTabWidget);

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
    int newPosition = QInputDialog::getInt(this, tr("Go to"), tr("Go to the chunk with line... (1-%1)").arg(mText->phraseCount()), mPosition, 1, mText->phraseCount(), 1, &ok );
    if( ok )
        moveToLine( newPosition );
}

void InterlinearChunkEditor::beginning()
{
    moveToLine( 0 );
}

void InterlinearChunkEditor::end()
{
    moveToLine( mText->phraseCount() );
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

    if( position >= mText->phraseCount() )
        return;

    mPosition = position;

    refreshLayout();
}

void InterlinearChunkEditor::refreshLayout()
{
    if( mPosition - mChunkSize < 0 )
    {
        ui->previousButton->setEnabled(false);
        ui->beginningButton->setEnabled(false);
    }
    else
    {
        ui->previousButton->setEnabled(true);
        ui->beginningButton->setEnabled(true);
    }

    if( mPosition + mChunkSize >= mText->phraseCount() )
    {
        ui->nextButton->setEnabled(false);
        ui->endButton->setEnabled(false);
    }
    else
    {
        ui->nextButton->setEnabled(true);
        ui->endButton->setEnabled(true);
    }

    if( mTextTabWidget != nullptr )
    {
        mTextTabWidget->setLines( makeLines() );
    }
}

QList<int> InterlinearChunkEditor::makeLines()
{
    QList<int> lines;
    // line numbers may have changed
    if( mPosition >= mText->phraseCount() )
    {
        mPosition = ((mText->phraseCount() - 1) / mChunkSize) * mChunkSize;
    }
    for( int i=mPosition; i < mPosition + mChunkSize && i < mText->phraseCount(); i++ )
        lines << i;
    return lines;
}

void InterlinearChunkEditor::closeEvent(QCloseEvent *event)
{
    mTextTabWidget->saveText();
    event->accept();
}

void InterlinearChunkEditor::keyReleaseEvent(QKeyEvent *event)
{
    if( MainWindow::mShortcuts.keysequence("NextPage").matches( toKeySequence(event) ) )
    {
        next();
    } else if( MainWindow::mShortcuts.keysequence("PreviousPage").matches( toKeySequence(event) ) )
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
    mTextTabWidget->setFocus(foci);
}
