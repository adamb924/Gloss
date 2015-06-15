/*!
  \class Sound
  \ingroup Data
  \brief This is a class for handling audio files. After constructing a Sound object with a QUrl to the file, users can call playSegment() to play a portion of the file. Every Text element has a (possibly null) Sound member.
*/

#ifndef SOUND_H
#define SOUND_H

#include <QtGlobal>
#include <QByteArray>
#include <QtMultimedia>

class Sound : public QObject
{
    Q_OBJECT
public:
    enum ReadBehavior { AsNeeded, AllAtOnce, AfterFirstRequest };

    explicit Sound( const QUrl & filename );
    ~Sound();

    bool playSegment(qlonglong start, qlonglong end);

    QAudioFormat getAudioFormat();

    bool isInvalid() const;

    QString filename() const;

    inline qint64 sampleAtTime( float time )
    {
        return (qint64)(time*mSampleRate);
    }

    inline qint64 bytePositionAtTime( float time )
    {
        return (mBitsPerSample/8) * sampleAtTime( time );
    }

private slots:
    void finishedPlaying(QAudio::State state);

private:
    void readHeader();

    QUrl mFileURL;
    QByteArray mAudioData;
    QBuffer *mBuffer;
    qint32 mSampleRate;
    qint16 mBitsPerSample;
    qint16 mNChannels;

    QAudioOutput *mAudioOutput;
};

#endif // SOUND_H
