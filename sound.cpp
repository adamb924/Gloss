#include "sound.h"

#include <QtMultimedia>

Sound::Sound( const QUrl & filename )
{
    mFileURL = filename;

    // defaults:
    mSampleRate = -1;
    mBitsPerSample = -1;
    mNChannels = -1;

    mAudioOutput = nullptr;

    readHeader();
}

Sound::~Sound()
{
}

bool Sound::isInvalid() const
{
    return mSampleRate == -1 || mBitsPerSample == -1 || mNChannels == -1;
}

QString Sound::filename() const
{
    return mFileURL.toLocalFile();
}

void Sound::readHeader()
{
    QFile audio_file(mFileURL.toLocalFile());
    if(audio_file.open(QIODevice::ReadOnly))
    {
        QByteArray header = audio_file.read(44);
        if(QString(header.left(4)) != "RIFF")
        {
            audio_file.close();
            return;
        }

        QDataStream stream(&header, QIODevice::ReadOnly);
        stream.setByteOrder(QDataStream::LittleEndian);
        stream.skipRawData(22);
        stream >> mNChannels;
        stream >> mSampleRate;
        stream.skipRawData(6);
        stream >> mBitsPerSample;

        audio_file.close();
    }
    else
    {
        mSampleRate = -1;
        mBitsPerSample = -1;
        mNChannels = -1;
        qWarning() << "Sound::readHeader" << "File could not be opened" << mFileURL.toLocalFile();
    }
}

bool Sound::playSegment(qlonglong start, qlonglong end)
{
    if( isInvalid() )
    {
        qWarning() << "Invalid Sound object" << mFileURL.toLocalFile();
        return false;
    }

    start = bytePositionAtTime( static_cast<float>(start)/1000.0f );
    end = bytePositionAtTime( static_cast<float>(end)/1000.0f );

    qlonglong duration = end - start;

    if( mAudioOutput != nullptr )
        mAudioOutput->stop();

    QFile audio_file(mFileURL.toLocalFile());
    if(audio_file.open(QIODevice::ReadOnly))
    {
        audio_file.seek(44 + start ); // skip wav header and move to start
        mAudioData = audio_file.read(duration);
        audio_file.close();

        mBuffer = new QBuffer( &mAudioData );
        mBuffer->open(QIODevice::ReadOnly);

        mAudioOutput = new QAudioOutput(getAudioFormat(), this);
        mAudioOutput->start(mBuffer);
        connect( mAudioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(finishedPlaying(QAudio::State)) );

        return true;
    }
    else
    {
        qWarning() << "File could not be opened" << mFileURL.toLocalFile();
        return false;
    }
}

QAudioFormat Sound::getAudioFormat()
{
    QAudioFormat format;
    format.setSampleSize(mBitsPerSample);
    format.setSampleRate(mSampleRate);
    format.setChannelCount(mNChannels);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);
    return format;
}

void Sound::finishedPlaying(QAudio::State state)
{
    if(state == QAudio::IdleState)
    {
        mAudioOutput->stop();
        delete mAudioOutput;
        delete mBuffer;
        mAudioOutput = nullptr;
    }
}
