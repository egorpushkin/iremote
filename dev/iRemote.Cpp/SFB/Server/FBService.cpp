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

#include "../Server/Services/Services.h"
#include "FBService.h"
#include "SFB.h"
#include "InitHandler.h"

// Server config
#include "../Server/Config.h"

// Handware provider
#include "../Hardware/Hardware.h"

// General messaging
#include "Messages/AuthMessage.h"
// SFB messaging
#include "../Messages/InitMessage.h"
#include "../Messages/RequestDevice.h"
#include "../Messages/DeviceInfo.h"
#include "../Messages/ControlMessage.h"

namespace RemotePC
{
namespace SFB
{
    // {3E2E7364-7418-445C-8C1D-BB2CD1B02257}
    const mc::Guid FBService::serviceId_ =
        { 0x3E2E7364, 0x7418, 0x445C, { 0x8C, 0x1D, 0xBB, 0x2C, 0xD1, 0xB0, 0x22, 0x57 } };

    FBService::FBService(IServicesManagerRef servicesManager)
        : mc::CommonImpl< IServiceHandler >()
        , servicesManager_( servicesManager )
        , fbManager_()
        , enabled_( false )
    {
    }

    /**
     * ICommon section
     */

    mc::result FBService::PostInit()
    {
        fbManager_ = mc::Class< FBManager >::Create( servicesManager_, ClassRoot_::GetSelf() );
        return mc::_S_OK;
    }

    /**
     * IServiceHandler section
     */

    mc::Guid FBService::ServiceId()
    {
        return serviceId_;
    }

    bool FBService::IsVersionSupported(int /* major */, int /* minor */)
    {
        return true;
    }

    void FBService::ReloadSettings()
    {
        ConfigureManager();
    }

    void FBService::DeviceConnected(const IDevicePtr& device, mc::IMessageRef /* authMessage */)
    {
        // Set SFB flags.
        ServiceDescriptorPtr descriptor = device->FindDescriptor(ClassRoot_::GetSelf());
        descriptor->Properties()->PutProperty(PROP_SFB_SUPPORT, false);
        descriptor->Properties()->PutProperty(PROP_SFB_REQUIRED, false);
        descriptor->Properties()->PutProperty(PROP_SFB_FULLSCREEN, false);
    }

    void FBService::DeviceDisconnected(const IDevicePtr& /* device */)
    {
        // Need to stop manager, if it was the only connected device.
        TriggerManagerStatus();
    }

    void FBService::ServiceRequested(const IDevicePtr& device)
    {
        // Update SFB flags.
        ServiceDescriptorPtr descriptor = device->FindDescriptor(ClassRoot_::GetSelf());
        descriptor->Properties()->PutProperty(PROP_SFB_SUPPORT, true);
        descriptor->Properties()->PutProperty(PROP_SFB_REQUIRED, true);
        descriptor->Properties()->PutProperty(PROP_SFB_FULLSCREEN, true);
    }

    void FBService::StreamConnected(const IDevicePtr& /* device */, mc::IProtocolRef stream)
    {
        // Register SFB messages.
        stream->GetMessagesMap()->Register(SFB::RequestDevice::messageId_, mc::Class< SFB::RequestDevice >::Create );
        stream->GetMessagesMap()->Register(SFB::ControlMessage::messageId_, mc::Class< SFB::ControlMessage >::Create );

        // Send initialization message.
        stream->Send(InitHandler::InitMessage());

        // Update manager status.
        TriggerManagerStatus();
    }

    void FBService::StreamDisconnected(const IDevicePtr& /* device */, mc::IProtocolRef /* stream */)
    {
        // Nothing to do here. According to current design, client
        // should initiate connection, if one of streams is broken.
        // Only client knows, whether it is still required or not.
        // Server initiates session, only when new device is connected.
    }

    void FBService::ControlMessage(const IDevicePtr& /* device */, mc::IMessageRef /* message */)
    {
        // Not implemented by design.
    }

    void FBService::StreamMessage(const IDevicePtr& device, mc::IProtocolRef stream, mc::IMessageRef message)
    {
        if ( SFB::RequestDevice::messageId_ == message->GetCode() )
        {
            HandleDeviceRequest(device, stream, message);
        }
        else if ( SFB::ControlMessage::messageId_ == message->GetCode() )
        {
            HandleControlMessage(device, stream, message);
        }
    }

    /**
     * Manager control
     */

