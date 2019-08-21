/*!
  \class SyntacticParsingWidget
  \ingroup SyntaxGUI
  \brief This class provides a user interface for producing syntactic annotations. From the user's perspective, it is one of the tabs that appears in a text window. It is used in TextTabWidget.

  The associated UI file is syntacticparsingwidget.ui.
*/

#ifndef SYNTACTICPARSINGWIDGET_H
#define SYNTACTICPARSINGWIDGET_H

#include <QWidget>

class QGraphicsScene;

class Text;
class Tab;
class Project;
class QGraphicsItem;
class SyntacticAnalysis;
class SyntacticAnalysisElement;
class Allomorph;

#include "syntactictype.h"

namespace Ui {
class SyntacticParsingWidget;
}

class SyntacticParsingWidget : public QWidget
{
    Q_OBJECT

public:
    SyntacticParsingWidget(Text *text,  const Tab * tab, const Project * project, QWidget *parent = nullptr);
    ~SyntacticParsingWidget();

private:
    void setupBaseline();
    QGraphicsItem* addConstituentElementToScene(SyntacticAnalysisElement *element );

    QList<SyntacticAnalysisElement *> selectedElements();

    void contextMenuEvent ( QContextMenuEvent * event );

protected:
    void keyReleaseEvent(QKeyEvent * event);

private slots:
    void redrawSyntacticAnnotations();
    void createConstituent(const SyntacticType &type = SyntacticType());
    void removeConstituent();
    void analysisSelectionChanged(const QString & newSelection);
    void newAnalysis();
    void editAnalysis();
    void deleteAnalysis();
    void saveText();
    void saveTextVerbose();
    void refreshText();

private:
    void maybeDisable();

    Ui::SyntacticParsingWidget *ui;
    Text *mText;
    const Tab *mTab;
    const Project *mProject;

    SyntacticAnalysis * mAnalysis;

    QList<QGraphicsItem*> mConstiuencyItems;
    QHash<const Allomorph*,QGraphicsItem*> mGraphicsItemAllomorphHash;

    int mInterMorphemeDistance;
    int mInterWordDistance;
    int mVerticalDistance;

    QGraphicsScene *mScene;
};

#endif // SYNTACTICPARSINGWIDGET_H
