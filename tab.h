#ifndef TAB_H
#define TAB_H

#include <QList>

#include "interlinearitemtype.h"

class Tab
{
public:
  Tab(const QString & name);

  QString name() const;

  void addInterlinearLineType( const InterlinearItemType & type );
  void addPhrasalGlossType( const InterlinearItemType & type );

  QList<InterlinearItemType> interlinearLines() const;
  QList<InterlinearItemType> phrasalGlossLines() const;

private:
    QString mName;

    QList<InterlinearItemType> mInterlinearLines;
    QList<InterlinearItemType> mPhrasalGlossLines;
};

#endif // TAB_H
