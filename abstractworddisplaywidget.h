#ifndef ABSTRACTWORDDISPLAYWIDGET_H
#define ABSTRACTWORDDISPLAYWIDGET_H

#include <QWidget>
#include <QString>
#include <QList>

#include "glossline.h"
#include "lingedit.h"

class Project;
class QVBoxLayout;
class WritingSystem;
class TextBit;

class AbstractWordDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    AbstractWordDisplayWidget(TextBit *bit, Project *project);

    QSize sizeHint() const;

    virtual void guessInterpretation() = 0;

protected:
    Project *mProject;
    TextBit *mTextBit;

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

#endif // ABSTRACTWORDDISPLAYWIDGET_H
