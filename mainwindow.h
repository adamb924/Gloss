/*!
  \class MainWindow
  \ingroup GUI
  \brief The main window of the application. A MainWindow can have a single project open at a time. Subwindows are texts of an opened project.  The layout is mainwindow.ui.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Project;
class QAction;
class WritingSystem;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Project *mProject;

    void addTableMenuItems();

    void setProjectActionsEnabled(bool enabled);

    void closeEvent(QCloseEvent *event);
    bool maybeSave();

    WritingSystem selectWritingSystem(bool *ok);

private slots:
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();
    void closeProject();
    void openText();

    void openText(const QString & textName);

    void projectClose();

    void sqlTableView( QAction * action );

    void addBlankText();
    void importFlexText();
    void importPlainText();
    void importPlainText(const QString & filepath , const WritingSystem & ws, bool openText);
};

#endif // MAINWINDOW_H
