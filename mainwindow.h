/*!
  \class MainWindow
  \ingroup GUI
  \brief The main window of the application.

  A MainWindow can have a single project open at a time.

  Subwindows are texts of an opened project. These would typically be TextTabWidget objects (opened with openText()), but also InterlinearChunkEditor objects (opened with various methods).

  The layout, containing the menu, is mainwindow.ui.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include "focus.h"

class Project;
class QAction;
class WritingSystem;
class QModelIndex;
class TextTabWidget;
class QStandardItemModel;
class InterlinearChunkEditor;
class Text;
class TextBit;
class QComboBox;

#include <QRegularExpression>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //! \brief This creates the list of available views in the views submenu (in the MainWindow menu bar).
    void setGuiElementsFromProject();

private:
    Ui::MainWindow *ui;

    QDockWidget * mSearchDock;
    QDockWidget * mAnnotationDock;

    QMenu *mInterlinearViewMenu;
    QMenu *mQuickViewMenu;

    QComboBox * mTextCombo;
    QComboBox * mViewCombo;
    QComboBox * mQuickViewCombo;

    Project *mProject;

    void setupToolbar();

    void addTableMenuItems();
    void addTableMenuItems(const QStringList & tableNames);
    void addMemoryModeMenuItems();

    void setProjectActionsEnabled(bool enabled);

    void closeEvent(QCloseEvent *event);
    bool maybeSave();

    WritingSystem selectWritingSystem(const QString & message, bool *ok);

    QStringList textsWithOpenWindows();

    void setAppropriateWindowTitle();

    /** @name Project Methods
     * These methods pertain to IO operations on projects, and project settings.
     */
    ///@{
private slots:
    void newProject();
    void openProject();
    void saveProject();
    void closeProject();
    void closeProjectWithoutSaving();
    void projectClose();
    int writingSystems();
    int projectOptions();
    void exportTexts();
    void syntacticElements();

    void viewChanged(int index);
    void quickViewChanged(int index);

    void setCurrentView(int index);
    void setCurrentQuickView(int index);

    void textMetadataDialog();

    ///@}

    /** @name Text Methods
     * These methods pertain to opening and importing texts.
     */
    ///@{
private slots:
    void importFlexText();
    void importPlainText();
    void importPlainText(const QString & filepath , const WritingSystem & ws, bool openText, const QRegularExpression & phraseDelimiter = QRegularExpression("[\\n\\r]+"), const QRegularExpression & paragraphDelimiter = QRegularExpression("\\n\\r?\\n\\r?") );
    void importEaf();
    bool importEaf(const QString & filepath, const QString & tierId, const WritingSystem & ws);
    void openText();
    void deleteText();
    void mergeTranslations();

    //! \brief Opens a text with a TextTabWidget object.
    TextTabWidget* openText(const QString & textName, const QList<Focus> & foci = QList<Focus>() );
    void bulkMergeTranslations();
    void mergeEaf();
    void bulkMergeEaf();
    void closeOpenTexts();
    void saveOpenTexts();
    InterlinearChunkEditor * openTextInChunks(const QString & textName, int linesPerScreen = 3);
    Text * textOfCurrentSubWindow();

    ///@}

    /** @name Search Methods
     * These methods pertain to the search functions.
     */
    ///@{
public slots:
    //! \brief Searches either the texts or the database index for the specified interpretation \a id, and puts the results in a dock.
    void searchForInterpretationById(qlonglong id);

    //! \brief Searches either the texts or the database index for the specified text form \a id, and puts the results in a dock.
    void searchForTextFormById(qlonglong id);

    //! \brief Searches either the texts or the database index for the specified gloss \a id, and puts the results in a dock.
    void searchForGlossById(qlonglong id);

    //! \brief Searches either the texts or the database index for the specified lexical entry \a id, and puts the results in a dock.
    void searchForLexicalEntryById(qlonglong id);

    //! \brief Searches either the texts or the database index for the specified allomorph \a id, and puts the results in a dock.
    void searchForAllomorphById(qlonglong id);

    //! \brief Searches either the texts or the database index for the specified text \a bit (text form or gloss), and puts the results in a dock.
    void searchForText(const TextBit & bit);

    //! \brief Substring-searches either the texts or the database index for the specified text \a bit (text form or gloss), and puts the results in a dock.
    void searchForSubstring(const TextBit & bit);

private slots:
    //! \brief Toggle the display of the search dock.
    void toggleSearchDock();

    //! \brief Toggle the display of the annotation dock.
    void toggleAnnotationDock();

    //! \brief Provide a dialog box for the user to perform an XQuery directly on the texts.
    void rawXQuery();

    //! \brief Rebuilds the database's indices from the XML files.
    void rebuildIndex();

    //! \brief Searches for approved lines and places the results in a search results dock.
    void findApprovedLines();

    //! \brief Searches for unapproved lines and places the results in a search results dock.
    void findUnapprovedLines();

    //! \brief Provide a dialog for the user to do a search-and-replace on the baseline texts of all open texts.
    void baselineSearchAndReplace();

private:
    void createSearchResultDock(QStandardItemModel * model, const QString & reminder);
    ///@}

    /** @name View Methods
     * These methods pertain to views.
     */
    ///@{
private slots:
    int viewConfigurationDialog();
    ///@}

    /** @name Report Methods
     * These methods pertain to the production of reports.
     */
    ///@{
private slots:
    ///@}

    /** @name Raw Access Methods
     * These methods pertain to raw database/XML access.
     */
    ///@{
private slots:
    void sqlQueryDialog();
    void sqlTableView( QAction * action );
    void generateTextReport();
    void countTextForms();
    void countGlosses();
    void createCountReport(const QString & typeString);
    ///@}

private slots:
    void focusTextPosition( const QString & textName , int lineNumber, const QList<Focus> & foci );
    void playSoundForLine( const QString & textName , int lineNumber );


    void removeUnusedGlossItems();
    void setTextXmlFromDatabase();



    void editLexicon();


    void setMemoryMode( QAction * action );

};

#endif // MAINWINDOW_H
