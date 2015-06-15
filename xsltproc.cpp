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

#include <QList>
#include <QtDebug>

Xsltproc::Xsltproc() :
    mErrorRedirect(false)
{
    exsltRegisterAll();
    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;
    mStylesheet = 0;
    mOutput = 0;
    mXml = 0;
    mParams = 0;
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
    for(int i=0; i<mNParams; i++)
    {
        delete mParams[i];
    }
    if( mParams != 0 ) delete mParams;
    mNParams = parameters.count();
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
    mParams[i] = NULL;
}

Xsltproc::ReturnValue Xsltproc::execute()
{
    Xsltproc::ReturnValue retval = Xsltproc::Success;
    try
    {
        if( freopen(mErrorFilename.toUtf8().data(),"w",stderr) == NULL ) throw Xsltproc::GenericFailure;

        mStylesheet = xsltParseStylesheetFile( (const xmlChar*)mStyleSheetFilename.toUtf8().data() );
        if(mStylesheet == 0) throw Xsltproc::InvalidStylesheet;

        mXml = xmlParseFile( (const char*)mXmlFilename.toUtf8().data() );
        if(mXml == 0) throw Xsltproc::InvalidXmlFile;

        mOutput = xsltApplyStylesheet(mStylesheet, mXml, (const char**)mParams);
        if(mOutput == 0) throw Xsltproc::GenericFailure;

        FILE *foutput = 0;
        foutput = fopen(mOutputFilename.toUtf8().data(),"w");
        if( foutput == 0 ) throw Xsltproc::CouldNotOpenOutput;
        xsltSaveResultToFile(foutput, mOutput, mStylesheet);
        fclose(foutput);
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
    if( mStylesheet != 0 )
    {
        xsltFreeStylesheet(mStylesheet);
        mStylesheet = 0;
    }
    if( mOutput != 0 )
    {
        xmlFreeDoc(mOutput);
        mOutput = 0;
    }
    if( mXml != 0 )
    {
        xmlFreeDoc(mXml);
        mXml=0;
    }
}

void Xsltproc::setErrorFilename(const QString & filename)
{
    mErrorFilename = filename;
}
