/*!
  \class FlexTextReader
  \ingroup IO
  \brief This class reads a .flextext file and puts the data into a Text object. Instantiated in Text. This class reads in .flextext files that Gloss created, as opposed to FlexTextImporter, which reads external .flextext files and adds their text elements to the database.
*/

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
