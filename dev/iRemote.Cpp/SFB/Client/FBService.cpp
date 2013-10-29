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

#include "Connector/Services/Services.h"
#include "Matrix.h"
#include "Texture.h"
#include "Cursor.h"
#include "Surface.h"
#include "IFBEvents.h"
#include "RenderingQueue.h"
#include "IFBService.h"
#include "FBService.h"

#include "../Messages/ControlMessage.h"
#include "../Messages/RequestDevice.h"
#include "../Messages/DeviceInfo.h"
#include "../Messages/DeviceDisconnected.h"
#include "../Messages/DeviceNotFound.h"
#include "../Messages/InitMessage.h"
#include "../Messages/BlockMessage.h"
#include "../Messages/BatchHeader.h"
#include "Messages/AuthResponse.h"
#include "Messages/AuthResponse2.h"
#include "Messages/ServiceRequest.h"
#include "Messages/StreamRequest.h"
#include "Messages/MoveMessage.h"
#include "Messages/MoveToMessage.h"
#include "Messages/Messages.h"
#include "Versions/Versions.h"

namespace MinCOM
{
	// {93F8AA5F-B16F-429B-BC19-16872BE1F937}
	template<> const Guid TypeInfo< RemotePC::SFB::IFBService >::iid_ =
		{ 0x93F8AA5F, 0xB16F, 0x429B, { 0xBC, 0x19, 0x16, 0x87, 0x2B, 0xE1, 0xF9, 0x37 } };	
}

namespace RemotePC
{
namespace SFB
{
	// {3E2E7364-7418-445C-8C1D-BB2CD1B02257}
	const mc::Guid FBService::serviceId_ =
		{ 0x3E2E7364, 0x7418, 0x445C, { 0x8C, 0x1D, 0xBB, 0x2C, 0xD1, 0xB0, 0x22, 0x57 } };
	
	FBService::FBService()
		: mc::CommonImpl< IServiceHandler >()
		, mc::CommonImpl< IFBService >()
		, servicesManager_()
		, surface_()
		, renderingQueue_()
	    , events_()
		, cellSize_()
	{
	}
	
	/**
	 * IFBService section
	 */		
	
	void FBService::setEvents(const IFBEventsPtr& events)
	{
		events_ = events;
	}
	
	SurfacePtr FBService::surface()
	{
		return surface_;
	}
	
	/**
	 * IServiceHandler section
	 */
	
	mc::Guid FBService::ServiceId()
	{
		return serviceId_;
	}
	
	void FBService::SetManager(IServicesManagerRef manager)
	{
		servicesManager_ = manager;
	}
	
	void FBService::DeviceConnected()
	{
		// Not implemented by design.
	}
	
	void FBService::DeviceDisconnected()
	{
		// Clear old surface. 
		events_->init(SurfacePtr());
		
		// Local rendering engine is not required untill new connection is tuned.
		if ( renderingQueue_ )
		{
			renderingQueue_->stop();
			renderingQueue_ = NULL;
		}		
		
		// All service streams are disconnected by ServicesManager. 
		// No need to do it here.		
	}
	
	void FBService::StreamConnected(mc::IProtocolRef stream)
	{
		// Register SFB messages.
		stream->GetMessagesMap()->Register(InitMessage::messageId_, mc::Class< InitMessage >::Create );
		stream->GetMessagesMap()->Register(ControlMessage::messageId_, mc::Class< SFB::ControlMessage >::Create );
		stream->GetMessagesMap()->Register(DeviceInfo::messageId_, mc::Class< DeviceInfo >::Create );		
		stream->GetMessagesMap()->Register(DeviceDisconnected::messageId_, mc::Class< SFB::DeviceDisconnected >::Create );
		stream->GetMessagesMap()->Register(DeviceNotFound::messageId_, mc::Class< DeviceNotFound >::Create );
		stream->GetMessagesMap()->Register(BlockMessage::messageId_, mc::Class< BlockMessage >::Create );
		stream->GetMessagesMap()->Register(BatchHeader::messageId_, mc::Class< BatchHeader >::Create );
		stream->GetMessagesMap()->Register(MoveToMessage::messageId_, mc::Class< MoveToMessage >::Create );		
		
		// Authentication message (StreamRequest) is sent automatically by ServicesManager (StreamContext).
		// Server initiates dialog, so nothing to be done here.
	}
	
	void FBService::StreamDisconnected(mc::IProtocolRef /* stream */)
	{
		// Local rendering engine is not required untill new connection is tuned.
		if ( renderingQueue_ )
		{
			renderingQueue_->stop();
			renderingQueue_ = NULL;
		}
		
		// Request new stream immediately after disconnection.
		IServicesManagerPtr servicesManager(servicesManager_);
		if ( !servicesManager )
			return;
		servicesManager->RequestStream( ClassRoot_::GetSelf() );		
	}
	
