#include "messagehandler.h"

#include <QtDebug>

MessageHandler::MessageHandler(QObject *parent) :
    QAbstractMessageHandler(parent)
{
}

void MessageHandler::handleMessage( QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation )
{
    qWarning() << "MessageHandler::handleMessage" << description;
}
