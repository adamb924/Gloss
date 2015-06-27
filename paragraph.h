#ifndef PARAGRAPH_H
#define PARAGRAPH_H

#include <QObject>
#include <QList>

class Phrase;
class GlossItem;

class Paragraph : public QObject
{
    Q_OBJECT

    friend class FlexTextReader;
    friend class FlexTextWriter;
    friend class FlexTextImporter;

public:
    explicit Paragraph(QObject *parent = 0);

    //! \brief Returns a pointer to the list of Phrases in the paragraph
    QList<Phrase*>* phrases();

    //! \brief Returns a const pointer to the list of Phrases in the paragraph
    const QList<Phrase *> *phrases() const;

    //! \brief Removes \a phrase from the paragraph
    bool removePhrase( Phrase * phrase );

    //! \brief Returns the index of \a phrase from the paragraph, or -1 if it does not exist there
    int indexOf(const Phrase *phrase ) const;

    //! \brief Returns the index of \a phrase from the paragraph, or -1 if it does not exist there
    int indexOfGlossItem(const GlossItem *item) const;

    //! \brief Returns the number of phrases in the paragraph
    int phraseCount() const;

    //! \brief Inserts \a phrase at position \a before
    void insertPhrase(int i, Phrase * phrase);

signals:

public slots:

private:
    QList<Phrase*> mPhrases;
};

#endif // PARAGRAPH_H
