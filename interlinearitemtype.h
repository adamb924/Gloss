/*!
  \class InterlinearItemType
  \ingroup GUI
  \brief A data class representing a line of a gloss: the type of the line (LineType: Text, Gloss), and the writing system of the line.
*/

#ifndef INTERLINEARITEM_H
#define INTERLINEARITEM_H

#include "writingsystem.h"

class InterlinearItemType {
public:
    enum LineType {
        ImmutableText,
        ImmutableGloss,
        Text,
        Gloss,
        Analysis,
        Null
    };

    InterlinearItemType();
    InterlinearItemType(LineType type, const WritingSystem & ws);
    InterlinearItemType(const QString & type, const WritingSystem & ws);

    void setType(LineType type);
    void setWritingSystem(const WritingSystem & ws);
    LineType type() const;
    WritingSystem writingSystem() const;

private:
    LineType mType;
    WritingSystem mWritingSystem;
};

typedef QList<InterlinearItemType> InterlinearItemTypeList;

#endif // GLOSSLINE_H
