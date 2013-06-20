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

class LexiconEdit : public QMainWindow
{
    Q_OBJECT

public:
    LexiconEdit(const DatabaseAdapter * dbAdapter, const MainWindow * mainWnd, QWidget *parent = 0);
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
};

#endif // LEXICONEDIT_H
