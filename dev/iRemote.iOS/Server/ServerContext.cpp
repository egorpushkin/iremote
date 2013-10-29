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

#include "IServerControl.h"
#include "IUIEvents.h"
#include "RenderingQueue.h"
#include "ServerContext.h"

#include "AuthMessage.h"
#include "AuthResponse.h"
#include "AuthResponse2.h"
#include "ScreenshotMessage.h"
#include "VolumeMessage.h"
#include "PasswordRequest.h"
#include "PasswordMessage.h"
#include "Messages.h"
#include "ShellResponse.h"
#include "ZoomParamsMessage.h"

// EXPERIMENTAL: Audio messages.
// #include "IDataMessage.h"
// #include "DataMessage.h"
// #include "AudioMessages.h"
// #include "AudioStream.h"

#include "Holder.h"

// Hosst manager support
#include "Hosts/Hosts.h"

// Session support
#include "Session.h"

namespace MinCOM
{

	// {4F8320D1-752E-4525-A90D-38FB97C082A3}
	template<> const Guid TypeInfo< RemotePC::IServerControl >::iid_ = 
		{ 0x4f8320d1, 0x752e, 0x4525, 0xa9, 0xd, 0x38, 0xfb, 0x97, 0xc0, 0x82, 0xa3 };

}

namespace RemotePC
{

	ServerContext::ServerContext( IUIEventsRef events, const std::string& ip, const std::string& service, mc::IMessageRef authMessage )
		: mc::CommonImpl< mc::IRunnable >()
		, mc::CommonImpl< IServerControl >()
		, mc::CommonImpl< mc::DProtocol >()
		, service_( mc::Library::Service() )
        , ip_(ip)
        , port_(service)
        , authMessage_( authMessage )
		, connection_()
		, protocol_()
        , authResponse_()
	    , blockListGuard_( mc::Library::Mutex() )
	    , tempBlockedMessages_()
		, events_( events )
        , isUIReady_(false)
		, stopFlagGuard_( mc::Library::Mutex() )
		, stopFlag_( false )
		, renderingQueue_( mc::Class< RenderingQueue >::Create() )
	{	
		// Check whether input data is correct.
		if ( !events_ || !authMessage_ ) throw std::exception();
	}
    
    ServerContext::~ServerContext()
    {
		// EXPERIMENTAL: Audio streaming.
		// AudioStream::Instance().Shutdown();
    }

	// IRunnable section
	mc::result ServerContext::Run()
	{		
        // Establish connection.
        connection_ = mc::Library::TCPConnection( service_ );
        mc::result code = connection_->Establish( ip_, port_ );
        if ( mc::Error::IsFailed( code ) )
        {
            events_->OnError("Failed to connect to host.");
            return mc::_S_OK;
        }
        
        // Configure messaging service.
		protocol_ = mc::IProtocolPtr( mc::Library::Protocol(connection_, mc::IProtocol::ASYNC) );
		protocol_->GetMessagesMap()->Register(AuthResponse::messageId_, mc::Class< AuthResponse >::Create );
		protocol_->GetMessagesMap()->Register(AuthResponse2::messageId_, mc::Class< AuthResponse2 >::Create );
        protocol_->GetMessagesMap()->Register(ScreenshotMessage::messageId_, mc::Class< ScreenshotMessage >::Create );
        protocol_->GetMessagesMap()->Register(VolumeMessage::messageId_, mc::Class< VolumeMessage >::Create );
        protocol_->GetMessagesMap()->Register(Messages::limitReached_, mc::Class< mc::MessageImpl >::Create );
        protocol_->GetMessagesMap()->Register(PasswordRequest::messageId_, mc::Class< PasswordRequest >::Create );        
        protocol_->GetMessagesMap()->Register(Messages::wrongDeviceIp_, mc::Class< mc::MessageImpl >::Create );		
        protocol_->GetMessagesMap()->Register(Messages::wrongPassword_, mc::Class< mc::MessageImpl >::Create );		
        protocol_->GetMessagesMap()->Register(ShellResponse::messageId_, mc::Class< ShellResponse >::Create );		
        protocol_->GetMessagesMap()->Register(ZoomParamsMessage::messageId_, mc::Class< ZoomParamsMessage >::Create );				
        
		// EXPERIMENTAL: Audio streaming.
		// protocol_->GetMessagesMap()->Register(DataMessage::messageId_, mc::Class< DataMessage >::Create );		
		
        // Subscribe on all messages from host.
		mc::Events::Advise(protocol_,
			mc::CommonImpl< mc::IRunnable >::GetSelf(),
			mc::TypeInfo< mc::DProtocol >::GetGuid());

        // Perform authentication.
		protocol_->Send( authMessage_ );
        
        // Notify ui on connection.
        events_->OnConnected();		
		// Allow this thread to access GL context.
		events_->AttachGLContext();
		
		// EXPERIMENTAL: Audio streaming.
		// AudioStream::Instance().Initialize();
		
		// Start screenshots renderer.
		renderingQueue_->start(events_);		
		
		// Just another safety measure.
		try
		{
            // Start analyzing network activity.
            service_->Run();
		}
		catch ( ... )
		{
		}

		return mc::_S_OK;
	}

