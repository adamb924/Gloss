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
    Q_UNUSED(type);
    Q_UNUSED(identifier);
    QMessageBox::warning(nullptr, mSource, QString("%1 (Line %2, Column %3)").arg(description).arg(sourceLocation.line()).arg(sourceLocation.column()));
}
