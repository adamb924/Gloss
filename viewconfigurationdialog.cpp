#include "viewconfigurationdialog.h"
#include "ui_viewconfigurationdialog.h"

#include <QtDebug>
#include <QItemSelection>
#include <QInputDialog>
#include <QMessageBox>

#include "project.h"
#include "view.h"
#include "tab.h"

#include "viewsmodel.h"
#include "tabsmodel.h"
#include "itemsmodel.h"
#include "phrasalglossesmodel.h"
#include "writingsystemcombo.h"
#include "databaseadapter.h"
#include "writingsystemdialog.h"
#include "itemeditdialog.h"
#include "tabeditdialog.h"

ViewConfigurationDialog::ViewConfigurationDialog(Project *project, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ViewConfigurationDialog),
    mProject(project)
{
    ui->setupUi(this);

    mViewModel = new ViewsModel(mProject);
    ui->viewView->setModel( mViewModel );

    mTabsModel = nullptr;
    mItemsModel = nullptr;
    mPhrasalGlossesModel = nullptr;

    mView = nullptr;
    mTab = nullptr;

    connect(ui->addView, SIGNAL(clicked()), this, SLOT(addView()) );
    connect(ui->removeView, SIGNAL(clicked()), this, SLOT(removeView()) );

    connect(ui->addTab, SIGNAL(clicked()), this, SLOT(addTab()) );
    connect(ui->removeTab, SIGNAL(clicked()), this, SLOT(removeTab()) );

    connect(ui->addItem, SIGNAL(clicked()), this, SLOT(addItem()) );
    connect(ui->removeItem, SIGNAL(clicked()), this, SLOT(removeItem()) );
    connect(ui->itemView, SIGNAL(activated(QModelIndex)), this, SLOT(editItem(QModelIndex)) );

    connect(ui->addPhrasalGloss, SIGNAL(clicked()), this, SLOT(addPhrasalGloss()) );
    connect(ui->removePhrasalGloss, SIGNAL(clicked()), this, SLOT(removePhrasalGloss()) );
    connect(ui->phrasalGlossView, SIGNAL(activated(QModelIndex)), this, SLOT(editPhrasalGloss(QModelIndex)) );

    connect(ui->viewView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(viewChanged(QItemSelection,QItemSelection)));
    connect(ui->itemWritingSystemsCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(indexLanguageChanged(int)));

    connect(ui->viewUp, SIGNAL(clicked()), this, SLOT(viewUp()));
    connect(ui->viewDown, SIGNAL(clicked()), this, SLOT(viewDown()));
    connect(ui->tabUp, SIGNAL(clicked()), this, SLOT(tabUp()));
    connect(ui->tabDown, SIGNAL(clicked()), this, SLOT(tabDown()));
    connect(ui->itemUp, SIGNAL(clicked()), this, SLOT(itemUp()));
    connect(ui->itemDown, SIGNAL(clicked()), this, SLOT(itemDown()));
    connect(ui->phrasalGlossUp, SIGNAL(clicked()), this, SLOT(phrasalGlossUp()));
    connect(ui->phrasalGlossDown, SIGNAL(clicked()), this, SLOT(phrasalGlossDown()));

    setTabWidgetsEnabled(false);
    setItemWidgetsEnabled(false);
}

ViewConfigurationDialog::~ViewConfigurationDialog()
{
    delete ui;
}

void ViewConfigurationDialog::addView()
{
    bool ok;
    QString	name = QInputDialog::getText(this, tr("New view name"), tr("Enter a name for the view"), QLineEdit::Normal, QString(), &ok );
    if(ok)
    {
        mViewModel->addView( name );
    }
}

void ViewConfigurationDialog::removeView()
{
    if( ui->viewView->selectionModel()->selectedIndexes().count() > 0 )
    {
        mViewModel->removeView( ui->viewView->selectionModel()->selectedIndexes().first() );
    }
}

void ViewConfigurationDialog::addTab()
{
    if( mTabsModel != nullptr )
    {
        TabEditDialog dlg;
        if( dlg.exec() )
        {
            mTabsModel->addTab( dlg.name(), dlg.type()  );
        }
    }
}

void ViewConfigurationDialog::removeTab()
{
    if( mTabsModel != nullptr && ui->tabView->selectionModel()->selectedIndexes().count() > 0 )
    {
        mTabsModel->removeTab( ui->tabView->selectionModel()->selectedIndexes().first() );
    }
}

void ViewConfigurationDialog::addItem()
{
    if( mTab == nullptr ) return;

    ItemEditDialog dialog(mProject->dbAdapter()->writingSystems(), this);
    dialog.setWritingSystem( currentWritingSystem(), true );
    dialog.exec();
    if( dialog.result() == QDialog::Accepted )
    {
        mItemsModel->addItem( dialog.type(), dialog.writingSystem() );
        populateWritingSystemCombo();
        ui->itemWritingSystemsCombo->setCurrentText( dialog.writingSystem().name() );
    }
}

