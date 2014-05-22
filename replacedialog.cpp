#include "replacedialog.h"
#include "ui_replacedialog.h"

#include <QtWidgets>
#include <QXmlStreamWriter>
#include <QtDebug>

#include "databaseadapter.h"
#include "searchreplaceform.h"

ReplaceDialog::ReplaceDialog(DatabaseAdapter *dbAdapter, QWidget *parent) :
        QDialog(parent),
        ui(new Ui::ReplaceDialog)
{
    ui->setupUi(this);

    mDbAdapter = dbAdapter;

    // TODO the offending line:
//    mInterlinearLines = mDbAdapter->glossInterlinearLines();

    setWindowTitle(tr("Search & Replace"));

    connect(this,SIGNAL(accepted()), this, SLOT(generateXsl()));

    setupLayout();
}

ReplaceDialog::~ReplaceDialog()
{
    delete ui;
}

void ReplaceDialog::setupLayout()
{
    SearchReplaceForm *searchInterpretationForm = new SearchReplaceForm(tr("Interpretation Id"), InterlinearItemType( InterlinearItemType::Null, mDbAdapter->metaLanguage() ), mDbAdapter, this);
    mSearchForms << searchInterpretationForm;
    ui->searchLayout->addWidget( searchInterpretationForm );

    SearchReplaceForm *replaceInterpretationForm = new SearchReplaceForm(tr("Interpretation Id"), InterlinearItemType( InterlinearItemType::Null, mDbAdapter->metaLanguage() ), mDbAdapter, this);
    mReplaceForms << replaceInterpretationForm;
    ui->replaceLayout->addWidget( replaceInterpretationForm );

    connect( searchInterpretationForm, SIGNAL(checkboxChanged(bool)), replaceInterpretationForm, SLOT(setCheckboxChecked(bool)));
    connect( replaceInterpretationForm, SIGNAL(checkboxChanged(bool)), searchInterpretationForm, SLOT(setCheckboxChecked(bool)));

    foreach( InterlinearItemType line , mInterlinearLines )
    {
        SearchReplaceForm *searchForm = new SearchReplaceForm( tr("%1 %2 Id").arg(line.writingSystem().name()).arg( line.type() == InterlinearItemType::Gloss ? tr("Gloss") : tr("Text Form") ), InterlinearItemType( line.type(), line.writingSystem()), mDbAdapter, this);
        mSearchForms << searchForm;
        ui->searchLayout->addWidget( searchForm );

        SearchReplaceForm *replaceForm = new SearchReplaceForm( tr("%1 %2 Id").arg(line.writingSystem().name()).arg( line.type() == InterlinearItemType::Gloss ? tr("Gloss") : tr("Text Form") ), InterlinearItemType( line.type(), line.writingSystem()), mDbAdapter, this);
        mReplaceForms << replaceForm;
        ui->replaceLayout->addWidget( replaceForm );

        connect( searchForm, SIGNAL(checkboxChanged(bool)), replaceForm, SLOT(setCheckboxChecked(bool)));
        connect( replaceForm, SIGNAL(checkboxChanged(bool)), searchForm, SLOT(setCheckboxChecked(bool)));
    }
}

QString ReplaceDialog::xmlFilePath() const
{
    return mXmlFilePath;
}

void ReplaceDialog::generateXsl()
{
    mXmlFilePath = QDir::temp().absoluteFilePath( "search-replace.xml" );
    QFile out(mXmlFilePath);

    bool isCandidateReplacement = mSearchForms.first()->id() != -1;

    if( out.open(QFile::WriteOnly) )
    {
        QXmlStreamWriter writer( &out );
        writer.setCodec("UTF-8");
        writer.setAutoFormatting(true);

        writer.writeStartDocument();
        writer.writeStartElement("replacement-schema");
        writer.writeNamespace("http://www.adambaker.org/gloss.php", "abg");

        if( isCandidateReplacement )
        {
            writer.writeStartElement("interpretation");
            writer.writeAttribute("id", QString::number(mSearchForms.first()->id()) );

            for(int i=1; i<mSearchForms.count(); i++)
            {
                if( mSearchForms.at(i)->id() != -1 )
                {
                    writer.writeStartElement("item");
                    writer.writeAttribute("http://www.adambaker.org/gloss.php", "id", QString::number(mSearchForms.at(i)->id()) );
                    writer.writeAttribute("type", mSearchForms.at(i)->type().type() == InterlinearItemType::Gloss ? QString("gls") : QString("txt") );
                    writer.writeAttribute("lang", mSearchForms.at(i)->type().writingSystem().flexString() );

                    if( mSearchForms.at(i)->type().type() == InterlinearItemType::Gloss )
                        writer.writeCharacters( mDbAdapter->glossFromId( mSearchForms.at(i)->id() ).text() );
                    else
                        writer.writeCharacters( mDbAdapter->textFormFromId( mSearchForms.at(i)->id() ).text() );

                    writer.writeEndElement(); // item
                }
            }

            writer.writeStartElement("with");
            writer.writeAttribute("id", QString::number(mReplaceForms.first()->id()) );

            for(int i=1; i<mReplaceForms.count(); i++)
            {
                if( mReplaceForms.at(i)->id() != -1 )
                {
                    writer.writeStartElement("item");
                    writer.writeAttribute("http://www.adambaker.org/gloss.php", "id", QString::number(mReplaceForms.at(i)->id()) );
                    writer.writeAttribute("type", mReplaceForms.at(i)->type().type() == InterlinearItemType::Gloss ? QString("gls") : QString("txt") );
                    writer.writeAttribute("lang", mSearchForms.at(i)->type().writingSystem().flexString() );

                    if( mReplaceForms.at(i)->type().type() == InterlinearItemType::Gloss )
                        writer.writeCharacters( mDbAdapter->glossFromId( mReplaceForms.at(i)->id() ).text() );
                    else
                        writer.writeCharacters( mDbAdapter->textFormFromId( mReplaceForms.at(i)->id() ).text() );

                    writer.writeEndElement(); // item
                }
            }
            writer.writeEndElement(); // with

            writer.writeEndElement(); // interpretation
        }
        else
        {
            for(int i=1; i<mSearchForms.count(); i++)
            {
                if( mSearchForms.at(i)->id() != -1 )
                {
                    writer.writeStartElement("replace");
                    writer.writeAttribute("type", mSearchForms.at(i)->type().type() == InterlinearItemType::Gloss ? QString("gls") : QString("txt") );
                    writer.writeAttribute("id", QString::number(mSearchForms.at(i)->id()) );
                    writer.writeAttribute("lang", mSearchForms.at(i)->type().writingSystem().flexString());

                    writer.writeStartElement("with");
                    writer.writeAttribute("type", mReplaceForms.at(i)->type().type() == InterlinearItemType::Gloss ? QString("gls") : QString("txt") );
                    writer.writeAttribute("id", QString::number(mReplaceForms.at(i)->id()) );
                    writer.writeAttribute("lang", mSearchForms.at(i)->type().writingSystem().flexString());
                    if( mReplaceForms.at(i)->type().type() == InterlinearItemType::Gloss )
                        writer.writeCharacters( mDbAdapter->glossFromId( mReplaceForms.at(i)->id() ).text() );
                    else
                        writer.writeCharacters( mDbAdapter->textFormFromId( mReplaceForms.at(i)->id() ).text() );
                    writer.writeEndElement(); // with

                    writer.writeEndElement(); // replace
                }
            }
        }

        writer.writeEndElement(); // replacement-schema

        writer.writeEndDocument();

        out.close();
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("The replacement file could not be opened for some reason."));
    }

}
