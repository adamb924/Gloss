#ifndef TAB_H
#define TAB_H

#include <QList>

#include "interlinearitemtype.h"

class Tab
{
public:
  Tab(const QString & name);

  QString name() const;

  void addInterlinearLineType( const WritingSystem & ws, const InterlinearItemType & type );
  void addPhrasalGlossType(const InterlinearItemType & type );

  QHash<WritingSystem,InterlinearItemTypeList> interlinearLines() const;
  InterlinearItemTypeList phrasalGlossLines() const;

private:
    QString mName;
    QHash<WritingSystem,InterlinearItemTypeList> mInterlinearLines;
    InterlinearItemTypeList mPhrasalGlossLines;
};

#endif // TAB_H
