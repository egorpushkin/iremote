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

// Server object model.
#include "../../Server/Client.h"
#include "../../Server/IClientHandler.h"
#include "../../Server/Services/Services.h"
#include "../../Server/IServerControl.h"
#include "ScreenshotSpreader.h"

// Hardware subsystem.
#include "Hardware/Hardware.h"

// Interaction protocol tool.
#include "Messages/ScreenshotMessage.h"

// QT is responsible for image capturing and resizing.
#include <QtGui/QtGui>

namespace RemotePC
{
	void ScreenshotSpreader::Handle(const Client& client, mc::IProtocolRef protocol, IServerControlRef /* server */)
	{
        if ( !client.IsScreenCaptureEnabled() )
            return;
        if ( !client.IsFrameBuffer() )
        {
            // Acquire mouse cursor position.
            RemotePC::ScreenPoint position = RemotePC::HardwareProvider::Instance().GetMouseControl()->GetPosition();

            float zoomLevel = client.GetZoomLevel();
            float imageDimension = 256.0f * zoomLevel;

            // Grab part of desktop.
            QPixmap screenImage = QPixmap::grabWindow(
                QApplication::desktop()->winId(),
                position.x_ - imageDimension / 2, position.y_ - imageDimension / 2,
                imageDimension, imageDimension);
            // Test whether image is correct.
            if ( screenImage.width() <= 0 || screenImage.height() <= 0  )
                return;

            screenImage = screenImage.scaled(
                QSize(256, 256),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);

            // Construct screenshot message from QT image.
            QByteArray imageBytes;
            QBuffer imageBuffer(&imageBytes);
            imageBuffer.open(QIODevice::WriteOnly);
            #if !defined(IREMOTE_NO_QT_PLUGINS)
                screenImage.save(&imageBuffer, "JPG");
            #else
                screenImage.save(&imageBuffer, "PNG");
            #endif

            mc::IMessagePtr message(
            mc::Class< RemotePC::ScreenshotMessage >::Create(
                imageBytes.size(), imageBytes.constData() ) );

            // Send image to the client.
            protocol->Send( message );
        }
	}
}
