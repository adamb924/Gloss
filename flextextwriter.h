#ifndef FLEXTEXTWRITER_H
#define FLEXTEXTWRITER_H

class Text;
class QString;

class FlexTextWriter
{
public:
    FlexTextWriter(Text *text);

    bool writeFile( const QString & filepath );

private:
    Text *mText;
};

#endif // FLEXTEXTWRITER_H
