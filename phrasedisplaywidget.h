#ifndef PHRASEDISPLAYWIDGET_H
#define PHRASEDISPLAYWIDGET_H

#include <QWidget>

#include <QList>
#include <QSet>
#include <QMultiHash>
#include "interlinearitemtype.h"

class Phrase;
class DatabaseAdapter;
class FlowLayout;
class WordDisplayWidget;
class GlossItem;
class Text;
class InterlinearDisplayWidget;
class LingEdit;

class PhraseDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    PhraseDisplayWidget(int index, Phrase *phrase, Text * text, InterlinearDisplayWidget *idw, DatabaseAdapter *dbAdapter,  QWidget *parent = 0);

signals:

public slots:
    void otherInterpretationsAvailableFor(int id);

private slots:
    void approveAll();
    void playSound();

private:
    //! \brief Add a line label for phrase \a i
    void addLineLabel();
    void addWordWidgets();
    WordDisplayWidget* addWordDisplayWidget(GlossItem *item);

    QList<InterlinearItemType> mInterlinearDisplayLines;
    QSet<QWidget*> mWordDisplayWidgets; // change to generic name
    QMultiHash<qlonglong,LingEdit*> mTextFormConcordance;
    QMultiHash<qlonglong,LingEdit*> mGlossConcordance;
    QMultiHash<qlonglong,WordDisplayWidget*> mWdwByInterpretationId; // change this to something generic?

    int mIndex;
    Phrase *mPhrase;
    DatabaseAdapter *mDbAdapter;
    FlowLayout *mFlowLayout;
    Text *mText;
    InterlinearDisplayWidget *mIdw;
};

#endif // PHRASEDISPLAYWIDGET_H
