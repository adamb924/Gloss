#ifndef GLOSSITEM_H
#define GLOSSITEM_H

#include <QObject>

class TextBit;

class GlossItem : public QObject
{
    Q_OBJECT
public:
    explicit GlossItem(TextBit *baselineText, QObject *parent = 0);

    void setId(qlonglong id);
    qlonglong id() const;

    TextBit* baselineText() const;

signals:

public slots:

private:
    qlonglong mId;
    TextBit *mBaselineText;
};

#endif // GLOSSITEM_H
