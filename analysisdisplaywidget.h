/*!
  \class AnalysisDisplayWidget
  \ingroup GUI
  \brief This is a subclass of InterlinearDisplayWidget for displaying an interlinear text for doing morphological analysis.
*/

#ifndef ANALYSISINTERLINEARDISPLAYWIDGET_H
#define ANALYSISINTERLINEARDISPLAYWIDGET_H

#include <QScrollArea>

#include "flowlayout.h"
#include "textbit.h"
#include "interlinearitemtype.h"

#include "interlineardisplaywidget.h"

#include <QList>

class Text;
class Project;
class WordDisplayWidget;
class GlossItem;
class AnalysisWidget;
class DatabaseAdapter;

class AnalysisDisplayWidget : public InterlinearDisplayWidget
{
    Q_OBJECT
public:
    AnalysisDisplayWidget(Text *text, Project *project, QWidget *parent = 0);

private:
    //! \brief Removes and deletes all widgets from the given \a layout
    void clearWidgets(QLayout * layout);

    //! \brief Add the word display widgets for phrase \a i to \a flowLayout
    void addWordWidgets( int i , QLayout * flowLayout );

    WordDisplayWidget* addWordDisplayWidget(GlossItem *item);

    void setLayoutFromText();

    //! \brief A concordance of analysis widgets, indexed by TextForm id
    QMultiHash<qlonglong,WordDisplayWidget*> mAnalysisWidgetConcordance;

    DatabaseAdapter *mDbAdapter;

public slots:
    void updateAnalysis(qlonglong textFormId);
};

#endif // ANALYSISINTERLINEARDISPLAYWIDGET_H
