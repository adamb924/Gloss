#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class Project;

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

public slots:
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();

    void addText();

};

#endif // MAINWINDOW_H
