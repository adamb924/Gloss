/*!
  \class Xsltproc
  \brief An interface class for performing XSL transformations with libxslt.
*/

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

    enum ReturnValue { Success, InvalidStylesheet, InvalidXmlFile, CouldNotOpenOutput, ApplyStylesheetFailure, GenericFailure };

    void setStyleSheet(const QString & filename);
    void setXmlFilename(const QString & filename);
    void setOutputFilename(const QString & filename);
    void setParameters(const QHash<QString,QString> & parameters);
    void setErrorFilename(const QString & filename);
    Xsltproc::ReturnValue execute();

private:
    void freeResources();
    bool getErrorFileHasContents();

    QString mStyleSheetFilename;
    QString mXmlFilename;
    QString mErrorFilename;
    QString mOutputFilename;
    char **mParams;
    unsigned int mNParams;

    bool mErrorRedirect;

    xsltStylesheetPtr mStylesheet;
    xmlDocPtr mXml;
    xmlDocPtr mOutput;
};

QDebug operator<<(QDebug dbg, const Xsltproc::ReturnValue & value);

#endif // XSLTPROC_H