	// IServerControl section
    mc::result ServerContext::Send(mc::IMessageRef message)
    {
        if ( !protocol_ ) 
        {
            // TOINV: Determine when it may happen.
            events_->OnError("");
            return mc::_E_FAIL;
        }        
        return protocol_->Send(message);
    }
    
	mc::result ServerContext::Stop()
	{
		// Stop flag must be set before blocking the object.
		SetStopFlag();
		
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );
		
		// Stop rendering screenshots.
		renderingQueue_->stop();
		renderingQueue_ = NULL;		
		
        // Prevent UI from receiving notifications from servier since this moment.
        isUIReady_ = false;
        
        // Destroy resources including connection object. This leads to returning control from all
        // synchronous asio routines.
        protocol_ = NULL;
        
        // Connection should be explicitly closed because it's async handlers hold strong
        // references to it. 
        mc::IConnectionWeak connectionWeak(connection_);
        connection_ = NULL;
        mc::IConnectionPtr connection(connectionWeak);
        if ( connection )
        {
            connection->Close();
            connection = NULL;
        }
        
		// Stop service responsible for communication with host.
        // This breaks all current connections.
		service_->Stop();
        
		return mc::_S_OK;
	}
    
    void ServerContext::SetUIReady() 
    {		
		if ( TestStopFlag() ) 
			return;		
        mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );
        isUIReady_ = true;
    }
    
    mc::IMessagePtr ServerContext::GetAuthResponse() const
    {
        return authResponse_;
    }
	
	void ServerContext::BlockMessage(const mc::Guid& messageId) 
	{
		mc::MutexScope locker( blockListGuard_ );
		tempBlockedMessages_.insert( messageId );
	}
	
	void ServerContext::UnblockMessage(const mc::Guid& messageId)
	{
		mc::MutexScope locker( blockListGuard_ );
		tempBlockedMessages_.erase( messageId );
	}
	
	// DProtocol section
	mc::result ServerContext::MessageArrived(mc::IProtocolRef protocol, mc::IMessageRef message)
	{
		// Messages from server are no longer interesting, when connection is 
		// stopping.
		if ( TestStopFlag() ) 
			return mc::_S_FALSE; 		
		
		// Blocking should be performed only when stop flag is tested.
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );
        
        if ( !events_ )
            return mc::_S_FALSE;
		
		mc::Guid messageId = message->GetCode();
		
		// Check block lists first.
		if ( TestBlock(messageId) )
			return mc::_S_FALSE;

		// Find proper message handler.
		if ( ScreenshotMessage::messageId_ == messageId )
		{
            if ( !isUIReady_ )
                return mc::_S_FALSE;            
			// Schedule screenshot decoding.
			renderingQueue_->handle(message);
		} 
        else if ( Messages::wrongPassword_ == message->GetCode() )
        {
			HandleWrongPassword();
		}
        else if ( PasswordRequest::messageId_ == messageId )
        {
			HandlePasswordRequest(message);
        }       
        else if ( AuthResponse2::messageId_ == messageId )
        {
            // Remember authentication response.
            authResponse_ = message;            
            // Handle cookie value.
            HandleAuthCookie(message);         
			
            // Host record should be modified only, if authentication has not been
            // performed with the help of cookie. That is why, the check is made over
			// authMessage_ property and not over authResponse_. authMessage_ property
			// contains initial message, that was sent from client to server. Cookie
			// presence there means that this was not initial authentication, but
			// session was restored. In this case it is not necessary to save/update 
			// host information as it has not been changed.
            bool hasCookie = authMessage_->GetProperty( AuthMessage::PROP_HAS_CODE ).toBool();
            if ( !hasCookie )
            {
                // Acquire and remember host id.
                mc::Guid hostId = message->GetProperty( AuthResponse2::PROP_HOST_ID ).toGuid();
                Session::Instance().GetCurrent().SetHostId(hostId);
                // Save or update information about current host.
                HostsManager::Instance().SaveHost( Session::Instance().GetCurrent() );
            }
            
            // Authentication is proven. Notify UI on success.
            events_->OnApproved();            
        }		
        else if ( AuthResponse::messageId_ == messageId )
        {
            // Remember authentication response.
            authResponse_ = message;            
            // Handle cookie value.
            HandleAuthCookie(message);         
            
            // Authentication is proven. Notify UI on success.
            events_->OnApproved();            			
			// This is the right place for notifying user on old Remote PC Suite.
			events_->WarnOnOldRemotePC();
        }
        else if ( VolumeMessage::messageId_ == messageId )
        {
            unsigned long value = message->GetProperty( VolumeMessage::PROP_VALUE ).toULong();
            events_->UpdateVolume(value);
        }
        else if ( Messages::limitReached_ == messageId )
        {
            events_->OnError("Limit of connections is reached.");
        }
        else if ( Messages::wrongDeviceIp_ == messageId )
        {
            events_->OnError("Wrong device IP address.");
        }
        else if ( ShellResponse::messageId_ == messageId )
        {
            std::string response = message->GetProperty(ShellResponse::PROP_RESPONSE).toString();
            events_->showShellResponse(response);
        }        
		else if ( ZoomParamsMessage::messageId_ == messageId ) 
		{
			// Acquire and set zoom params.
			float minZoom = message->GetProperty(ZoomParamsMessage::PROP_MIN_ZOOM).toFloat();
			float maxZoom = message->GetProperty(ZoomParamsMessage::PROP_MAX_ZOOM).toFloat();
			float currentZoom = message->GetProperty(ZoomParamsMessage::PROP_CURRENT).toFloat();
			events_->updateZoomParams(minZoom, maxZoom, currentZoom);
		}
		// else if ( DataMessage::messageId_ == message->GetCode() )
		// {
		//  	IDataMessagePtr dataMessage(message);
		//  	// EXPERIMENTAL: Handle audio message.			
		//  	if ( AudioMessages::pcmDataMessage_ == dataMessage->GetDataId() )
		//  	{
		//  		AudioStream::Instance().Handle(message);
		//  	}
		// }
		
		return mc::_S_OK;
	}

	mc::result ServerContext::DataErrorOccured(mc::IProtocolRef protocol)
	{
		if ( TestStopFlag() ) 
			return mc::_S_FALSE; 		
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

        // Notify UI on disconnection.
        events_->OnDisconnected();

		return mc::_S_OK;
	}

	mc::result ServerContext::Disconnected(mc::IProtocolRef protocol)
	{
		if ( TestStopFlag() ) 
			return mc::_S_FALSE; 		
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

        // Notify UI on disconnection.
        events_->OnDisconnected();
        
		return mc::_S_OK;
	}
	
