#ifndef TEXT_H
#define TEXT_H

#include <QObject>
#include <QString>
#include <QList>

#include "textbit.h"

class WritingSystem;
class Project;
class QFile;

class Text : public QObject
{
    Q_OBJECT
public:
    enum BaselineMode { Orthographic, Phonetic };
    enum CandidateStatus { SingleOption, MultipleOption };
    enum ApprovalStatus { Approved, Unapproved };

    Text();
    Text(const QString & name, BaselineMode bm, WritingSystem *ws, Project *project);
    Text(QFile *file, Project *project);
    Text(QFile *file, BaselineMode bm, WritingSystem *ws, Project *project);

    QString name() const;
    void setName(const QString & name);
    BaselineMode baselineMode() const;
    void setBaselineMode(BaselineMode bm);
    WritingSystem* writingSystem() const;
    void setWritingSystem(WritingSystem *ws);

    QString baselineText() const;
    void setBaselineText(const QString & text);

    QList< QList<TextBit*>* >* baselineBits();

private:
    QString mName;    
    QString mBaselineText;

    Project *mProject;

    CandidateStatus mCandidateStatus;
    ApprovalStatus mApprovalStatus;

    BaselineMode mBaselineMode;
    WritingSystem *mBaselineWritingSystem;

    QList< QList<TextBit*>* > mBaselineBits;

    void clearTextBits();

    void setBaselineBitsFromBaseline();

    void guessInterpretation(TextBit *bit);

    void importTextFromFlexText(QFile *file, bool baselineInfoFromFile = false);

signals:
    void baselineTextChanged(const QString & baseline);
    void idChanged(TextBit *b, qlonglong oldId);
};

#endif // TEXT_H
