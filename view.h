/*!
  \class View
  \ingroup Interlinear
  \brief This is the class representation of the &lt;view&gt; tag in configuration.xml. Its most important member is a list of Tab objects. Users choose between views with Project | View or Project | Quick view. Every Project has a list of View objects.
*/

#ifndef VIEW_H
#define VIEW_H

#include <QList>

#include "tab.h"

class View
{
public:
    enum Type { Full, Quick };

    explicit View(const QString & name);
    ~View();

    QString name() const;
    const QList<Tab*> * tabs() const;
    QList<Tab*> * tabs();

    void setName(const QString & name);

private:
    QString mName;
    QList<Tab*> mTabs;
};

#endif // VIEW_H
