#include "shortcuteditordialog.h"
#include "ui_shortcuteditordialog.h"

#include <QtGui>
#include <QListView>
#include <QItemSelectionModel>

ShortcutEditorDialog::ShortcutEditorDialog(KeyboardShortcuts shortcuts, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShortcutEditorDialog),
    mShortcuts(shortcuts)
{
    ui->setupUi(this);

    ui->invalidKeystroke->hide();

    initialize();

    connect(ui->shortcutEdit, SIGNAL(textChanged(QString)), this, SLOT(setKeystroke(QString)));
    connect(ui->restoreDefaults, SIGNAL(clicked()), this, SLOT(restoreDefaults()));
}

ShortcutEditorDialog::~ShortcutEditorDialog()
{
    delete ui;
}

KeyboardShortcuts ShortcutEditorDialog::shortcuts() const
{
    return mShortcuts;
}

void ShortcutEditorDialog::itemChanged(const QModelIndex &current, const QModelIndex &previous)
{
    Q_UNUSED(previous);
    mCurrentCode = mModel->data(current).toString();
    ui->descriptionEdit->setText( mShortcuts.description(mCurrentCode) );
    ui->shortcutEdit->setText( mShortcuts.keysequence(mCurrentCode).toString() );
    ui->invalidKeystroke->setVisible( mShortcuts.keysequence(mCurrentCode).toString().isEmpty() );
}

void ShortcutEditorDialog::setKeystroke(const QString & text)
{
    QKeySequence ks( text );
    mShortcuts.setKeySequence( mCurrentCode, ks );
    ui->invalidKeystroke->setVisible( ks.toString().isEmpty() );
}

void ShortcutEditorDialog::initialize()
{
    mModel = new QStringListModel( mShortcuts.codes() );
    mModel->sort(0);
    ui->listView->setModel(mModel);
    connect(ui->listView->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(itemChanged(QModelIndex,QModelIndex)));
}

void ShortcutEditorDialog::restoreDefaults()
{
    mShortcuts.setDefaultShortcuts();
    initialize();
    ui->listView->setCurrentIndex( mModel->index(0,0) );
}
