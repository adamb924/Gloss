/*!
  \class GlossLine
  \ingroup GUI
  \brief A data class representing a line of a gloss: the type of the line (LineType: Text, Gloss), and the writing system of the line.
*/

#ifndef GLOSSLINE_H
#define GLOSSLINE_H

#include "writingsystem.h"

class GlossLine {
public:
    enum LineType {
        Text,
        Gloss
    };

    GlossLine();
    GlossLine(LineType type, const WritingSystem & ws);

    void setType(LineType type);
    void setWritingSystem(const WritingSystem & ws);
    LineType type() const;
    WritingSystem writingSystem() const;

private:
    LineType mType;
    WritingSystem mWritingSystem;
};


#endif // GLOSSLINE_H
