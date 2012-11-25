#ifndef INTERLINEARDISPLAYWIDGET_H
#define INTERLINEARDISPLAYWIDGET_H

#include <QWidget>
#include <QList>

class QVBoxLayout;

#include "project.h"
#include "flowlayout.h"
#include "textbit.h"

class InterlinearDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    explicit InterlinearDisplayWidget(WritingSystem *baselineWs, Project::BaselineMode baselineMode, Project *project, QWidget *parent = 0);

    WritingSystem* writingSystem() const;
    void setWritingSystem(WritingSystem *ws);

signals:

public slots:
    void setText(const QString& text);

private:
    Project *mProject;

    WritingSystem *mWritingSystem; // for the baseline text

    QString mText;

    void setLayoutFromText();

    QVBoxLayout *mLayout;

    Project::BaselineMode mBaselineMode;

    QList< QList<TextBit*>* > mTextBits;

    QList<FlowLayout*> mLineLayouts;
    QList<QWidget*> mWordDisplayWidgets;
};

#endif // INTERLINEARDISPLAYWIDGET_H
