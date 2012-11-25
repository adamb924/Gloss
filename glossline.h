#ifndef GLOSSLINE_H
#define GLOSSLINE_H

#include "project.h"

class GlossLine {
public:
    GlossLine();
    GlossLine(Project::GlossLineType type, WritingSystem *ws);

    void setType(Project::GlossLineType type);
    void setWritingSystem(WritingSystem *ws);
    Project::GlossLineType type() const;
    WritingSystem* writingSystem() const;

private:
    Project::GlossLineType mType;
    WritingSystem *mWritingSystem;
};


#endif // GLOSSLINE_H
