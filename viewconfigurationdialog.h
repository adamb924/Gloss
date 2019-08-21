/*!
  \class ViewConfigurationDialog
  \ingroup GUI
  \brief This is a dialog box for configuring the View (and Tab etc.) elements of a Project. It is used in MainWindow. The associated UI file is viewconfigurationdialog.ui.
*/

#ifndef VIEWCONFIGURATIONDIALOG_H
#define VIEWCONFIGURATIONDIALOG_H

#include <QDialog>

class Project;
class ViewsModel;
class TabsModel;
class ItemsModel;
class PhrasalGlossesModel;
class QItemSelection;
class View;
class Tab;
class WritingSystem;

namespace Ui {
class ViewConfigurationDialog;
}

class ViewConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    ViewConfigurationDialog(Project * project, QWidget *parent = nullptr);
    ~ViewConfigurationDialog();

    void populateWritingSystemCombo();
private slots:
    void addView();
    void removeView();
    void addTab();
    void removeTab();
    void addItem();
    void removeItem();
    void addPhrasalGloss();
    void removePhrasalGloss();
    void viewChanged(const QItemSelection & selected, const QItemSelection & deselected);
    void tabChanged(const QItemSelection & selected, const QItemSelection & deselected);
    void indexLanguageChanged(int index);
    void editPhrasalGloss(const QModelIndex & index);
    void editItem(const QModelIndex & index);

    void viewUp();
    void viewDown();
    void tabUp();
    void tabDown();
    void itemUp();
    void itemDown();
    void phrasalGlossUp();
    void phrasalGlossDown();

    void setTabWidgetsEnabled(bool enabled);
    void setItemWidgetsEnabled(bool enabled);

    void accept();

private:
    WritingSystem currentWritingSystem() const;
    bool viewsAreAcceptable();

    Ui::ViewConfigurationDialog *ui;

    Project *mProject;
    View *mView;
    Tab * mTab;

    ViewsModel * mViewModel;
    TabsModel * mTabsModel;
    ItemsModel * mItemsModel;
    PhrasalGlossesModel * mPhrasalGlossesModel;
};

#endif // VIEWCONFIGURATIONDIALOG_H
