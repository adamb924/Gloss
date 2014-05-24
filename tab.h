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
  ~Tab();

  QString name() const;
  void setName(const QString & name);

  void addInterlinearLineType( const WritingSystem & ws, const InterlinearItemType & type );
  void removeInterlinearLineType( const WritingSystem & ws, int index );
  void editInterlinearLineType( const WritingSystem & ws, int index, const InterlinearItemType & type );

  void addPhrasalGlossType(const InterlinearItemType & type );
  void removePhrasalGloss(int index);
  void setPhrasalGloss(int index, const InterlinearItemType & type);

  QHash<WritingSystem,InterlinearItemTypeList*> interlinearLines() const;
  QHash<WritingSystem,InterlinearItemTypeList*> interlinearLines();
  InterlinearItemTypeList* phrasalGlossLines();
  const InterlinearItemTypeList *phrasalGlossLines() const;

private:
    QString mName;
    QHash<WritingSystem,InterlinearItemTypeList*> mInterlinearLines;
    InterlinearItemTypeList mPhrasalGlossLines;
};

#endif // TAB_H
