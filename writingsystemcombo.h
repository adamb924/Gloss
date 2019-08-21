/*!
  \class WritingSystemCombo
  \ingroup GUI
  \brief This is a QComboBox subclass for selecting a WritingSystem.
*/

#ifndef WRITINGSYSTEMCOMBO_H
#define WRITINGSYSTEMCOMBO_H

#include <QComboBox>
#include <QList>

#include "writingsystem.h"

class DatabaseAdapter;

class WritingSystemCombo : public QComboBox
{
    Q_OBJECT
public:
    explicit WritingSystemCombo(QWidget *parent = nullptr);

    void setWritingSystems(const QList<WritingSystem> & writingSystems);
    void setCurrentWritingSystem(const WritingSystem & ws);

    WritingSystem currentWritingSystem() const;

signals:
    void writingSystemSelected( qlonglong wsId );
    void writingSystemSelected( const WritingSystem & ws );

private slots:
    void indexChanged( int index );

private:
    const DatabaseAdapter * mDbAdapter;
    QList<WritingSystem> mWritingSystems;
};

#endif // WRITINGSYSTEMCOMBO_H
