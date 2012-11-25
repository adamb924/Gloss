#ifndef WORDDISPLAYWIDGET_H
#define WORDDISPLAYWIDGET_H

#include <QWidget>
#include <QString>
#include <QList>

#include "glossline.h"
#include "lingedit.h"

class Project;
class QVBoxLayout;
class WritingSystem;
class TextBit;

class WordDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    WordDisplayWidget(TextBit *bit, Project::BaselineMode baselineMode, Project *project);

    QSize sizeHint() const;

    void guessInterpretation();

protected:
    Project *mProject;
    TextBit *mTextBit;

    Project::BaselineMode mBaselineMode;

    enum CandidateStatus { SingleOption, MultipleOption };
    enum ApprovalStatus { Approved, Unapproved };

    CandidateStatus mCandidateStatus;
    ApprovalStatus mApprovalStatus;

    void fillData();

private:
    void contextMenuEvent ( QContextMenuEvent * event );

    void setupLayout();

    QVBoxLayout *mLayout;

    QList<GlossLine> mGlossLines;
    QList<LingEdit*> mEdits;

private slots:
    void newGloss();
};

#endif // WORDDISPLAYWIDGET_H
