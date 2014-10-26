/*!
  \class ChooseLexicalEntriesDialog
  \ingroup MorphologicalAnalysis
  \brief A form for the user to choose (or create) lexical entries to go along with allomorphs. ChooseLexicalEntriesDialog will have one or more LexicalEntryForm objects, which provide the interface. ChooseLexicalEntriesDialog then commits those changes to the database upon acceptance.
*/

#ifndef CHOOSELEXICALENTRIESDIALOG_H
#define CHOOSELEXICALENTRIESDIALOG_H

#include <QDialog>
#include <QList>

#include "lexicalentryform.h"

class DatabaseAdapter;
class GlossItem;
class QDialogButtonBox;
class Project;

#include "morphologicalanalysis.h"

class ChooseLexicalEntriesDialog : public QDialog
{
    Q_OBJECT
public:
    //! \brief \a parseString must have the id of the textForm with which the morphological analysis is associated
    ChooseLexicalEntriesDialog(const TextBit & parseString, const GlossItem *glossItem, Project *project, QWidget *parent = 0);

    MorphologicalAnalysis * morphologicalAnalysis() const;

signals:
    void resegment();

public slots:

private:
    Project * mProject;
    const DatabaseAdapter * mDbAdapter;
    TextBit mParseString;
    QList<TextBit> mAllomorphStrings;
    QList<LexicalEntryForm*> mEntries;
    QDialogButtonBox *mButtonBox;
    QPushButton * mOk;

    void setupLayout();
    const GlossItem *mGlossItem;

private slots:
    void commitChanges();
    void rollbackChanges();
    void setAcceptable();
};

#endif // CHOOSELEXICALENTRIESDIALOG_H
