#ifndef WORDDISPLAYWIDGET_H
#define WORDDISPLAYWIDGET_H

#include <QWidget>
#include <QString>
#include <QList>

#include "glossline.h"
#include "lingedit.h"
#include "text.h"

class Project;
class QVBoxLayout;
class WritingSystem;
class TextBit;

class WordDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    WordDisplayWidget(TextBit *bit, Text::BaselineMode baselineMode, Project *project);

    QSize sizeHint() const;


    TextBit* textBit() const;

protected:
    Project *mProject;
    TextBit *mTextBit;

    Text::BaselineMode mBaselineMode;


    void fillData();

    LingEdit* getAppropriateEdit(const TextBit & bit, GlossLine::LineType type );

private:
    void contextMenuEvent ( QContextMenuEvent * event );

    void setupLayout();

    QVBoxLayout *mLayout;

    QList<GlossLine> mGlossLines;
    QList<LingEdit*> mEdits;

signals:
    void idChanged(WordDisplayWidget *w, qlonglong oldId, qlonglong newId );

    void glossChanged(const TextBit & bit);
    void orthographyChanged(const TextBit & bit);
    void transcriptionChanged(const TextBit & bit);

private slots:
    void newGloss();

public slots:
    void updateEdit( const TextBit & bit, GlossLine::LineType type );
};

#endif // WORDDISPLAYWIDGET_H
