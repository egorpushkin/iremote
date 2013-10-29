/**
 * Copyright (c) 2013 Egor Pushkin. All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Common/Common.h"

#include <stdlib.h>
#include <math.h>

#include <QtMultimedia/QAudioDeviceInfo>
#include <QtMultimedia/QAudioInput>
#include <QtMultimedia/QAudio>
#include "audioinput.h"

#include "../../Common/Messages/DataMessage.h"
#include "../../Common/Messages/AudioMessages.h"

#define BUFFER_SIZE 4096

/**
 * TODO: Ensure that this object is being destroed and cleans up all resources!
 */

AudioInput::AudioInput(QObject* parent)
    : QIODevice( parent )
    , server_()
    , input_(NULL)
    , maxValue_(0)
{
}

AudioInput::~AudioInput()
{
    std::cout << "AudioInput::~AudioInput" << std::endl;
}

void AudioInput::Start(RemotePC::IServerControlRef server)
{
    server_ = server;

    /*
    QString output
        = QApplication::applicationDirPath()
        + QString("/test.raw");
    outputFile = new QFile();
    outputFile->setFileName(output);
    outputFile->open( QIODevice::WriteOnly | QIODevice::Truncate );
*/

    QAudioFormat format;
    format.setFrequency(4000);
    format.setChannels(1);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setCodec("audio/pcm");

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if ( !info.isFormatSupported(format) )
    {
        // qWarning() << "Default format not supported - trying to use nearest";
        format = info.nearestFormat(format);
    }

    // input_ = new QAudioInput(QAudioDeviceInfo::defaultOutputDevice(), format, NULL);
    input_ = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(), format, NULL);
    // open(QIODevice::WriteOnly);
    open(QIODevice::ReadWrite);
    input_->start(this);

/*
    QAudio::Error error = input_->error();
    if ( QAudio::NoError != error )
    {
        std::cout << "AudioInput::Start" << error << std::endl;
    }
*/

/*
    QString dataFile
        = QApplication::applicationDirPath()
        + QString("/test.raw");
    file_ = new QFile(dataFile);
    file_->open(QIODevice::ReadOnly);

    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(sendData()));
    timer_->start( 100 );
*/
}

void AudioInput::sendData()
{
    char buffer[1024 * 30];
    qint64 bytesRead = file_->read(buffer, 1024 * 30);
    if ( -1 == bytesRead || 0 == bytesRead )
    {
        QString dataFile
            = QApplication::applicationDirPath()
            + QString("/freelove.wav");
        file_ = new QFile(dataFile);
        file_->open(QIODevice::ReadOnly);
        return;
    }

    std::cout << "AudioInput::~sendData " << bytesRead << std::endl;

    // Construct message from data.
    mc::IMessagePtr message( mc::Class< RemotePC::DataMessage >::Create(
        RemotePC::AudioMessages::pcmDataMessage_, 1024 * 30, buffer) );
    // Send data message to all authenticated clients.
    server_->SpreadMessage(message);
}

void AudioInput::Stop()
{
    server_ = NULL;
    close();
    input_->stop();
    input_->disconnect(this);

    // outputFile->close();

    delete input_;
    input_ = NULL;
}

qint64 AudioInput::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)
    return 0;
}

qint64 AudioInput::writeData(const char *data, qint64 len)
{
    if ( NULL == server_ )
        return len;

    std::cout << "AudioInput::~writeData " << len << std::endl;

	// Construct message from data.
    mc::IMessagePtr message( mc::Class< RemotePC::DataMessage >::Create(
		RemotePC::AudioMessages::pcmDataMessage_, (size_t)len, data) );
    // Send data message to all authenticated clients.
    server_->SpreadMessage(message);

    return len;
}

int AudioInput::LinearMax()
{
    return maxValue_;
}

/**
 * QT manages lifetime of this object.
 * Thus it should not be deleted, when there are no more references to it.
 */
long AudioInput::DecrementReference()
{
    return 1;
}
