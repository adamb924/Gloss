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
    bool showBaselineTextTab() const;
    void setShowBaselineTextTab(bool show);

private:
    QString mName;
    QList<Tab> mTabs;
    bool mBaselineTextTab;
};

#endif // VIEW_H
