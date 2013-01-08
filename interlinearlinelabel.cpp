#include "interlinearlinelabel.h"

#include <QMenu>
#include <QContextMenuEvent>

InterlinearLineLabel::InterlinearLineLabel(int lineNumber, const QString & label, QWidget *parent) :
    QLabel(label, parent)
{
    mLineNumber = lineNumber;

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMinimumSize(30, 30);
}

void InterlinearLineLabel::contextMenuEvent ( QContextMenuEvent * event )
{
    QMenu menu(this);

    menu.addAction(tr("Approve all"), this, SLOT(emitApproveAll()) );
    menu.addAction(tr("Play sound"), this, SLOT(emitPlaySound()) );

    menu.exec(event->globalPos());
}

void InterlinearLineLabel::emitApproveAll()
{
    emit approveAll(mLineNumber);
}

void InterlinearLineLabel::emitPlaySound()
{
    emit playSound(mLineNumber);
}
