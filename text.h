#ifndef TEXT_H
#define TEXT_H

#include "project.h"

class Text
{
public:
    Text();

    Text(const QString & name, Project::BaselineMode bm, WritingSystem *ws);

    QString name() const;
    void setName(const QString & name);
    Project::BaselineMode baselineMode() const;
    void setBaselineMode(Project::BaselineMode bm);
    WritingSystem* writingSystem() const;
    void setWritingSystem(WritingSystem *ws);

    QString baselineText() const;
    void setBaselineText(const QString & text);

private:
    QString mName;

    QString mBaselineText;

    Project::BaselineMode mBaselineMode;
    WritingSystem *mBaselineWritingSystem;
};

#endif // TEXT_H
