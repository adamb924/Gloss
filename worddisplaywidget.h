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
class InterlinearDisplayWidget;
class ImmutableLabel;
class AnalysisWidget;

class WordDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    WordDisplayWidget(GlossItem *item, Qt::Alignment alignment, const QList<InterlinearItemType> & lines, InterlinearDisplayWidget *ildw, DatabaseAdapter *dbAdapter);
    ~WordDisplayWidget();

    QHash<qlonglong, LingEdit*> textFormEdits() const;
    QHash<qlonglong, LingEdit*> glossEdits() const;
    GlossItem* glossItem();

    void refreshMorphologicalAnalysis(const WritingSystem & ws);

private:
    GlossItem *mGlossItem;
    Qt::Alignment mAlignment;

    InterlinearDisplayWidget *mInterlinearDisplayWidget;

    void contextMenuEvent ( QContextMenuEvent * event );
    void addInterpretationSubmenu(QMenu *menu );
    void addTextFormSubmenu(QMenu *menu, const WritingSystem & writingSystem );
    void addGlossSubmenu(QMenu *menu, const WritingSystem & writingSystem );

    void setupLayout();
    LingEdit* addGlossLine( const InterlinearItemType & glossLine );
    LingEdit* addTextFormLine( const InterlinearItemType & glossLine );
    ImmutableLabel* addImmutableLine( const InterlinearItemType & glossLine, bool technicolor );
    AnalysisWidget* addAnalysisWidget( const InterlinearItemType & glossLine );

    QVBoxLayout *mLayout;

    QLabel *mBaselineWordLabel;
    QList<InterlinearItemType> mGlossLines;
    QHash<WritingSystem, LingEdit*> mTextFormEdits;
    QHash<WritingSystem, LingEdit*> mGlossEdits;
    QHash<WritingSystem, AnalysisWidget*> mAnalysisWidgets;

    void mouseDoubleClickEvent ( QMouseEvent * event );

    DatabaseAdapter *mDbAdapter;

signals:
    void textFormIdChanged(LingEdit * edit, qlonglong newId);
    void glossIdChanged(LingEdit * edit, qlonglong newId);
    void alternateInterpretationAvailableFor( int id );
//    void analysisChanged(AnalysisWidget * analysisWidget, qlonglong textFormId);
    void morphologicalAnalysisChanged(qlonglong textFormId);

public slots:
    void sendConcordanceUpdates();

private slots:
    void newInterpretation();


    void newGloss(QAction *action);
    void newTextForm(QAction *action);

    void newGloss(const WritingSystem & ws);
    void newTextForm(const WritingSystem & ws);

    void copyGlossFromBaseline(QAction *action);
    void copyTextFormFromBaseline(QAction *action);

    void fillData();

    void otherInterpretation();

    void selectDifferentCandidate(QAction *action);
    void selectDifferentGloss(QAction *action);
    void selectDifferentTextForm(QAction *action);

    void displayDatabaseReport();

    void editBaselineText();

};

#endif // WORDDISPLAYWIDGET_H
