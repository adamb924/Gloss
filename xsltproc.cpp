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
}

Xsltproc::~Xsltproc()
{
    if( mStylesheet != 0 )
        xsltFreeStylesheet(mStylesheet);
    if( mOutput != 0 )
        xmlFreeDoc(mOutput);
    if( mXml != 0 )
        xmlFreeDoc(mXml);

    xsltCleanupGlobals();
    xmlCleanupParser();
}

bool Xsltproc::setStyleSheet(const QString & filename)
{
    mStyleSheetFilename = filename;
    mStylesheet = xsltParseStylesheetFile( (const xmlChar*)filename.toUtf8().data() );
    return mStylesheet != 0;
}

bool Xsltproc::setXmlFile(const QString & filename)
{
    mXmlFilename = filename;
    mXml = xmlParseFile( (const char*)filename.toUtf8().data() );
    return mXml != 0;
}

void Xsltproc::setOutputFile(const QString & filename)
{
    mOutputFile = filename;
}

void Xsltproc::setParameters(const QHash<QString,QString> & parameters)
{
    mParameters = parameters;
}

Xsltproc::ReturnValue Xsltproc::execute()
{
    if( mStylesheet == 0 )
        return Xsltproc::InvalidStylesheet;
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
        byteArrays << new QByteArray(iter.value().toUtf8());
        params[i++] = byteArrays.last()->data();
    }
    params[i] = NULL;

//    for(int i=0; i<mParameters.count(); i++)
//    {
//        qDebug() << QString(params[2*i]) << QString(params[2*i + 1]);
//    }

    mOutput = xsltApplyStylesheet(mStylesheet, mXml, (const char**)params);

    FILE *fid = fopen(mOutputFile.toUtf8().data(),"w");
//    fprintf(fid, "test\n");
    if( fid == 0 )
    {
        qDebug() << "Could not open output file." << mOutputFile;
    }
    else
    {
        qDebug() << xsltSaveResultToFile(fid, mOutput, mStylesheet);
        fclose(fid);
    }


    qDeleteAll(byteArrays);

    return Xsltproc::Success;
}
