/*!
  \class AllTagsModel
  \ingroup GUIModel
  \ingroup LexiconEdit
  \brief This class is a model for displaying grammatical tags associated with a lexical entry. Currently it is only used by LexiconEdit.
*/

#ifndef ALLTAGSMODEL_H
#define ALLTAGSMODEL_H

#include <QStandardItemModel>

class DatabaseAdapter;

class AllTagsModel : public QStandardItemModel
{
    Q_OBJECT
public:
    AllTagsModel(const DatabaseAdapter * dbAdapter, QWidget *parent = 0);

    void refreshTags( );

    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
    bool setData ( const QModelIndex & index, const QVariant & value, int role );

    QStringList positiveTags() const;
    QStringList negativeTags() const;

signals:

public slots:

private:
    const DatabaseAdapter * mDbAdapter;

};

#endif // ALLTAGSMODEL_H
