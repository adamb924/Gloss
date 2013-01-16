#ifndef GLOSSDISPLAYWIDGET_H
#define GLOSSDISPLAYWIDGET_H

#include "interlineardisplaywidget.h"

#include "interlinearitemtype.h"

class Project;
class Text;
class LingEdit;
class WordDisplayWidget;
class TextBit;
class GlossItem;
class Phrase;

#include <QList>
#include <QMultiHash>

class GlossDisplayWidget : public InterlinearDisplayWidget
{
    Q_OBJECT
public:
    GlossDisplayWidget(Text *text, Project *project, QWidget *parent = 0);
    GlossDisplayWidget(Text *text, Project *project, QList<int> lines, QWidget *parent = 0);
    ~GlossDisplayWidget();

public slots:

    void updateGloss( const TextBit & bit );
    void updateText( const TextBit & bit );

    void removeGlossFromConcordance( QObject * edit );
    void removeTextFormFromConcordance( QObject * edit );

    void otherInterpretationsAvailableFor(int id);

private slots:
    void baselineTextUpdated(const QString & baselineText);



private:
    void setLayoutFromText();
    void setLayoutFromText(QList<int> lines);

    //! \brief Removes and deletes all widgets from the given \a layout
    void clearWidgets(QLayout * layout);

    //! \brief Add the word display widgets for phrase \a i to \a flowLayout
    void addWordWidgets( int i , QLayout * flowLayout );

    void clearData();

    WordDisplayWidget* addWordDisplayWidget(GlossItem *item, Phrase *phrase);

    QMultiHash<qlonglong,WordDisplayWidget*> mWdwByInterpretationId; // change this to something generic?

};

#endif // GLOSSDISPLAYWIDGET_H
