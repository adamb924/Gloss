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

class Project;
class QVBoxLayout;
class WritingSystem;
class TextBit;
class InterlinearDisplayWidget;

class WordDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    WordDisplayWidget(GlossItem *item, Qt::Alignment alignment, InterlinearDisplayWidget *ildw, Project *project);

    GlossItem* glossItem() const;

    void updateEdit( const TextBit & bit, GlossLine::LineType type );

private:
    Project *mProject;
    GlossItem *mGlossItem;
    Qt::Alignment mAlignment;

    InterlinearDisplayWidget *mInterlinearDisplayWidget;

    void fillData();

    Qt::Alignment calculateAlignment() const;

    void contextMenuEvent ( QContextMenuEvent * event );

    void setupLayout();

    QVBoxLayout *mLayout;

    QList<GlossLine> mGlossLines;
    QHash<WritingSystem, LingEdit*> mEdits;

private slots:
    void newGloss();

signals:

};

#endif // WORDDISPLAYWIDGET_H
