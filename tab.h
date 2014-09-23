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
    enum TabType { InterlinearDisplay, SyntacticParsing, Null };

    Tab(const QString & name, TabType type = Tab::InterlinearDisplay );
    ~Tab();

    QString name() const;
    void setName(const QString & name);

    void addInterlinearLineType( const WritingSystem & ws, const InterlinearItemType & type );
    void removeInterlinearLineType( const WritingSystem & ws, int index );
    void editInterlinearLineType( const WritingSystem & ws, int index, const InterlinearItemType & type );

    void addPhrasalGlossType(const InterlinearItemType & type );
    void removePhrasalGloss(int index);
    void setPhrasalGloss(int index, const InterlinearItemType & type);

    InterlinearItemTypeList interlinearLines(const WritingSystem & ws) const;
    QList<WritingSystem> interlinearLineKeys() const;
    void interlinearLineSwap(const WritingSystem & ws, int i, int j);

    InterlinearItemTypeList* phrasalGlossLines();
    const InterlinearItemTypeList *phrasalGlossLines() const;

    Tab::TabType type() const;

    static QString getTypeString(TabType t);
    static TabType getType(QString t);

private:
    QString mName;
    QHash<WritingSystem,InterlinearItemTypeList*> mInterlinearLines;
    InterlinearItemTypeList mPhrasalGlossLines;
    Tab::TabType mType;
};

#endif // TAB_H
