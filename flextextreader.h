#ifndef FLEXTEXTREADER_H
#define FLEXTEXTREADER_H

class QString;
class DatabaseAdapter;
class Text;

class FlexTextReader
{
public:
    explicit FlexTextReader(Text *text);

    enum Result { FlexTextReadSuccess, FlexTextReadBaselineNotFound, FlexTextReadXmlReadError, FlexTextReadNoAttempt };

    FlexTextReader::Result readFile(const QString & filepath , bool baselineInfoFromFile);

protected:
    Text *mText;
    DatabaseAdapter *mDbAdapter;
};

#endif // FLEXTEXTREADER_H
