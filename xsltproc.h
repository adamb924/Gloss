#ifndef XSLTPROC_H
#define XSLTPROC_H

#include <QHash>
#include <QString>

#include <libxslt/xslt.h>
#include <libxslt/transform.h>

class Xsltproc
{
public:
    Xsltproc();
    ~Xsltproc();

    enum ReturnValue { Success, InvalidStylesheet, InvalidXmlFile, GenericFailure };

    bool setStyleSheet(const QString & filename);
    bool setXmlFile(const QString & filename);
    void setOutputFile(const QString & filename);
    void setParameters(const QHash<QString,QString> & parameters);
    Xsltproc::ReturnValue execute();

private:
    QString mStyleSheetFilename;
    QString mXmlFilename;
    QString mOutputFile;
    QHash<QString,QString> mParameters;

    xsltStylesheetPtr mStylesheet;
    xmlDocPtr mXml;
    xmlDocPtr mOutput;
};

#endif // XSLTPROC_H
