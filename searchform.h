#ifndef SEARCHFORM_H
#define SEARCHFORM_H

#include <QWidget>

namespace Ui {
class SearchForm;
}

class DatabaseAdapter;
class TextBit;

class SearchForm : public QWidget
{
    Q_OBJECT

public:
    SearchForm(const DatabaseAdapter * dbAdapter, QWidget *parent = 0);
    ~SearchForm();

signals:
    void searchForInterpretationById(qlonglong id);
    void searchForTextFormById(qlonglong id);
    void searchForGlossById(qlonglong id);
    void searchForLexicalEntryById(qlonglong id);
    void searchForAllomorphById(qlonglong id);
    void searchForText(const TextBit & bit);
    void searchForSubstring(const TextBit & bit);

public slots:
    void setXmlTextWarning(bool relevant);

private slots:
    void interpretationId();
    void textFormId();
    void glossId();
    void lexicalEntryId();
    void allomorphId();
    void text();

private:
    Ui::SearchForm *ui;
};

#endif // SEARCHFORM_H
