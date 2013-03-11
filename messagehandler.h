/*!
  \class MessageHandler
  \ingroup Data
  \brief A message handler that sends error message output to qWarning(). Currently this is used only with QXmlQuery.
*/

#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <QAbstractMessageHandler>

class MessageHandler : public QAbstractMessageHandler
{
    Q_OBJECT
public:
    MessageHandler(const QString & source, QObject *parent = 0);

protected:
    void handleMessage( QtMsgType type, const QString & description, const QUrl & identifier, const QSourceLocation & sourceLocation );

    QString mSource;

signals:

public slots:

};

#endif // MESSAGEHANDLER_H
