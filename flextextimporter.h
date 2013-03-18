#ifndef FLEXTEXTIMPORTER_H
#define FLEXTEXTIMPORTER_H

#include "flextextreader.h"

class WritingSystem;

class FlexTextImporter : public FlexTextReader
{
public:
    FlexTextImporter(Text * text);

    FlexTextReader::Result readFile(const QString & filepath);
};

#endif // FLEXTEXTIMPORTER_H
