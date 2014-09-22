/*!
  \class SyntacticAnalysisElement
  \ingroup SyntaxGUI
  \brief This subclass of QMimeData provides a way to pass a list of SyntacticAnalysisElement objects pointers in mime data. It is used in ConstituentGraphicsItem and MorphemeGraphicsItem.
*/

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
