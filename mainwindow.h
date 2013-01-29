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
class QModelIndex;
class TextDisplayWidget;
class QStandardItemModel;

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

    WritingSystem selectWritingSystem(const QString & message, bool *ok);

    void createSearchResultDock(QStandardItemModel * model, const QString & reminder);

    QStringList textsWithOpenWindows();

public slots:
    void searchForInterpretationById(qlonglong id);
    void searchForTextFormById(qlonglong id);
    void searchForGlossById(qlonglong id);

private slots:
    void rebuildIndex();

    void newProject();
    void openProject();
    void saveProject();
    void closeProject();
    void closeProjectWithoutSaving();
    void openText();
    void deleteText();
    void mergeTranslations();

    TextDisplayWidget* openText(const QString & textName);

    void projectClose();

    void sqlTableView( QAction * action );

    void writingSystems();

    void addBlankText();
    void importFlexText();
    void importPlainText();
    void importPlainText(const QString & filepath , const WritingSystem & ws, bool openText);

    void searchGlossItems();
    void substringSearchGlossItems();

    void searchForInterpretationById();
    void searchForTextFormById();
    void searchForGlossById();


    void searchResultSelected( const QModelIndex & index );
    void focusTextPosition( const QString & textName , int lineNumber );
    void playSoundForLine( const QString & textName , int lineNumber );
    void editLine( const QString & textName , int lineNumber );
    void editLineWithContext( const QString & textName , int lineNumber );
    void rawXQuery();
    void removeUnusedGlossItems();
    void sqlQueryDialog();
    void bulkMergeTranslations();
    void mergeEaf();
    void bulkMergeEaf();
    void setTextXmlFromDatabase();

    void generateTextReport();
    void countTextForms();
    void countGlosses();
    void createCountReport(const QString & typeString);

    void searchAndReplace();

    void findApprovedLines();
    void findUnapprovedLines();

    void closeOpenTexts();
    void openTextLine();
    void openTextLineWithContext();

    void editLexicon();
};

#endif // MAINWINDOW_H