void ViewConfigurationDialog::removeItem()
{
    if( mTab == nullptr || ui->itemView->selectionModel()->selectedIndexes().count() == 0 ) return;
    mItemsModel->removeItem( currentWritingSystem(), ui->itemView->selectionModel()->selectedIndexes().first() );
}

void ViewConfigurationDialog::addPhrasalGloss()
{
    if( mPhrasalGlossesModel == nullptr ) return;
    WritingSystemDialog dialog(mProject->dbAdapter()->writingSystems(), this);
    dialog.exec();
    if( dialog.result() == QDialog::Accepted )
    {
        mPhrasalGlossesModel->addPhrasalGloss( dialog.writingSystem() );
    }
}

void ViewConfigurationDialog::removePhrasalGloss()
{
    if( mPhrasalGlossesModel == nullptr || ui->phrasalGlossView->selectionModel()->selectedRows().isEmpty() ) return;
    mPhrasalGlossesModel->removePhrasalGloss( ui->phrasalGlossView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::viewChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    if( ! selected.indexes().isEmpty() )
    {
        int currentView = selected.indexes().first().row();
        if( mTabsModel != nullptr )
        {
            delete mTabsModel;
        }

        mView = mProject->views()->at( currentView );
        mTabsModel = new TabsModel( mView );
        ui->tabView->setModel(mTabsModel);
        connect(ui->tabView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(tabChanged(QItemSelection,QItemSelection)) );

        setTabWidgetsEnabled(true);
        if( ui->tabView->selectionModel()->selectedRows().count() == 0 )
        {
            setItemWidgetsEnabled(false);
        }
    }
    else
    {
        setTabWidgetsEnabled(false);
    }
}

void ViewConfigurationDialog::populateWritingSystemCombo()
{
    ui->itemWritingSystemsCombo->clear();

    QList<WritingSystem> availableWS = mTab->interlinearLineKeys();
    for(int i=0; i<availableWS.count(); i++)
    {
        ui->itemWritingSystemsCombo->addItem( availableWS.at(i).name() );
    }

    ui->itemWritingSystemsCombo->setEnabled( ui->itemWritingSystemsCombo->count() > 0 );

    WritingSystem currentWS = currentWritingSystem();
    if( mItemsModel != nullptr )
    {
        delete mItemsModel;
    }
    mItemsModel = new ItemsModel(mTab,currentWS);
    ui->itemView->setModel(mItemsModel);
}

void ViewConfigurationDialog::tabChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    if( ! selected.indexes().isEmpty() &&  !ui->viewView->selectionModel()->selectedRows().isEmpty() && mView != nullptr )
    {
        int currentTab = selected.indexes().first().row();
        mTab = mView->tabs()->at(currentTab);

        populateWritingSystemCombo();


        if( mPhrasalGlossesModel != nullptr )
        {
            delete mPhrasalGlossesModel;
        }
        mPhrasalGlossesModel = new PhrasalGlossesModel(mTab);
        ui->phrasalGlossView->setModel(mPhrasalGlossesModel);

        setItemWidgetsEnabled(true);
    }
    else
    {
        setItemWidgetsEnabled(false);
    }
}

void ViewConfigurationDialog::indexLanguageChanged(int index)
{
    Q_UNUSED(index);
    WritingSystem currentWS = currentWritingSystem();
    if( mItemsModel != nullptr )
    {
        delete mItemsModel;
    }
    mItemsModel = new ItemsModel(mTab,currentWS);
    ui->itemView->setModel(mItemsModel);
}

void ViewConfigurationDialog::editPhrasalGloss(const QModelIndex &index)
{
    WritingSystemDialog dialog(mProject->dbAdapter()->writingSystems(), this);
    dialog.exec();
    if( dialog.result() == QDialog::Accepted )
    {
        mPhrasalGlossesModel->editPhrasalGloss( index, dialog.writingSystem() );
    }
}

void ViewConfigurationDialog::editItem(const QModelIndex &index)
{
    if( mTab == nullptr ) return;
    ItemEditDialog dialog(mProject->dbAdapter()->writingSystems(), this);
    dialog.setWritingSystem( currentWritingSystem(), false );
    dialog.setType( mTab->interlinearLines(currentWritingSystem())->at( index.row() ) );
    dialog.exec();
    if( dialog.result() == QDialog::Accepted )
    {
        mItemsModel->editItem(currentWritingSystem(), index, dialog.type() );
    }
}

