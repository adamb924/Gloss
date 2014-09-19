#ifndef SYNTACTICANALYSISELEMENTMIME_H
#define SYNTACTICANALYSISELEMENTMIME_H

#include <QMimeData>
#include <QList>

class SyntacticAnalysisElement;

class SyntacticAnalysisElementMime : public QMimeData
{
    Q_OBJECT
public:
    SyntacticAnalysisElementMime(SyntacticAnalysisElement * elements);
    SyntacticAnalysisElementMime(const QList<SyntacticAnalysisElement *> & elements);

    QList<SyntacticAnalysisElement *> elements() const;

signals:

public slots:

private:
    QList<SyntacticAnalysisElement *> mElements;

};

#endif // SYNTACTICANALYSISELEMENTMIME_H
