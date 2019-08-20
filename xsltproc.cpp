#include "xsltproc.h"

#include <string.h>
#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/DOCBparser.h>
#include <libxml/xinclude.h>
#include <libxml/catalog.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libexslt/exslt.h>

#include <QFileInfo>
#include <QList>
#include <QtDebug>

Xsltproc::Xsltproc()
{
    exsltRegisterAll();
    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;

    mStylesheet = nullptr;
    mOutput = nullptr;
    mXml = nullptr;

    mParams = nullptr;
    mNParams = 0;
}

Xsltproc::~Xsltproc()
{
    xsltCleanupGlobals();
    xmlCleanupParser();
}

void Xsltproc::setStyleSheet(const QString & filename)
{
    mStyleSheetFilename = filename;
}

void Xsltproc::setXmlFilename(const QString & filename)
{
    mXmlFilename = filename;
}

void Xsltproc::setOutputFilename(const QString & filename)
{
    mOutputFilename = filename;
}

void Xsltproc::setParameters(const QHash<QString,QString> & parameters)
{
    for(unsigned int i=0; i<mNParams; i++)
    {
        delete mParams[i];
    }
    if( mParams != nullptr ) delete mParams;
    mNParams = static_cast<unsigned int>(parameters.count());
    mParams = new char*[2*mNParams+1];

    QList<QByteArray*> byteArrays;
    QHashIterator<QString,QString> iter(parameters);
    int i=0;
    while(iter.hasNext())
    {
        iter.next();
        byteArrays << new QByteArray(iter.key().toUtf8());
        mParams[i++] = byteArrays.last()->data();
        QString paramValue = "\"" + iter.value() + "\"";
        byteArrays << new QByteArray(paramValue.toUtf8());
        mParams[i++] = byteArrays.last()->data();
    }
    mParams[i] = nullptr;
}

Xsltproc::ReturnValue Xsltproc::execute()
{
    Xsltproc::ReturnValue retval = Xsltproc::Success;
    try
    {
        if( freopen(mErrorFilename.toUtf8().data(),"w",stderr) == nullptr ) throw Xsltproc::GenericFailure;

        mStylesheet = xsltParseStylesheetFile( reinterpret_cast<const xmlChar*>(mStyleSheetFilename.toUtf8().data()) );
        if(mStylesheet == nullptr || getErrorFileHasContents())
            throw Xsltproc::InvalidStylesheet;

        mXml = xmlParseFile( static_cast<const char*>(mXmlFilename.toUtf8().data()) );
        if(mXml == nullptr || getErrorFileHasContents())
            throw Xsltproc::InvalidXmlFile;

        mOutput = xsltApplyStylesheet(mStylesheet, mXml, const_cast<const char**>(mParams) );
        if(mOutput == nullptr || getErrorFileHasContents())
            throw Xsltproc::ApplyStylesheetFailure;

        if( 0 == xsltSaveResultToFilename(mOutputFilename.toUtf8().data(), mOutput, mStylesheet, 0) )
            throw Xsltproc::CouldNotOpenOutput;
    }
    catch(Xsltproc::ReturnValue e)
    {
        retval = e;
    }

    fclose(stderr);
    freeResources();

    return retval;
}

void Xsltproc::freeResources()
{
    if( mStylesheet != nullptr )
    {
        xsltFreeStylesheet(mStylesheet);
        mStylesheet = nullptr;
    }
    if( mOutput != nullptr )
    {
        xmlFreeDoc(mOutput);
        mOutput = nullptr;
    }
    if( mXml != nullptr )
    {
        xmlFreeDoc(mXml);
        mXml=nullptr;
    }
}

bool Xsltproc::getErrorFileHasContents()
{
    fclose(stderr);
    QFileInfo info(mErrorFilename);
    if( info.size() > 0 )
    {
        return true;
    }
    else
    {
        if( freopen(mErrorFilename.toUtf8().data(),"w",stderr) == nullptr ) throw Xsltproc::GenericFailure;
        return false;
    }
}

void Xsltproc::setErrorFilename(const QString & filename)
{
    mErrorFilename = filename;
}

QDebug operator<<(QDebug dbg, const Xsltproc::ReturnValue &value)
{
    switch(value) {
    case Xsltproc::Success:
        dbg << "Xsltproc::Success";
        break;
    case Xsltproc::InvalidStylesheet:
        dbg << "Xsltproc::InvalidStylesheet";
        break;
    case Xsltproc::InvalidXmlFile:
        dbg << "Xsltproc::InvalidXmlFile";
        break;
    case Xsltproc::CouldNotOpenOutput:
        dbg << "Xsltproc::CouldNotOpenOutput";
        break;
    case Xsltproc::ApplyStylesheetFailure:
        dbg << "Xsltproc::ApplyStylesheetFailure";
        break;
    case Xsltproc::GenericFailure:
        dbg << "Xsltproc::GenericFailure";
        break;
    }
    return dbg;
}
