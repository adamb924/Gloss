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

Xsltproc::Xsltproc()
{
    exsltRegisterAll();
    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;

    mStylesheet = 0;
    mOutput = 0;
    mXml = 0;

    mErrorRedirect = false;
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
    mParameters = parameters;
}

Xsltproc::ReturnValue Xsltproc::execute()
{
    mStylesheet = xsltParseStylesheetFile( (const xmlChar*)mStyleSheetFilename.toUtf8().data() );
    if( mStylesheet == 0 )
        return Xsltproc::InvalidStylesheet;

    mXml = xmlParseFile( (const char*)mXmlFilename.toUtf8().data() );
    if( mXml == 0 )
        return Xsltproc::InvalidXmlFile;

    QList<QByteArray*> byteArrays;

    char *params[2*mParameters.count()+1];

    QHashIterator<QString,QString> iter(mParameters);
    int i=0;
    while(iter.hasNext())
    {
        iter.next();
        byteArrays << new QByteArray(iter.key().toUtf8());
        params[i++] = byteArrays.last()->data();
        QString paramValue = "\"" + iter.value() + "\"";
        byteArrays << new QByteArray(paramValue.toUtf8());
        params[i++] = byteArrays.last()->data();
    }
    params[i] = NULL;

    if( mErrorRedirect )
        freopen(mErrorFilename.toUtf8().data(),"w",stderr);

    mOutput = xsltApplyStylesheet(mStylesheet, mXml, (const char**)params);

    if( mErrorRedirect )
        fclose(stderr);

    qDeleteAll(byteArrays);

    int bytesWritten = 0;
    FILE *fid = fopen(mOutputFilename.toUtf8().data(),"w");
    if( fid == 0 )
    {
        qWarning() << "Could not open output file." << mOutputFilename;
    }
    else
    {
        bytesWritten = xsltSaveResultToFile(fid, mOutput, mStylesheet);
        fclose(fid);
    }

    if( mStylesheet != 0 )
        xsltFreeStylesheet(mStylesheet);
    if( mOutput != 0 )
        xmlFreeDoc(mOutput);
    if( mXml != 0 )
        xmlFreeDoc(mXml);

    if( bytesWritten > 0 )
        return Xsltproc::Success;
    else
        return Xsltproc::GenericFailure;
}

void Xsltproc::setErrorFilename(const QString & filename)
{
    mErrorRedirect = true;
    mErrorFilename = filename;
}
