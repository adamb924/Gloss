#include "sound.h"

#include <QtMultimedia>

Sound::Sound( const QUrl & filename )
{
    mFileURL = filename;

    // defaults:
    mSampleRate = -1;
    mBitsPerSample = -1;
    mNChannels = -1;

    readHeader();
    mAudioOutput = new QAudioOutput(getAudioFormat());
}

Sound::~Sound()
{
    mAudioOutput->deleteLater();
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

        char *data = header.data();
        mSampleRate = *((long*)data + 6);
        mBitsPerSample = *(data+34);
        mNChannels = *(data+22);

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

    if( mAudioOutput != 0 )
        mAudioOutput->stop();

    start = bytePositionAtTime( (float)start/1000.0f );
    end = bytePositionAtTime( (float)end/1000.0f );

    qlonglong duration = end - start;

    QFile audio_file(mFileURL.toLocalFile());
    if(audio_file.open(QIODevice::ReadOnly))
    {
        audio_file.seek(44 + start ); // skip wav header and move to start
        QByteArray audioData = audio_file.read(duration);
        audio_file.close();

        play( &audioData );

        return true;
    }
    else
    {
        qWarning() << "File could not be opened" << mFileURL.toLocalFile();
        return false;
    }
}

void Sound::play(QByteArray * audioData)
{
    QBuffer audio_buffer( audioData );
    audio_buffer.open(QIODevice::ReadOnly);

    mAudioOutput->start(&audio_buffer);
    QEventLoop loop;
    QObject::connect(mAudioOutput, SIGNAL(stateChanged(QAudio::State)), &loop, SLOT(quit()));
    do {
        loop.exec();
    } while(mAudioOutput->state() == QAudio::ActiveState);
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
