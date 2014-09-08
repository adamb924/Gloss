#ifndef SYNTACTICANALYSISELEMENTMIME_H
#define SYNTACTICANALYSISELEMENTMIME_H

#include <QMimeData>

class SyntacticAnalysisElement;

class SyntacticAnalysisElementMime : public QMimeData
{
    Q_OBJECT
public:
    SyntacticAnalysisElementMime(SyntacticAnalysisElement * element);

    SyntacticAnalysisElement * element() const;

signals:

public slots:

private:
    SyntacticAnalysisElement * mElement;

};

#endif // SYNTACTICANALYSISELEMENTMIME_H
