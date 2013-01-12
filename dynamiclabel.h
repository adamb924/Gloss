#ifndef DYNAMICLABEL_H
#define DYNAMICLABEL_H

#include <QLabel>

class DynamicLabel : public QLabel
{
    Q_OBJECT
public:
    explicit DynamicLabel(QObject *parent = 0);

signals:

public slots:

};

#endif // DYNAMICLABEL_H
