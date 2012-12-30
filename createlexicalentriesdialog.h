#ifndef CREATELEXICALENTRIESDIALOG_H
#define CREATELEXICALENTRIESDIALOG_H

#include <QDialog>

class DatabaseAdapter;

#include "allomorph.h"

class CreateLexicalEntriesDialog : public QDialog
{
    Q_OBJECT
public:
    CreateLexicalEntriesDialog(const TextBit & parseString, DatabaseAdapter *dbAdapter, QWidget *parent = 0);

signals:

public slots:

private:
    DatabaseAdapter *mDbAdapter;
    TextBit mParseString;
    MorphologicalAnalysis mAnalysis;

    void fillMorphologicalAnalysis();
    void setupLayout();

private slots:
    void commitChangesToDatabase();

};

#endif // CREATELEXICALENTRIESDIALOG_H
