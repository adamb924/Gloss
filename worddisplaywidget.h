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

#include "glossline.h"
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

    void updateEdit( const TextBit & bit, GlossLine::LineType type );

private:
    GlossItem *mGlossItem;
    Qt::Alignment mAlignment;

    InterlinearDisplayWidget *mInterlinearDisplayWidget;

    Qt::Alignment calculateAlignment( Qt::LayoutDirection match , Qt::LayoutDirection current ) const;

    void contextMenuEvent ( QContextMenuEvent * event );

    void setupLayout();

    QVBoxLayout *mLayout;

    QLabel *mBaselineWordLabel;
    QList<GlossLine> mGlossLines;
    QHash<WritingSystem, LingEdit*> mEdits;

    void mouseDoubleClickEvent ( QMouseEvent * event );

    DatabaseAdapter *mDbAdapter;

public slots:
    void updateBaselineLabelStyle();

private slots:
    void newGloss();
    void selectDifferentCandidate(QAction *action);
    void fillData();

signals:

};

#endif // WORDDISPLAYWIDGET_H
