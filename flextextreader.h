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

private:

    //! \brief Reads the given flextext file to set the baseline writing system for the text
    bool setBaselineWritingSystemFromFile(const QString & filePath );


protected:
    Text *mText;
    DatabaseAdapter *mDbAdapter;
};

#endif // FLEXTEXTREADER_H
