/*!
  \class EditWithSuggestionsDialog
  \ingroup LexiconEdit
  \brief This is a generic form that prompts the user with an edit box and a list of suggested strings to select. See editwithsuggestionsdialog.ui.

  Although it could be reused, this is currently only instantated in LexiconEdit.
*/

#ifndef EDITWITHSUGGESTIONSDIALOG_H
#define EDITWITHSUGGESTIONSDIALOG_H

#include <QDialog>

class TextBit;
class WritingSystem;
class QStringListModel;

namespace Ui {
class EditWithSuggestionsDialog;
}

class EditWithSuggestionsDialog : public QDialog
{
    Q_OBJECT

public:
    EditWithSuggestionsDialog(const WritingSystem & ws, QWidget *parent = 0);
    ~EditWithSuggestionsDialog();

    void setDefaultString( const QString & string );
    void setSuggestions( const QStringList & strings );
    QString text() const;
    TextBit textBit() const;

private slots:
    void copyValue(const QModelIndex & index);

private:
    Ui::EditWithSuggestionsDialog *ui;
    QStringListModel *mModel;
};

#endif // EDITWITHSUGGESTIONSDIALOG_H
