#include "messagehandler.h"

#include <QtDebug>
#include <QMessageBox>

MessageHandler::MessageHandler(const QString & source, QObject *parent) :
    QAbstractMessageHandler(parent)
{
  mSource = source;
}

void MessageHandler::handleMessage( QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation )
{
    QMessageBox::warning(0, tr("XQuery Error"), QString("%1:\n%2 (Line %3, Column %4)").arg(mSource).arg(description).arg(sourceLocation.line()).arg(sourceLocation.column()));
}