    void FBService::ConfigureManager()
    {
        // Read config properties.
        float fps = Config::Instance().GetSFBFPS();
        size_t cellSize = Config::Instance().GetSFBCellSize();
        int compression = Config::Instance().GetSFBCompression();
        Config::SFBFormat_ pixelFormat = Config::Instance().GetSFBPixelFormat();

        // Construct format object.
        SFB::RasterFormatPtr format( new SFB::RasterFormat() );
#ifdef WIN32
        if ( Config::SFB_GRAYSCALE == pixelFormat )
        {
            // Win. Grayscale.
            format->format_ = SFB::RasterFormat::GRAYSCALE;
            format->bytesPerPixel_ = 1;
            format->cellSize_ = cellSize;
            format->orientation_ = SFB::RasterFormat::NORMAL;
            format->method_ = SFB::RasterFormat::JPEG;
            format->quality_ = compression;
        }
        else
        {
            // Win. RGB.
            format->format_ = SFB::RasterFormat::BGR;
            format->bytesPerPixel_ = 3;
            format->cellSize_ = cellSize;
            format->orientation_ = SFB::RasterFormat::NORMAL;
            format->method_ = SFB::RasterFormat::JPEG;
            format->quality_ = compression;
        }
#else
        if ( Config::SFB_GRAYSCALE == pixelFormat )
        {
            // Mac. Grayscale.
            format->format_ = SFB::RasterFormat::GRAYSCALE;
            format->bytesPerPixel_ = 1;
            format->cellSize_ = cellSize;
            format->orientation_ = SFB::RasterFormat::UPSIDEDOWN;
            format->method_ = SFB::RasterFormat::JPEG;
            format->quality_ = compression;
        }
        else
        {
            // Mac. RGB.
            format->format_ = SFB::RasterFormat::RGB;
            format->bytesPerPixel_ = 3;
            format->cellSize_ = cellSize;
            format->orientation_ = SFB::RasterFormat::UPSIDEDOWN;
            format->method_ = SFB::RasterFormat::JPEG;
            format->quality_ = compression;
        }
#endif

        // Configure manager.
        fbManager_->configure(fps, format);
    }

    void FBService::TriggerManagerStatus()
    {
        IServicesManagerPtr servicesManager(servicesManager_);
        if ( !servicesManager )
            return;

        bool shouldStart = false;
        // Walk through the list of connected devices and check whether
        // there are any in active state with SFB mode required.
        {
            mc::MutexScope locker(servicesManager->DevicesLock());
            size_t devicesCount = servicesManager->DevicesCount();
            for ( size_t i = 0 ; i < devicesCount ; ++i )
            {
                IDevicePtr device( servicesManager->DeviceAtIndex(i) );
                ServiceDescriptorPtr descriptor( device->FindDescriptor( ClassRoot_::GetSelf() ) );
                shouldStart |= descriptor->Properties()->GetProperty(PROP_SFB_REQUIRED).toBool();
            }
        }

        // Start/stop the manager.
        ConfigureManager();
        ( shouldStart ) ? ( fbManager_->start() ) : ( fbManager_->stop() );
    }

    /**
     * Messages section
     */

    mc::result FBService::HandleDeviceRequest(const IDevicePtr& device, mc::IProtocolRef stream, mc::IMessageRef /* message */)
    {
        // Send requested device info.
        stream->Send( mc::Class< SFB::DeviceInfo >::Create(
            RequestDevice::primaryDisplayId_,
            HardwareProvider::Instance().GetDisplayControl()->Width(),
            HardwareProvider::Instance().GetDisplayControl()->Height(),
            std::string("Primary Display")) );

        // Request full screen on the next update.
        ServiceDescriptorPtr descriptor = device->FindDescriptor(ClassRoot_::GetSelf());
        descriptor->Properties()->PutProperty(PROP_SFB_FULLSCREEN, true);

        return mc::_S_OK;
    }

    mc::result FBService::HandleControlMessage(const IDevicePtr& device, mc::IProtocolRef stream, mc::IMessageRef message)
    {
        // Check whether SFB is still required by the device.
        const int command = message->GetProperty(SFB::ControlMessage::PROP_COMMAND).toInt();
        bool sfbRequired = false;
        if ( SFB::ControlMessage::COMMAND_START == command )
        {
            sfbRequired = true;
        }
        else if ( SFB::ControlMessage::COMMAND_STOP == command )
        {
            sfbRequired = false;
        }

        // Update SFB flags.
        ServiceDescriptorPtr descriptor = device->FindDescriptor(ClassRoot_::GetSelf());
        descriptor->Properties()->PutProperty(PROP_SFB_REQUIRED, sfbRequired);

        // Perform initialization, when SFB is turned on again.
        if ( sfbRequired )
        {
            // Send initialization message.
            stream->Send(InitHandler::InitMessage());
        }

        // Update service status.
        TriggerManagerStatus();
        return mc::_S_OK;
    }
}
}
