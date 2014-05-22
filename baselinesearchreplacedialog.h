#ifndef BASELINESEARCHREPLACEDIALOG_H
#define BASELINESEARCHREPLACEDIALOG_H

#include <QDialog>

#include "textbit.h"

namespace Ui {
class BaselineSearchReplaceDialog;
}

class DatabaseAdapter;

class BaselineSearchReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    BaselineSearchReplaceDialog(const DatabaseAdapter * dbAdapter, const WritingSystem & defaultWs, QWidget *parent = 0);
    ~BaselineSearchReplaceDialog();

    TextBit searchString() const;
    TextBit replaceString() const;

private:
    const DatabaseAdapter * mDbAdapter;
    Ui::BaselineSearchReplaceDialog *ui;

private slots:
    void offerSearchFeedback();
    void offerReplaceFeedback();
};

#endif // BASELINESEARCHREPLACEDIALOG_H
