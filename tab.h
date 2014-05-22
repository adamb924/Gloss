/*!
  \class Tab
  \ingroup Interlinear
  \brief This is the class representation of the &lt;tab&gt; tag in configuration.xml, which determines the configuration of the interlinear lines in an interlinear display. Its most important members are InterlinearItemTypeList objects with interlinear line specifications for the word and phrase levels. Every View has a list of Tab objects.
*/

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
