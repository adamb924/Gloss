#ifndef SYNTACTICPARSINGWIDGET_H
#define SYNTACTICPARSINGWIDGET_H

#include <QWidget>

class QGraphicsScene;

class Text;
class Tab;
class Project;
class QGraphicsItem;

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
    void setupLayout();

private slots:

private:
    Ui::SyntacticParsingWidget *ui;
    Text *mText;
    const Tab *mTab;
    const Project *mProject;

    QList<QGraphicsItem*> mPreviouslySelectedItems;

    int mInterMorphemeDistance;
    int mInterWordDistance;
    int mVerticalDistance;

    QGraphicsScene *mScene;
};

#endif // SYNTACTICPARSINGWIDGET_H
