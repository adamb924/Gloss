#ifndef VIEW_H
#define VIEW_H

#include <QList>

#include "tab.h"

class View
{
public:
    enum Type { Full, Quick };

    View(const QString & name);

    QString name() const;
    const QList<Tab> * tabs() const;
    QList<Tab> * tabs();

private:
    QString mName;
    QList<Tab> mTabs;
};

#endif // VIEW_H
