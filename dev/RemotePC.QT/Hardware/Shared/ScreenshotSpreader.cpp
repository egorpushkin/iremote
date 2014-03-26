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
#include "../../Server/Config.h"
#include "ScreenshotSpreader.h"

// Hardware subsystem.
#include "Hardware/Hardware.h"

// Interaction protocol tool.
#include "Messages/ScreenshotMessage.h"

// QT is responsible for image capturing and resizing.
#include <QtGui/QtGui>
#include <QDesktopWidget>
#include <QApplication>

namespace RemotePC
{
	void ScreenshotSpreader::Handle(const Client& client, mc::IProtocolRef protocol, IServerControlRef /* server */)
	{
        if ( !client.IsScreenCaptureEnabled() )
            return;

        // Acquire mouse cursor position.
        RemotePC::ScreenPoint position = RemotePC::HardwareProvider::Instance().GetMouseControl()->GetPosition();

        // Calculate bounding box around current mouse position.
        float blockSize = 256.0f;
        float zoomLevel = client.GetZoomLevel();
        int imageSize = (int)( blockSize * zoomLevel );
        int leftX = position.x_ - imageSize / 2;
        int topY = position.y_ - imageSize / 2;
        int rightX = leftX + imageSize;
        int bottomY = topY + imageSize;

        // Find screen to which cursor currently belongs.
        QDesktopWidget* desktop = QApplication::desktop();
        int screenNumber = desktop->screenNumber(QPoint(position.x_, position.y_));
        QWidget* screen = desktop->screen(screenNumber);
        QRect geometry = screen->geometry();

        // Cut areas beyond the surface of the screen.
        int leftdX = ( leftX < geometry.left() ) ? ( geometry.left() - leftX ) : 0;
        int topdY = ( topY < geometry.top() ) ? ( geometry.top() - topY ) : 0;
        int rightdX = ( rightX > geometry.right() ) ? ( rightX - geometry.right() ) : 0;
        int bottomdY = ( bottomY >= geometry.bottom() ) ? ( bottomY - geometry.bottom() ) : 0;
        leftX += leftdX;
        topY += topdY;
        rightX += rightdX;
        bottomY += bottomdY;
        int fragmentWidth = imageSize - leftdX - rightdX;
        int fragmentHeight = imageSize - topdY - bottomdY;
        bool isBoundary = ( leftdX > 0 ) || ( topdY > 0 ) || ( rightdX > 0 ) || ( bottomdY > 0 );

        // Grab part of the screen.
        QPixmap fragment = QApplication::primaryScreen()->grabWindow(
            QApplication::desktop()->winId(),
            leftX, topY, fragmentWidth, fragmentHeight);

        // Check to see if anything was actually grabbed.
        fragmentWidth = fragment.width();
        fragmentHeight = fragment.height();
        if ( fragmentWidth <= 0 || fragmentHeight <= 0 )
        {
            return;
        }

        if ( isBoundary )
        {
            // Image was grabbed right next to one of screen edges.
            QPixmap temp(blockSize, blockSize);
            QPainter painter(&temp);
            painter.fillRect(0, 0, blockSize, blockSize, QColor(Qt::black));
            QRect source(0, 0, fragmentWidth, fragmentHeight);
            QRect target(
                leftdX * blockSize / imageSize, topdY * blockSize / imageSize,
                fragmentWidth * blockSize / imageSize, fragmentHeight * blockSize / imageSize);
            painter.drawPixmap(target, fragment, source);
            fragment = temp;
        }
        else
        {
            if ( imageSize != (int)blockSize )
            {
                // Image was grabbed from within the screen.
                fragment = fragment.scaled(
                    QSize(blockSize, blockSize),
                    Qt::KeepAspectRatio,
                    Qt::SmoothTransformation);
            }
        }

        // Construct screenshot message from QT image.
        QByteArray imageBytes;
        QBuffer imageBuffer(&imageBytes);
        imageBuffer.open(QIODevice::WriteOnly);
        #if !defined(IREMOTE_NO_QT_PLUGINS)
            fragment.save(&imageBuffer, "JPG", Config::Instance().GetSFBCompression());
        #else
            fragment.save(&imageBuffer, "PNG", Config::Instance().GetSFBCompression());
        #endif

        mc::IMessagePtr message(
            mc::Class< RemotePC::ScreenshotMessage >::Create(
                imageBytes.size(), imageBytes.constData() ) );

        // Send image to the client.
        protocol->Send( message );
	}
}
