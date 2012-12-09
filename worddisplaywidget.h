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

class WordDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    WordDisplayWidget(GlossItem *item, Qt::Alignment alignment, InterlinearDisplayWidget *ildw, DatabaseAdapter *dbAdapter);

    QHash<qlonglong, LingEdit*> textFormEdits() const;
    QHash<qlonglong, LingEdit*> glossEdits() const;

private:
    GlossItem *mGlossItem;
    Qt::Alignment mAlignment;

    InterlinearDisplayWidget *mInterlinearDisplayWidget;

    Qt::Alignment calculateAlignment( Qt::LayoutDirection match , Qt::LayoutDirection current ) const;

    void contextMenuEvent ( QContextMenuEvent * event );
    void addInterpretationSubmenu(QMenu *menu );
    void addTextFormSubmenu(QMenu *menu, const WritingSystem & writingSystem );
    void addGlossSubmenu(QMenu *menu, const WritingSystem & writingSystem );

    void setupLayout();
    LingEdit* addGlossLine( const InterlinearItemType & glossLine );
    LingEdit* addTextFormLine( const InterlinearItemType & glossLine );

    QVBoxLayout *mLayout;

    QLabel *mBaselineWordLabel;
    QList<InterlinearItemType> mGlossLines;
    QHash<WritingSystem, LingEdit*> mTextFormEdits;
    QHash<WritingSystem, LingEdit*> mGlossEdits;

    void mouseDoubleClickEvent ( QMouseEvent * event );

    DatabaseAdapter *mDbAdapter;

signals:
    void textFormIdChanged(LingEdit * edit, qlonglong newId);
    void glossIdChanged(LingEdit * edit, qlonglong newId);

public slots:
    void updateBaselineLabelStyle();

private slots:
    void newInterpretation();
    void newGloss(QAction *action);
    void newTextForm(QAction *action);
    void fillData();

    void selectDifferentCandidate(QAction *action);
    void selectDifferentGloss(QAction *action);
    void selectDifferentTextForm(QAction *action);

};

#endif // WORDDISPLAYWIDGET_H
