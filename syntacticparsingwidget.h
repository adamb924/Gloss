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

namespace Ui {
class SyntacticParsingWidget;
}

class SyntacticParsingWidget : public QWidget
{
    Q_OBJECT

public:
    SyntacticParsingWidget(Text *text,  const Tab * tab, const Project * project, QWidget *parent = 0);
    ~SyntacticParsingWidget();

private:
    void setupBaseline();
    void redrawSyntacticAnnotations();
    QGraphicsItem* addElementToScene(SyntacticAnalysisElement *element );

    QList<SyntacticAnalysisElement *> selectedElements();

    void contextMenuEvent ( QContextMenuEvent * event );

protected:
    void keyReleaseEvent(QKeyEvent * event);

private slots:
    void createConstituent();
    void analysisSelectionChanged(const QString & newSelection);
    void newAnalysis();
    void deleteAnalysis();
    void saveText();
    void saveTextVerbose();

private:
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
