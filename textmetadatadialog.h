#ifndef TEXTMETADATADIALOG_H
#define TEXTMETADATADIALOG_H

#include <QDialog>

namespace Ui {
class TextMetadataDialog;
}

class Project;
class QDataWidgetMapper;

class TextMetadataDialog : public QDialog
{
    Q_OBJECT

public:
    TextMetadataDialog(Project * project, QWidget *parent = nullptr);
    ~TextMetadataDialog();

private:
    Ui::TextMetadataDialog *ui;
    Project * mProject;
    QDataWidgetMapper * mMapper;
};

#endif // TEXTMETADATADIALOG_H
