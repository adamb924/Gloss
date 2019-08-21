#ifndef PUNCTUATIONDISPLAYWIDGET_H
#define PUNCTUATIONDISPLAYWIDGET_H

#include "immutablelabel.h"

class DatabaseAdapter;

class PunctuationDisplayWidget : public ImmutableLabel
{
    Q_OBJECT
public:
    PunctuationDisplayWidget(GlossItem * glossItem, const DatabaseAdapter * dbAdapter, QWidget * parent = nullptr);

private:
    GlossItem * mGlossItem;
    const DatabaseAdapter *mDbAdapter;

    void contextMenuEvent ( QContextMenuEvent * event );

signals:
    void requestNewPhraseFromHere( GlossItem * glossItem );
    void requestNoPhraseFromHere( GlossItem * glossItem );

private slots:
    void updateDisplay();
    void beginNewPhraseHere();
    void noNewPhraseHere();
    void editBaselineTextForm();
    void editBaselineText();
};

#endif // PUNCTUATIONDISPLAYWIDGET_H
