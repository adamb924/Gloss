#include "view.h"

View::View(const QString &name)
{
    mName = name;
    mBaselineTextTab = false;
}

QString View::name() const
{
    return mName;
}

const QList<Tab> * View::tabs() const
{
    return &mTabs;
}

QList<Tab> * View::tabs()
{
    return &mTabs;
}

bool View::showBaselineTextTab() const
{
    return mBaselineTextTab;
}

void View::setShowBaselineTextTab(bool show)
{
    mBaselineTextTab = show;
}
