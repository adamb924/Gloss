#ifndef PROJECTOPTIONSDIALOG_H
#define PROJECTOPTIONSDIALOG_H

#include <QDialog>

namespace Ui {
class ProjectOptionsDialog;
}

class WritingSystem;
class Project;
class WritingSystemListModel;
class AnnotationTypeListModel;

class ProjectOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    ProjectOptionsDialog(Project * project, QWidget *parent = 0);
    ~ProjectOptionsDialog();

private slots:
    void mediaFolderChooser();

    void addAnnotation();
    void removeAnnotation();
    void editAnnotation(const QModelIndex & index);

    void addCitation();
    void removeCitation();
    void editCitation(const QModelIndex & index);

    void addGloss();
    void removeGloss();
    void editGloss(const QModelIndex & index);

private:
    Ui::ProjectOptionsDialog *ui;
    Project * mProject;

    WritingSystemListModel * mCitationModel;
    WritingSystemListModel * mGlossModel;
    AnnotationTypeListModel * mAnnotationTypeModel;
};

#endif // PROJECTOPTIONSDIALOG_H