#pragma mark Server messages handlers
	
	void ServerContext::HandleAuthCookie(mc::IMessageRef message)
	{
		// Acquire cookie parameters.
		bool hasCode = message->GetProperty( AuthResponse::PROP_HAS_CODE ).toBool();
		mc::Guid code = message->GetProperty( AuthResponse::PROP_CODE ).toGuid();		
		// Save cookie if any.
		if ( hasCode )
			Holder::Instance().SetCookie(code);   		
	}
	
	void ServerContext::HandlePasswordRequest(mc::IMessageRef message)
	{
		bool forcedAuth = message->GetProperty( PasswordRequest::PROP_REQUEST_AUTH ).toBool();
        mc::Guid hostId = message->GetProperty( PasswordRequest::PROP_HOST_ID ).toGuid();			
		if ( forcedAuth )
		{      
            // Password may be requested here, if
            // - it was changed on the host side;
            // - it is the first authentication attempt.
            // If the host is saved in local db, password record should be cleared.
			try
			{
				// Lock hosts manager for reading.
				mc::IReadWriteLock::Reader_ locker(HostsManager::Instance().GetLock());				
				// Find host by id.
				Host& host = HostsManager::Instance().GetHost(hostId);
                
                // Reset password, if host record is found.
                host.ResetPassword();
            }
            catch ( ... )
            {
            }
            
            // Present password popup.
			events_->OnPasswordRequested(false);            
		}
		else
		{			
			try
			{
				// Lock hosts manager for reading.
				mc::IReadWriteLock::Reader_ locker(HostsManager::Instance().GetLock());				
				// Find host by id.
				Host& host = HostsManager::Instance().GetHost(hostId);
                
                // Check whether host record contains password.
                if ( !host.HasPassword() )
                    // Ask user for password for the host.
                    throw std::exception();
				
				// Construct password message with password extracted from user record.
				// Host record stores and returns password in MD5. Password message does not modify 
				// input string. Thus, it may be constructed with value extracted from host record.
				mc::IMessagePtr passwordMessage( mc::Class< PasswordMessage >::Create( host.GetPassword() ) );
				// Send password to server for verification.
				Send(passwordMessage);
                
                // Password (extracted from the db) must be saved in the session.
                // Session contains valid host object, which replaces the one,
                // which is stored in the db, upon successful authentication.
                Session::Instance().GetCurrent().SetMd5Password( host.GetPassword() );
			} 
			catch ( ... )
			{
				// Host was not found. Proceed to regular authentication (ask user for the password).
				events_->OnPasswordRequested(false);								
			}
		}		
	}
	
	/** 
	 * This method does nothing except that it presents password prompt.
	 */
	void ServerContext::HandleWrongPassword()
	{
		events_->OnPasswordRequested(true);  		
	}
	
#pragma mark Stop flag helpers
	
	void ServerContext::SetStopFlag()
	{
		mc::MutexScope locker( stopFlagGuard_ );
		stopFlag_ = true;
	}
	
	/**
	 * @return true, if stop flag is set. In most cases callers should just
	 * quit in this case. In all cases, caller must not block object by means of
	 * internal mutex, which is accessible by 
	 * mc::CommonImpl< IServerControl >::GetLock().
	 */
	bool ServerContext::TestStopFlag()
	{
		mc::MutexScope locker( stopFlagGuard_ );
		return stopFlag_;
	}
	
	/**
	 * Returns true, if message is blocked.
	 */
	bool ServerContext::TestBlock(const mc::Guid& messageId)
	{
		mc::MutexScope locker( blockListGuard_ );
		return ( tempBlockedMessages_.end() != tempBlockedMessages_.find( messageId ) );		
	}

}
