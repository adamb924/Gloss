/*!
  \class WordDisplayWidget
  \ingroup GUI
  \brief A widget for displaying a word in an InterlinearDisplayWidget. The corresponding data class is GlossItem.
*/

#ifndef WORDDISPLAYWIDGET_H
#define WORDDISPLAYWIDGET_H

#include <QWidget>
#include <QString>
#include <QList>
#include <QHash>

#include "interlinearitemtype.h"
#include "lingedit.h"
#include "text.h"

class DatabaseAdapter;
class QVBoxLayout;
class QLabel;
class WritingSystem;
class TextBit;
class ImmutableLabel;
class AnalysisWidget;
class Concordance;

class WordDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    WordDisplayWidget(GlossItem *item, Qt::Alignment alignment, const QList<InterlinearItemType> & lines, DatabaseAdapter *dbAdapter, QWidget *parent = 0);
    ~WordDisplayWidget();

    QHash<qlonglong, LingEdit*> textFormEdits() const;
    QHash<qlonglong, LingEdit*> glossEdits() const;
    GlossItem* glossItem();

    void refreshMorphologicalAnalysis(const WritingSystem & ws);
    void setFocused(bool isFocused);

private:
    GlossItem *mGlossItem;
    Concordance *mConcordance;
    Qt::Alignment mAlignment;

    void setupShortcuts();

    void keyPressEvent ( QKeyEvent * event );

    void contextMenuEvent ( QContextMenuEvent * event );
    void addInterpretationSubmenu(QMenu *menu );
    void addTextFormSubmenu(QMenu *menu, const WritingSystem & writingSystem );
    void addGlossSubmenu(QMenu *menu, const WritingSystem & writingSystem );
    void addSearchSubmenu(QMenu *menu);

    void cycleInterpretation();
    void cycleTextForm( const WritingSystem & ws );
    void cycleGloss( const WritingSystem & ws );

    void setupLayout();
    LingEdit* addGlossLine( const InterlinearItemType & glossLine );
    LingEdit* addTextFormLine( const InterlinearItemType & glossLine );
    ImmutableLabel* addImmutableTextFormLine( const InterlinearItemType & glossLine, bool technicolor );
    ImmutableLabel* addImmutableGlossLine( const InterlinearItemType & glossLine, bool technicolor );
    AnalysisWidget* addAnalysisWidget( const InterlinearItemType & glossLine );

    QVBoxLayout *mLayout;

    QLabel *mBaselineWordLabel;
    QList<InterlinearItemType> mGlossLines;
    QHash<WritingSystem, LingEdit*> mTextFormEdits;
    QHash<WritingSystem, LingEdit*> mGlossEdits;
    QHash<WritingSystem, ImmutableLabel*> mImmutableLines;
    QHash<WritingSystem, AnalysisWidget*> mAnalysisWidgets;

    void mouseDoubleClickEvent ( QMouseEvent * event );

    DatabaseAdapter *mDbAdapter;

signals:
    void splitWidgetInTwo( GlossItem *glossItem, const TextBit & wordOne, const TextBit & wordTwo );
    void splitWidget( GlossItem *glossItem, const QList<TextBit> & bits );

    void mergeGlossItemWithNext( GlossItem *glossItem );
    void mergeGlossItemWithPrevious( GlossItem *glossItem );

    void requestInterpretationSearch(qlonglong id);
    void requestTextFormSearch(qlonglong id);
    void requestGlossSearch(qlonglong id);

    void requestRemoveGlossItem( GlossItem *glossItem );

    void requestApproveLine( WordDisplayWidget * wdw );
    void requestLeftGlossItem( WordDisplayWidget * wdw );
    void requestRightGlossItem( WordDisplayWidget * wdw );
    void requestPlaySound( WordDisplayWidget * wdw );

private slots:
    void playSound();
    void approveLine();
    void rightGlossItem();
    void leftGlossItem();

    void textFormSearch(QAction * action);
    void glossSearch(QAction * action);
    void interpretationSearch(QAction * action);

    void guessGloss();
    void guessGloss( const WritingSystem & ws );

    void newInterpretation();
    void duplicateInterpretation();

    void newGloss(QAction *action);
    void newTextForm(QAction *action);

    void newGloss(const WritingSystem & ws);
    void newTextForm(const WritingSystem & ws);

    void copyGlossFromBaseline(QAction *action);
    void copyGlossFromBaseline(const WritingSystem & ws);
    void copyGlossFromBaseline();

    void removeGlossItem();

    void fillData();

    void selectDifferentCandidate(QAction *action);
    void selectDifferentGloss(QAction *action);
    void selectDifferentTextForm(QAction *action);

    void displayDatabaseReport();

    void editBaselineText();
    void editBaselineText(const QString & text);
    void editBaselineTextKeepAnnotations();
    void splitIntoMultipleWords();
    void mergeWithNext();
    void mergeWithPrevious();
};

#endif // WORDDISPLAYWIDGET_H
