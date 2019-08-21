/*!
  \class PhrasalGlossesModel
  \ingroup GUIModel
  \brief This is a model of a list of phrasal glosses (i.e., as child elements of a Tab). It's currently used in ViewConfigurationDialog, with a simple QListView.
*/

#ifndef PHRASALGLOSSESMODEL_H
#define PHRASALGLOSSESMODEL_H

#include <QAbstractListModel>

class Tab;
class WritingSystem;

class PhrasalGlossesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    PhrasalGlossesModel(Tab * tab, QObject *parent = nullptr);

    void addPhrasalGloss(const WritingSystem & ws);
    void removePhrasalGloss(int index);
    void editPhrasalGloss(const QModelIndex &index, const WritingSystem & ws);

    int columnCount(const QModelIndex & parent = QModelIndex()) const;
    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex & index) const;

    void moveUp(int index);
    void moveDown(int index);

signals:

public slots:

private:
    Tab * mTab;
};

#endif // PHRASALGLOSSESMODEL_H
