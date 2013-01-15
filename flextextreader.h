#ifndef FLEXTEXTREADER_H
#define FLEXTEXTREADER_H

class Text;
class QString;

class FlexTextReader
{
public:
    explicit FlexTextReader(Text *text);

    bool readFile( const QString & filepath );

private:
    Text *mText;
};

#endif // FLEXTEXTREADER_H
