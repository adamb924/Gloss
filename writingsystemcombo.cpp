#include "writingsystemcombo.h"

#include "databaseadapter.h"

WritingSystemCombo::WritingSystemCombo(QWidget *parent) :
    QComboBox(parent)
{
    connect( this, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)) );
}

void WritingSystemCombo::setWritingSystems(const QList<WritingSystem> & writingSystems)
{
    mWritingSystems = writingSystems;
    for(int i=0; i<mWritingSystems.count(); i++)
        addItem( tr("%1 (%2)").arg( mWritingSystems.at(i).name() ).arg( mWritingSystems.at(i).abbreviation() ) );
}

void WritingSystemCombo::setCurrentWritingSystem(const WritingSystem & ws)
{
    setCurrentIndex( mWritingSystems.indexOf(ws) );
}

void WritingSystemCombo::indexChanged( int index )
{
    if( index < mWritingSystems.count() )
    {
        emit writingSystemSelected( mWritingSystems.at(index).id() );
        emit writingSystemSelected( mWritingSystems.at(index) );
    }
}

WritingSystem WritingSystemCombo::currentWritingSystem() const
{
    return mWritingSystems.at( currentIndex() );
}
