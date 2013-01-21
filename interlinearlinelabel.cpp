#include "interlinearlinelabel.h"

#include <QMenu>
#include <QContextMenuEvent>

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

    menu.exec(event->globalPos());
}

void InterlinearLineLabel::mouseDoubleClickEvent ( QMouseEvent * event )
{
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