	void FBService::ControlMessage(mc::IMessageRef message)
	{
		mc::Guid messageId = message->GetCode();
		if ( AuthResponse2::messageId_ == messageId )
		{		
			// Check whether service handler is properly configured
			// (an error should not occur during normal flow here).
			IServicesManagerPtr servicesManager(servicesManager_);
			if ( !servicesManager )
				return;
			mc::IProtocolPtr device = servicesManager->GetDevice();
			if ( !device )
				return;		
			
			// Client side service handler forcibly requests service from server.
			mc::IMessagePtr serviceRequest( mc::Class< ServiceRequest >::Create(
				serviceId_, iREMOTE_SFB_VER_MAJOR, iREMOTE_SFB_VER_MINOR, std::string()) );
			device->Send(serviceRequest);
		} 
		else if ( StreamRequest::messageId_ == messageId )
		{
			// Initial stream is requested here. All subsequent requests are sent from
			// local StreamDisconnected handler.
			IServicesManagerPtr servicesManager(servicesManager_);
			if ( !servicesManager )
				return;
			servicesManager->RequestStream( ClassRoot_::GetSelf() );
		}
		else if ( MoveToMessage::messageId_ == messageId ) 
		{
			if ( surface_ )
			{
				float x = message->GetProperty(MoveMessage::PROP_DX).toFloat();
				float y = message->GetProperty(MoveMessage::PROP_DY).toFloat();				
				surface_->internalCursor()->set(x, y);
				events_->cursor();
			}
		}		
        else if ( Messages::serviceFailure_ == messageId )
		{
			// Not implemented in this version.			
		}
        else if ( Messages::streamSuccess_ == messageId )
		{
			// Not implemented in this version.			
		}
        else if ( Messages::streamFailure_ == messageId )
		{
			// Not implemented in this version.			
		}
	}
	
	void FBService::StreamMessage(mc::IProtocolRef stream, mc::IMessageRef message)
	{
		mc::Guid messageId = message->GetCode();
		
        if ( InitMessage::messageId_ == messageId ) 
        {
			// Kill current rendering engine, but not the surface. 
			if ( renderingQueue_ )
			{
				renderingQueue_->stop();
				renderingQueue_ = NULL;
			}
			
            // Handle cell size.
			cellSize_ = message->GetProperty(InitMessage::PROP_CELLSIZE).toInt();
            
			// Request specific device from server.
            mc::IMessagePtr requestDevice(mc::Class< RequestDevice >::Create(
				RequestDevice::primaryDisplayId_));
            stream->Send(requestDevice);            
        }
        else if ( DeviceInfo::messageId_ == messageId )         
        {
            // Handle screen size.
			size_t screenWidth = message->GetProperty(DeviceInfo::PROP_WIDTH).toInt();
			size_t screenHeight = message->GetProperty(DeviceInfo::PROP_HEIGHT).toInt();			
			
			// Create new surface.
			SurfacePtr surface = SurfacePtr( new Surface(screenWidth, screenHeight, cellSize_) );
			// Collect old cursor properties. 
			if ( surface_ )
				surface->cursor()->set( surface_->cursor() );
			// Remember new surface.
			surface_ = surface;
			
			// Initialize local rendering engine.
			renderingQueue_ = mc::Class< RenderingQueue >::Create();
			renderingQueue_->start(events_, surface_);
			
			// Notify local controls on new surface.
			events_->init(surface_);
        }        	
		else if ( MoveToMessage::messageId_ == messageId ) 
		{
			if ( surface_ )
			{
				float x = message->GetProperty(MoveMessage::PROP_DX).toFloat();
				float y = message->GetProperty(MoveMessage::PROP_DY).toFloat();				
				surface_->internalCursor()->set(x, y);
				events_->cursor();
			}
		}
		else if ( ( BatchHeader::messageId_ == messageId ) 
			|| ( BlockMessage::messageId_ == messageId ) ) 
		{
			if ( renderingQueue_ )
			{
				renderingQueue_->handle(message);
			}
		}		
        else if ( DeviceDisconnected::messageId_ == messageId )         
		{
			// Not implemented in this version.
		}
        else if ( DeviceNotFound::messageId_ == messageId )         
		{
			// Not implemented in this version.			
		}		
        else if ( DeviceNotFound::messageId_ == messageId )         
		{
			// Not implemented in this version.			
		}		
	}
}
}
