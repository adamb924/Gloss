#ifndef TEXTINFO_H
#define TEXTINFO_H

#include "project.h"

class TextInfo
{
public:
    TextInfo();
    TextInfo(const QString & name, Project::BaselineMode bm, WritingSystem *ws);

    QString name() const;
    void setName(const QString & name);
    Project::BaselineMode baselineMode() const;
    void setBaselineMode(Project::BaselineMode bm);
    WritingSystem* writingSystem() const;
    void setWritingSystem(WritingSystem *ws);

private:
    QString mName;
    Project::BaselineMode mBaselineMode;
    WritingSystem *mBaselineWritingSystem;
};

#endif // TEXTINFO_H