WritingSystem ViewConfigurationDialog::currentWritingSystem() const
{
    if( ui->itemWritingSystemsCombo->count() == 0 || mTab == nullptr )
    {
        return WritingSystem();
    }
    else
    {
        return mTab->interlinearLineKeys().at( ui->itemWritingSystemsCombo->currentIndex() );
    }
}

void ViewConfigurationDialog::viewUp()
{
    if(mViewModel == nullptr || ui->viewView->selectionModel()->selectedRows().count() == 0 ) return;
    mViewModel->moveUp( ui->viewView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::viewDown()
{
    if(mViewModel == nullptr || ui->viewView->selectionModel()->selectedRows().count() == 0 ) return;
    mViewModel->moveDown( ui->viewView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::tabUp()
{
    if(mTabsModel == nullptr || ui->tabView->selectionModel()->selectedRows().count() == 0 ) return;
    mTabsModel->moveUp( ui->tabView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::tabDown()
{
    if(mTabsModel == nullptr || ui->tabView->selectionModel()->selectedRows().count() == 0 ) return;
    mTabsModel->moveDown( ui->tabView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::itemUp()
{
    if(mItemsModel == nullptr || ui->itemView->selectionModel()->selectedRows().count() == 0 ) return;
    mItemsModel->moveUp( currentWritingSystem(), ui->itemView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::itemDown()
{
    if(mItemsModel == nullptr || ui->itemView->selectionModel()->selectedRows().count() == 0 ) return;
    mItemsModel->moveDown( currentWritingSystem(), ui->itemView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::phrasalGlossUp()
{
    if(mPhrasalGlossesModel == nullptr || ui->phrasalGlossView->selectionModel()->selectedRows().count() == 0 ) return;
    mPhrasalGlossesModel->moveUp( ui->phrasalGlossView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::phrasalGlossDown()
{
    if(mPhrasalGlossesModel == nullptr || ui->phrasalGlossView->selectionModel()->selectedRows().count() == 0 ) return;
    mPhrasalGlossesModel->moveDown( ui->phrasalGlossView->selectionModel()->selectedRows().first().row() );
}

void ViewConfigurationDialog::setTabWidgetsEnabled(bool enabled)
{
    ui->tabDown->setEnabled(enabled);
    ui->tabUp->setEnabled(enabled);
    ui->tabView->setEnabled(enabled);
    ui->addTab->setEnabled(enabled);
    ui->removeTab->setEnabled(enabled);
}

void ViewConfigurationDialog::setItemWidgetsEnabled(bool enabled)
{
    ui->itemWritingSystemsCombo->setEnabled(enabled);
    ui->itemView->setEnabled(enabled);
    ui->itemUp->setEnabled(enabled);
    ui->itemDown->setEnabled(enabled);
    ui->addItem->setEnabled(enabled);
    ui->removeItem->setEnabled(enabled);

    ui->phrasalGlossView->setEnabled(enabled);
    ui->phrasalGlossUp->setEnabled(enabled);
    ui->phrasalGlossDown->setEnabled(enabled);
    ui->addPhrasalGloss->setEnabled(enabled);
    ui->removePhrasalGloss->setEnabled(enabled);
}

void ViewConfigurationDialog::accept()
{
    if( viewsAreAcceptable() )
    {
        QDialog::accept();
    }
}

bool ViewConfigurationDialog::viewsAreAcceptable()
{
    if( mProject->views()->isEmpty() )
    {
        QMessageBox::warning(this, tr("Error"), tr("You need at least one view.") );
        return false;
    }

    for(int i=0; i < mProject->views()->count(); i++)
    {
        View * v = mProject->views()->at(i);

        if( v->tabs()->isEmpty() )
        {
            QMessageBox::warning(this, tr("Error"), tr("Each view needs at least one tab for the view '%1' (or just delete the view).").arg(v->name()) );
            return false;
        }

        for(int j=0; j < v->tabs()->count(); j++)
        {
            Tab * t = v->tabs()->at(j);
            QList<WritingSystem> keys = t->interlinearLineKeys();

            if( keys.isEmpty() )
            {
                QMessageBox::warning(this, tr("Error"), tr("You need to have at least one entry for the view, '%1' and the tab '%2.'").arg(v->name()).arg(t->name()) );
                return false;
            }

            for(int k=0; k < keys.count(); k++)
            {
                if( t->interlinearLines(keys.at(k))->isEmpty() )
                {
                    QMessageBox::warning(this, tr("Error"), tr("For each tab and baseline type, you need to have at least one interlinear line for the view '%1,' tab  '%2,' and baseline '%3.'").arg(v->name()).arg(t->name()).arg(keys.at(k).name()) );
                    return false;
                }
            }
        }
    }
    return true;
}
