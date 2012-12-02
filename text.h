#ifndef TEXT_H
#define TEXT_H

#include <QObject>
#include <QString>
#include <QList>

#include "textbit.h"
#include "glossitem.h"

class WritingSystem;
class Project;
class QFile;
class QXmlStreamWriter;

class Text : public QObject
{
    Q_OBJECT
public:
    enum CandidateStatus { SingleOption, MultipleOption };
    enum ApprovalStatus { Approved, Unapproved };

    Text();
    Text(const QString & name, WritingSystem *ws, Project *project);
    Text(QFile *file, Project *project);
    Text(QFile *file, WritingSystem *ws, Project *project);

    QString name() const;
    void setName(const QString & name);

    QString comment() const;
    void setComment(const QString & comment);

    WritingSystem* writingSystem() const;
    void setWritingSystem(WritingSystem *ws);

    QString baselineText() const;
    void setBaselineText(const QString & text);

//    QList< QList<TextBit*>* >* baselineBits();
    QList< QList<GlossItem*>* >* glossItems();

    //! \brief Serialize the text to an XML file
    bool serialize(const QString & filename) const;

    //! \brief Serialize the interlinear text (i.e., <interlinear-text> to the XML stream
    bool serializeInterlinearText(QXmlStreamWriter *stream) const;

    //! \brief Write an <item> to the text stream, with specified attributes and text content
    void writeItem(const QString & type, const WritingSystem * ws, const QString & text , QXmlStreamWriter *stream) const;

private:
    QString mName, mComment;
    QString mBaselineText;

    Project *mProject;

    CandidateStatus mCandidateStatus;
    ApprovalStatus mApprovalStatus;

    WritingSystem *mBaselineWritingSystem;

//    QList< QList<TextBit*>* > mBaselineBits;
    QList< QList<GlossItem*>* > mGlossItems;

    void clearTextBits();

    void setBaselineBitsFromBaseline();

    //! \brief Attempt to set the (interpretation) id of \a bit by querying the database for interpretations compatible with the text form TextBit.
    void guessInterpretation(TextBit *bit);

    //! \brief Attempt to set the (interpretation) id of \a bit by querying the database for interpretations compatible with text and gloss TextBits.
    void guessInterpretation(TextBit *bit, const QList<TextBit> & textForms , const QList<TextBit> & glossForms);

    void importTextFromFlexText(QFile *file, bool baselineInfoFromFile = false);

signals:
    void baselineTextChanged(const QString & baseline);
    void idChanged(TextBit *b, qlonglong oldId);
};

#endif // TEXT_H
