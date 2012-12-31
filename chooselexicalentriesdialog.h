#ifndef CHOOSELEXICALENTRIESDIALOG_H
#define CHOOSELEXICALENTRIESDIALOG_H

#include <QDialog>

class DatabaseAdapter;
class GlossItem;

#include "allomorph.h"

class ChooseLexicalEntriesDialog : public QDialog
{
    Q_OBJECT
public:
    ChooseLexicalEntriesDialog(const TextBit & parseString, GlossItem *glossItem, DatabaseAdapter *dbAdapter, QWidget *parent = 0);

signals:

public slots:

private:
    DatabaseAdapter *mDbAdapter;
    TextBit mParseString;
    MorphologicalAnalysis mAnalysis;

    void fillMorphologicalAnalysis();
    void setupLayout();
    GlossItem *mGlossItem;

private slots:
    void commitChangesToDatabase();

};

#endif // CHOOSELEXICALENTRIESDIALOG_H
