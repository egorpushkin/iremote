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

#ifndef AUDIOINPUT_H__REMOTEPC__INCLUDED_
#define AUDIOINPUT_H__REMOTEPC__INCLUDED_

#include <QtMultimedia/qaudioinput.h>
#include <QtMultimedia/qaudiooutput.h>
#include <QFile>
#include <QApplication>
#include <QTimer>

#include <MinCOM/MinCOM.h>

// Audio subsystem.
#include "../Provider/IAudioInput.h"

// Server object model.
#include "../../Server/Client.h"
#include "../../Server/IClientHandler.h"
#include "../../Server/IServerControl.h"

class AudioInput
    : public QIODevice
    , public mc::CommonImpl< RemotePC::IAudioInput >
{
    Q_OBJECT
public:

    AudioInput(QObject* parent);
    ~AudioInput();

    // IAudioInput section
    virtual void Start(RemotePC::IServerControlRef server);

    virtual void Stop();

    // QIODevice section
    int LinearMax();

    qint64 readData(char *data, qint64 maxlen);

    qint64 writeData(const char *data, qint64 len);

    // ICommon section
    virtual long DecrementReference();

private slots:

    void sendData();

private:

    RemotePC::IServerControlPtr server_;

    // QAudioInput *input_;
    QAudioOutput *input_;

    int maxValue_;

    QFile *file_;
    QTimer *timer_;

    // QFile *outputFile;

signals:

    void update();

};

#endif // !AUDIOINPUT_H__REMOTEPC__INCLUDED_

