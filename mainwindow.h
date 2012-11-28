#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Project;
class QAction;

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

private slots:
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();

    void sqlTableView( QAction * action );

    void addBlankText();
    void importFlexText();
};

#endif // MAINWINDOW_H
