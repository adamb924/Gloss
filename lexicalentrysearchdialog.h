/*!
  \class LexicalEntrySearchDialog
  \ingroup MorphologicalAnalysis
  \brief This is a widget for searching for a lexical item. The upper combo box lists lexical entries that begin with the search string, and the lower combo box a substring search.

  It is instantiated either in LexicalEntryForm ("Link to other...") or in an AnalysisWidget, when in respons to the "Link to other..." button being clicked in CreateLexicalEntryDialog.

  The interface is in lexicalentrysearchdialog.ui.
*/

#ifndef LEXICALENTRYSEARCHDIALOG_H
#define LEXICALENTRYSEARCHDIALOG_H

#include <QDialog>

namespace Ui {
    class LexicalEntrySearchDialog;
}

class DatabaseAdapter;
class QStandardItemModel;
class Project;

#include "writingsystem.h"

class LexicalEntrySearchDialog : public QDialog
{
    Q_OBJECT

public:
    LexicalEntrySearchDialog(const Project * project, QWidget *parent = nullptr);
    ~LexicalEntrySearchDialog();

    qlonglong lexicalEntryId() const;

private:
    Ui::LexicalEntrySearchDialog *ui;
    const Project * mProject;
    const DatabaseAdapter * mDbAdapter;

    QList<WritingSystem> mWritingSystems;
    QStandardItemModel *mFirstModel;
    QStandardItemModel *mSecondModel;
    qlonglong mLexicalEntryId;

private slots:
    void fillCandidates();
    void changeCurrentWritingSystem(int index);
    void activated(const QModelIndex & index);
};

#endif // LEXICALENTRYSEARCHDIALOG_H
