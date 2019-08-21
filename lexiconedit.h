/*!
  \class LexiconEdit
  \ingroup Lexicon
  \brief This is a form for editing the lexicon. Beginning on the left and moving clockwise, this consists of a:
   - LexiconView (associated with a LexiconModel)
   - QTableView (associate with an AllomorphModel)
   - QTableView (associate with a MorphologicalAnalysisModel)
   - Two TagView objects (the left associated with a TagModel, the right associated with an AllTagsModel)

  The associated form is lexiconedit.ui.
*/


#ifndef LEXICONEDIT_H
#define LEXICONEDIT_H

#include <QMainWindow>

namespace Ui {
    class LexiconEdit;
}

class DatabaseAdapter;
class MainWindow;
class QModelIndex;
class LexiconModel;
class QSortFilterProxyModel;
class Project;

class LexiconEdit : public QMainWindow
{
    Q_OBJECT

public:
    LexiconEdit(Project *project, QWidget *parent = nullptr);
    ~LexiconEdit();

signals:
    void textFormIdSearch(qlonglong textFormId);

private slots:
    void analysisDoubleClicked( const QModelIndex & index );
    void editForm( const QModelIndex & index );
    void setFilterAppliesToSortColumnOnly();

private:
    Ui::LexiconEdit * ui;
    LexiconModel * mLexiconModel;
    QSortFilterProxyModel * mLexiconProxyModel;
    const DatabaseAdapter * mDbAdapter;
    Project * mProject;
};

#endif // LEXICONEDIT_H
