#ifndef SOUND_H
#define SOUND_H

#include <QtGlobal>
#include <QByteArray>
#include <QtMultimedia>

class Sound
{
public:
    enum ReadBehavior { AsNeeded, AllAtOnce, AfterFirstRequest };

    Sound( const QUrl & filename );
    ~Sound();

    bool playSegment(qlonglong start, qlonglong end);
    void play(QByteArray * audioData);

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

private:
    void readHeader();

    QUrl mFileURL;
    QByteArray mAudioData;
    qint32 mSampleRate;
    qint16 mBitsPerSample;
    qint16 mNChannels;

    QAudioOutput *mAudioOutput;
};

#endif // SOUND_H
