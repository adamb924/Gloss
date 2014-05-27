#include "interlinearlinelabel.h"

#include <QMenu>
#include <QContextMenuEvent>
#include <QMessageBox>

InterlinearLineLabel::InterlinearLineLabel(int lineNumber, const QString & label, bool soundAvailable, const QString & soundSummary, QWidget *parent) :
    QLabel(label, parent)
{
    mLineNumber = lineNumber;
    mSoundAvailable = soundAvailable;

    if( !soundSummary.isEmpty() )
        setToolTip(soundSummary);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(30, 30);
}

void InterlinearLineLabel::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    menu.addAction(tr("Approve all"), this, SLOT(emitApproveAll()) );
    menu.addAction(tr("Edit baseline text"), this, SLOT(emitEditLine()) );

    // this seems like the most intuitive thing to me, from a user interface standpoint
    QAction *playSound = menu.addAction(tr("Play sound"), this, SLOT(emitPlaySound()) );
    if( !mSoundAvailable )
        playSound->setEnabled(false);

    menu.addAction(tr("Remove line"), this, SLOT(emitRemoveLine()) );

    menu.exec(event->globalPos());
}

void InterlinearLineLabel::mouseDoubleClickEvent ( QMouseEvent * event )
{
    Q_UNUSED(event);
    emitPlaySound();
}

void InterlinearLineLabel::emitApproveAll()
{
    emit approveAll(mLineNumber);
}

void InterlinearLineLabel::emitPlaySound()
{
    emit playSound(mLineNumber);
}

void InterlinearLineLabel::emitEditLine()
{
    emit editLine(mLineNumber);
}

void InterlinearLineLabel::emitRemoveLine()
{
    if( QMessageBox::Yes == QMessageBox::question(this, tr("Confirm deletion"), tr("Are you sure you want to remove this gloss item? This cannot be undone."), QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) )
        emit removeLine(mLineNumber);
}
