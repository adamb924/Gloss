/*!
  \class MainWindow
  \ingroup GUI
  \brief The main window of the application. A MainWindow can have a single project open at a time. Subwindows are texts of an opened project.  The layout is mainwindow.ui.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include "focus.h"

class Project;
class QAction;
class WritingSystem;
class QModelIndex;
class TextDisplayWidget;
class QStandardItemModel;
class InterlinearChunkEditor;
class Text;
class TextBit;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void refreshViews();

private:
    Ui::MainWindow *ui;

    QDockWidget * mSearchDock;
    QDockWidget * mAnnotationDock;

    QMenu *mInterlinearViewMenu;
    QMenu *mQuickViewMenu;

    Project *mProject;

    void addTableMenuItems();
    void addMemoryModeMenuItems();

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
    void searchForLexicalEntryById(qlonglong id);
    void searchForAllomorphById(qlonglong id);
    void searchForText(const TextBit & bit);
    void searchForSubstring(const TextBit & bit);

private slots:
    void searchGlossItems();
    void substringSearchGlossItems();
    void searchForInterpretationById();
    void searchForTextFormById();
    void searchForGlossById();
    void searchForLexicalEntryById();
    void searchForAllomorphById();

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

    void toggleSearchDock();

    TextDisplayWidget* openText(const QString & textName, const QList<Focus> & foci = QList<Focus>() );

    void projectClose();

    void sqlTableView( QAction * action );

    void writingSystems();

    void addBlankText();
    void importFlexText();
    void importPlainText();
    void importPlainText(const QString & filepath , const WritingSystem & ws, bool openText);
    void importEaf();
    bool importEaf(const QString & filepath, const QString & tierId, const WritingSystem & ws, bool openText);


    void focusTextPosition( const QString & textName , int lineNumber, const QList<Focus> & foci );
    void playSoundForLine( const QString & textName , int lineNumber );
    void editLine( const QString & textName , int lineNumber, const QList<Focus> & foci );
    void editLineWithContext( const QString & textName , int lineNumber, const QList<Focus> & foci );
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
    void baselineSearchAndReplace();

    void findApprovedLines();
    void findUnapprovedLines();

    void closeOpenTexts();
    void saveOpenTexts();

    void editLexicon();
    void annotationDock();

    InterlinearChunkEditor * openTextInChunks(const QString & textName, int linesPerScreen);

    void setMemoryMode( QAction * action );

    Text * textOfCurrentSubWindow();
};

#endif // MAINWINDOW_H
