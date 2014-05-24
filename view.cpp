#include "view.h"

View::View(const QString &name)
{
    mName = name;
}

View::~View()
{
    qDeleteAll(mTabs);
}

QString View::name() const
{
    return mName;
}

const QList<Tab *> *View::tabs() const
{
    return &mTabs;
}

QList<Tab*> * View::tabs()
{
    return &mTabs;
}

void View::setName(const QString &name)
{
    mName = name;
}
