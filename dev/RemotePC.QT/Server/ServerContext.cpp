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

#include "ServerContext.h"

#include "Config.h"

// Core messaging
#include "Messages/MoveMessage.h"
#include "Messages/MoveToMessage.h"
#include "Messages/MouseClickMessage.h"
#include "Messages/ScrollMessage.h"
#include "Messages/ScrollMessage2.h"
#include "Messages/KeyboardMessage.h"
#include "Messages/AuthMessage.h"
#include "Messages/AuthResponse.h"
#include "Messages/AuthResponse2.h"
#include "Messages/Messages.h"
#include "Messages/VolumeMessage.h"
#include "Messages/CookieMessage.h"
#include "Messages/PasswordRequest.h"
#include "Messages/PasswordMessage.h"
#include "Messages/ZoomLevelMessage.h"
#include "Messages/ScreenshotMessage.h"
#include "Messages/ServiceRequest.h"
#include "Messages/StreamRequest.h"
#include "Versions/Features.h"

// Hardware subsystem
#include "Hardware/Hardware.h"

// Users manager support
#include "Server/UsersManager/User.h"
#include "Server/UsersManager/DUsersManager.h"
#include "Server/UsersManager/UsersManager.h"

// Services manager support
#include "Server/Services/ServiceDescriptor.h"
#include "Server/Services/IDevice.h"
#include "Server/Services/ServicesManager.h"

// Shell support
#include "Shell/ShellSupport.h"

// Modal notifications support.
#include "AuthContext.h"
#include "OldiRemoteContext.h"

namespace MinCOM
{
	// {4F8320D1-752E-4525-A90D-38FB97C082A3}
	template<> const Guid TypeInfo< RemotePC::IServerControl >::iid_ = 
        { 0x4f8320d1, 0x752e, 0x4525, { 0xa9, 0xd, 0x38, 0xfb, 0x97, 0xc0, 0x82, 0xa3 } };

	// {D6DBC5E8-B8C7-4e07-BD46-485E704EC78F}
	template<> const Guid TypeInfo< RemotePC::IClientHandler >::iid_ = 
        { 0xd6dbc5e8, 0xb8c7, 0x4e07, { 0xbd, 0x46, 0x48, 0x5e, 0x70, 0x4e, 0xc7, 0x8f } };
}

namespace RemotePC
{
    ServerContext::ServerContext( IUIEventsRef events )
		: mc::CommonImpl< mc::IRunnable >()
		, mc::CommonImpl< IServerControl >()
		, mc::CommonImpl< mc::DServer >()
		, mc::CommonImpl< mc::DProtocol >()
		, service_( mc::Library::Service() )
		, clients_()
		, authJobs_( mc::Library::JobsQueue() )
		, listOfCookies_()
		, events_( events )
        , servicesManager_( mc::Class< RemotePC::ServicesManager >::Create() )
	{	
		MC_LOG_ROUTINE;
		// Check whether input data is correct.
		if ( !events_ ) throw std::exception();
		// Run authentication jobs queue if everything is fine.
		authJobs_->Run();
	}

	ServerContext::~ServerContext()
	{
		// Detach itself from shell service.
		IShellControlPtr shellControl = ShellHost::Instance().GetShellControl();
		if ( shellControl )
			shellControl->AttachServer( NULL );
	}

    // ICommon section
    mc::result ServerContext::PostInit()
    {
        return mc::_S_OK;
    }

	// IRunnable section
	mc::result ServerContext::Run()
	{		
		MC_LOG_ROUTINE;

        // Initialize services manager.
        std::string service = Config::Instance().GetService();
        servicesManager_->SetPort(service);

        // Start server.
		mc::IServerPtr server = mc::Library::TCPServer( service_ );
        if ( mc::Error::IsError( server->AsyncAccept(service) ) )
		{
			events_->FailedToStartService();
			return mc::_S_FALSE;
		}

		// Attach itself to shell service.
		IShellControlPtr shellControl = ShellHost::Instance().GetShellControl();
		if ( shellControl )
			shellControl->AttachServer( ClassRoot_::GetSelf() );

		// Subscribe on network server events to handle inbound connections.
		mc::Events::Advise(
			server, 
			mc::CommonImpl< IRunnable >::GetSelf(), 
			mc::TypeInfo< DServer >::GetGuid());	

		// Notify UI on successful service startup.
		events_->ServiceStarted();

		// Just another safety measure.
		try
		{
			service_->Run();
		}
		catch ( ... )
		{
		}

		return mc::_S_OK;
	}

	// IServerControl section
	mc::result ServerContext::Stop()
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

		// Stop service from accepting connections.
		service_->Stop();

		// Unregister current clients correctly.
		while ( clients_.size() > 0 )
		{
			// Retrieve next client.
			mc::IProtocolPtr client = (*clients_.begin()).first;
			// Disconnect client forcibly.
			UnregisterClient(client);
		}

		return mc::_S_OK;
	}

	mc::result ServerContext::HandleClient(size_t cookie, bool auth)
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

		// Find client.
		mc::IProtocolPtr client = ClientByCookie( cookie );
		if ( !client )
			return mc::_E_FAIL;

		return HandleClient(client, auth);
	}

	mc::result ServerContext::HandleClient(mc::IProtocolRef client, bool auth)
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

		if ( auth )
		{
			// Find client record.
			Client& clientRecord = clients_[client];
			// Remember authentication flag.
			clientRecord.SetAuthenticated(auth);
			// Notify UI on new client.
            events_->OnNewClient(client->GetCookie());
			// Notify client on successful authentication.			
			client->Send( PrepareAuthMessage( clientRecord.GetMajorVersion(), clientRecord.GetMinorVersion() ) );
			// Notify client on current volume value. 
			SendVolumeLevel(client);
			// Ensure that client supports zooming.
			if ( clientRecord.IsZoomingSupported() )
			{
				// Notify client on zoom parameters.
				client->Send( HardwareProvider::Instance().PrepareZoomParams() );
			}
            // Notify services manager on new client.
            servicesManager_->DeviceConnected(client, clientRecord.GetAuthMessage());
		}
		else
		{
			UnregisterClient(client);
		}		

		return mc::_S_OK;
	}

	bool ServerContext::HasClients()
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );
		// TODO: This should be improved to return value depending on a number
		// of authenticated clients.
		return ( clients_.size() > 0 );
	}

	bool ServerContext::HasClient(size_t cookie) 
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );
		return ( NULL != ClientByCookie(cookie) );
	}

	Client ServerContext::GetClient(size_t cookie)
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );
		for ( Clients_::iterator iter = clients_.begin() ; iter != clients_.end() ; ++iter )
		{
			// Acquire next client.
			mc::IProtocolRef client( (*iter).first );
			if ( !client )
				continue;
			// Check whether this is the one we are looking for.
			if ( !client->TestCookie( cookie ) )
				continue;
			return (*iter).second;
		}		
		throw std::exception();
	}
	
	bool ServerContext::TestClient(const Client& client)
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

		try 
		{
			// Check for limit of free connections.
			//////////////////////////////////////////////////////////////////////////
			if ( client.IsFree() )
			{
				// Enumerate all connected clients.
				for ( Clients_::iterator iter = clients_.begin() ; iter != clients_.end() ; ++iter )
				{
					// Attempt to find at least someone free and authenticated.
					if ( (*iter).second.IsAuthenticated() && 
						(*iter).second.IsFree() )
					{
						// Acquire protocol associated with client to deliver
						// limit message.
						mc::IProtocolPtr clientProtocol = ClientByCookie( client.GetCookie() );
						if ( !clientProtocol )
							return false;

						// Send message to notify client on the reason of
						// disconnection.
						mc::IMessagePtr limitMessage( mc::Class< mc::MessageImpl >::Create( Messages::limitReached_ ) );
						clientProtocol->Send( limitMessage );
						
						// Disconnection should not be performed here because it
						// leads to dead lock. Honest client should disconnect 
						// itself.
						
						// Notify caller on result of the operation.
						return false;
					}
				}
			}

#if defined(__MACH__)
			// Check for iRemove version.
			//////////////////////////////////////////////////////////////////////////
			if ( !Features::iRemoteMac(client.GetMajorVersion(), client.GetMinorVersion()) )
			{
				// Notify user locally on version incompatibility.
				mc::IRunnablePtr warnContext(
					mc::Class< OldiRemoteContext >::Create( client.GetCookie(), events_ ) );
				authJobs_->Push(warnContext);
				// Disconnection should not be performed here.
				return false;
			}
#endif // Platform

			// Another checks may follow here.
		}
		catch ( ... )
		{
			return false;
		}		

		return true;
	}

	mc::result ServerContext::SpreadMessage(mc::IMessageRef message)
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

		// Enumerate all connected clients.
		for ( Clients_::iterator iter = clients_.begin() ; iter != clients_.end() ; ++iter )
		{
			// Acquire next client.
			mc::IProtocolRef client( (*iter).first );

			// Check whether client is alive and is authenticated.
			if ( !client || !(*iter).second.IsAuthenticated() )
				continue;

			// Send message to next client.
			client->Send( message );
		}
		return mc::_S_OK;
	}

	mc::result ServerContext::RequestPassword(size_t cookie, bool requestAuth)
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

		// Find client.
		mc::IProtocolPtr client = ClientByCookie( cookie );
		if ( !client )
			return mc::_E_FAIL;

		// Submit password request.
		RequestPassword(client, requestAuth);

		return mc::_S_OK;
	}

	mc::result ServerContext::DisconnectClient(size_t cookie)
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

		// Find client.
		mc::IProtocolPtr client = ClientByCookie( cookie );
		if ( !client )
			return mc::_E_FAIL;

		// Target connection has been found. Use internal tool to disconnect.
		UnregisterClient( client );
		return mc::_S_OK;
	}

	void ServerContext::HandleAuthorized(IClientHandlerRef handler)
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

		if ( !handler )
			return;

		// Enumerate all connected clients.
        Clients_::iterator iter = clients_.begin();
        Clients_::iterator end = clients_.end();
        for ( ; iter != end ; ++iter )
		{
			// Acquire next client.
			mc::IProtocolRef clientProtocol( (*iter).first );
			Client clientRecord( (*iter).second );

			// Check whether client is alive and is authenticated.
			if ( !clientProtocol || !clientRecord.IsAuthenticated() )
				continue;
			
			// Handle client.
			handler->Handle(clientRecord, clientProtocol, ClassRoot_::GetSelf());
		}
	}

    IServicesManagerPtr ServerContext::ServicesManager()
    {
        return servicesManager_;
    }

	// DServer section
	mc::result ServerContext::ConnectionAccepted(mc::IConnectionRef connection)
	{	
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

		// Handle connection.
		mc::IProtocolPtr client( mc::Library::Protocol(connection, mc::IProtocol::ASYNC) ); 
		// Configure messages registry.
		client->GetMessagesMap()->Register(MoveMessage::messageId_, mc::Class< MoveMessage >::Create );
        client->GetMessagesMap()->Register(MoveToMessage::messageId_, mc::Class< MoveToMessage >::Create );
        client->GetMessagesMap()->Register(MouseClickMessage::messageId_, mc::Class< MouseClickMessage >::Create );
		client->GetMessagesMap()->Register(ScrollMessage::messageId_, mc::Class< ScrollMessage >::Create );
        client->GetMessagesMap()->Register(ScrollMessage2::messageId_, mc::Class< ScrollMessage2 >::Create );
		client->GetMessagesMap()->Register(AuthMessage::messageId_, mc::Class< AuthMessage >::Create );
		client->GetMessagesMap()->Register(KeyboardMessage::messageId_, mc::Class< KeyboardMessage >::Create );
		client->GetMessagesMap()->Register(Messages::leftButtonDown_, mc::Class< mc::MessageImpl >::Create );
		client->GetMessagesMap()->Register(Messages::leftButtonUp_, mc::Class< mc::MessageImpl >::Create );
		client->GetMessagesMap()->Register(Messages::middleButtonDown_, mc::Class< mc::MessageImpl >::Create );
		client->GetMessagesMap()->Register(Messages::middleButtonUp_, mc::Class< mc::MessageImpl >::Create );
		client->GetMessagesMap()->Register(Messages::rightButtonDown_, mc::Class< mc::MessageImpl >::Create );
		client->GetMessagesMap()->Register(Messages::rightButtonUp_, mc::Class< mc::MessageImpl >::Create );
		client->GetMessagesMap()->Register(VolumeMessage::messageId_, mc::Class< VolumeMessage >::Create );
		client->GetMessagesMap()->Register(CookieMessage::messageId_, mc::Class< CookieMessage >::Create );
		client->GetMessagesMap()->Register(PasswordMessage::messageId_, mc::Class< PasswordMessage >::Create );
		client->GetMessagesMap()->Register(ShellRequest::messageId_, mc::Class< ShellRequest >::Create );
		client->GetMessagesMap()->Register(ZoomLevelMessage::messageId_, mc::Class< ZoomLevelMessage >::Create );
        client->GetMessagesMap()->Register(ServiceRequest::messageId_, mc::Class< ServiceRequest >::Create );
        client->GetMessagesMap()->Register(StreamRequest::messageId_, mc::Class< StreamRequest >::Create );

		// Subscribe on message notifications.
		mc::Events::Advise(client, 
			mc::CommonImpl< mc::IRunnable >::GetSelf(), 
			mc::TypeInfo< mc::DProtocol >::GetGuid());
		
		// Register client and set it's state to unregistered.
		clients_[client] = Client( connection->GetIpAddress(), client->GetCookie() );

		return mc::_S_OK;
	}

	// DProtocol section
	mc::result ServerContext::MessageArrived(mc::IProtocolRef protocol, mc::IMessageRef message)
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

		// TODO: Check ban list. 

        // Check for service initialization message.
        if ( StreamRequest::messageId_ == message->GetCode() )
        {
            HandleStream(protocol, message);
            return mc::_S_OK;
        }

		// First of all we should check whether authentication message 
		// is arrived or no.
        if ( AuthMessage::messageId_ == message->GetCode() )
		{
			HandleAuthMessage(protocol, message);
            return mc::_S_OK;
		}

		// Check whether auth message has been already received.
		if ( !clients_[protocol].HasAuthMessage() )
		{
			// Auth message has not been received yet.
			UnregisterClient(protocol);
			return mc::_S_OK;
		}

		// Then we should proceed to password check.
		if ( PasswordMessage::messageId_ == message->GetCode() )
		{
			HandlePassword(protocol, message);
			return mc::_S_OK;
		}

		if ( false == clients_[protocol].IsAuthenticated() )
		{
			// Connection has not been authorized yet. Client should be 
			// disconnected in this case.
			UnregisterClient(protocol);
			return mc::_S_OK;
		}		
		
		if ( MoveMessage::messageId_ == message->GetCode() )
		{
            HandleMoveByMessage(protocol, message);
        }
        else if ( MoveToMessage::messageId_ == message->GetCode() )
        {
            HandleMoveToMessage(protocol, message);
        }
        else if ( MouseClickMessage::messageId_ == message->GetCode() )
        {
            HandleMouseClickMessage(message);
        }
		else if ( ScrollMessage::messageId_ == message->GetCode() )
		{
			HandleScrollMessage(message);
		}
        else if ( ScrollMessage2::messageId_ == message->GetCode() )
        {
            HandleScrollMessage2(message);
        }
		else if ( Messages::leftButtonDown_ == message->GetCode() )
		{
			HardwareProvider::Instance().GetMouseControl()->LeftButtonDown();
		}
		else if ( Messages::leftButtonUp_ == message->GetCode() )
		{
			HardwareProvider::Instance().GetMouseControl()->LeftButtonUp();
		}
		else if ( Messages::middleButtonDown_ == message->GetCode() )
		{
			HardwareProvider::Instance().GetMouseControl()->MiddleButtonDown();
		}
		else if ( Messages::middleButtonUp_ == message->GetCode() )
		{
			HardwareProvider::Instance().GetMouseControl()->MiddleButtonUp();
		}
		else if ( Messages::rightButtonDown_ == message->GetCode() )
		{
			HardwareProvider::Instance().GetMouseControl()->RightButtonDown();
		}
		else if ( Messages::rightButtonUp_ == message->GetCode() )
		{
			HardwareProvider::Instance().GetMouseControl()->RightButtonUp();
		}
		else if ( KeyboardMessage::messageId_ == message->GetCode() )
		{
			HandleKeyboardMessage(message);
		}
		else if ( VolumeMessage::messageId_ == message->GetCode() )
		{
			HandleVolumeMessage(message);
		}
		else if ( CookieMessage::messageId_ == message->GetCode() )
		{
			HandleCookieMessage(message);
		}
		else if ( ShellRequest::messageId_ == message->GetCode() )
		{
			HandleShellRequest(message);
		}
		else if ( ZoomLevelMessage::messageId_ == message->GetCode() )
		{
			HandleZoomLevel(protocol, message);
		}

        // Propogate message to services manager. This is required in order to deliver
        // compatibility information and general commands to service handlers.
        servicesManager_->ControlMessage(protocol, message);

		return mc::_S_OK;
	}

	mc::result ServerContext::DataErrorOccured(mc::IProtocolRef protocol)
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

		// What we need to do is to unregister client correctly. 
		UnregisterClient(protocol);

		return mc::_S_OK;
	}

	mc::result ServerContext::Disconnected(mc::IProtocolRef protocol)
	{
		MC_LOG_ROUTINE;
		mc::CoreMutexLock locker( mc::CommonImpl< IServerControl >::GetLock() );

        // What we need to do is to unregister client correctly.
        UnregisterClient(protocol);

		return mc::_S_OK;
	}

	// Internal tools
	//////////////////////////////////////////////////////////////////////////
	void ServerContext::SendVolumeLevel(mc::IProtocolRef client)
	{
		// Acquire current volume value.
		unsigned long value = 0;
		HardwareProvider::Instance().GetVolumeControl()->GetNormalizedVolume(value);
		// Construct message do spread new value.
		mc::IMessagePtr message( mc::Class< VolumeMessage >::Create( VolumeMessage::DEVICE_MASTER, value ) );
		// Send message.
		client->Send(message);
	}

	mc::IProtocolPtr ServerContext::ClientByCookie(size_t cookie)
	{
		MC_LOG_ROUTINE;
		for ( Clients_::iterator iter = clients_.begin() ; iter != clients_.end() ; ++iter )
		{
			// Acquire next client.
			mc::IProtocolRef client( (*iter).first );
			if ( !client )
				continue;
			// Check whether this is the one we are looking for.
			if ( !client->TestCookie( cookie ) )
				continue;
			return client;
		}
		return NULL;
	}

	void ServerContext::UnregisterClient(mc::IProtocolRef protocol)
	{
		MC_LOG_ROUTINE;
		// Save cookie for later use.
		size_t cookie = protocol->GetCookie();		
        // 1. Notify services on disconnection.
        servicesManager_->DeviceDisconnected(protocol);
        // 2. Unsubscribe from protocol events.
		mc::Events::Unadvise(protocol, 
			mc::CommonImpl< mc::IRunnable >::GetSelf(), 
			mc::TypeInfo< mc::DProtocol >::GetGuid());
        // 3. It is not enough just to erase protocol from the map to
		// shutdown connection properly.
        // 3.1. Acquire connection.
		mc::IConnectionPtr connection = protocol->GetConnection();
        // 3.2. Close connection explicitly.
		connection->Close();
        // 3.3. Remove protocol object from cache finally.
		clients_.erase( protocol );
        // 4. Notify UI on disconnection.
		events_->OnDisconnection( cookie );
	}

    mc::result ServerContext::HandleStream(mc::IProtocolRef protocol, mc::IMessageRef message)
    {
        MC_LOG_ROUTINE;
        // 1. Unsubscribe from protocol events.
        mc::Events::Unadvise(protocol,
            mc::CommonImpl< mc::IRunnable >::GetSelf(),
            mc::TypeInfo< mc::DProtocol >::GetGuid());
        // 2. Remove protocol object from cache.
        clients_.erase( protocol );
        // 3. Notify services manager on stream.
        servicesManager_->StreamConnected(protocol, message);
        return mc::_S_OK;
    }

	mc::result ServerContext::HandleAuthMessage(mc::IProtocolRef protocol, mc::IMessageRef message)
	{
		MC_LOG_ROUTINE;

		// TODO: Message contains information on client device and version.
		// This should be checked in future. Host software should work properly
		// with clients of all generations. 

		// Attach auth message to client.
		clients_[protocol].AttachMessage(message);

		// Check whether limit of free connected devices is reached.
		if ( !TestClient(clients_[protocol]) )
			// This routine takes care about client disconnection itself.
			return mc::_S_FALSE;

		// Check whether client has valid authentication code.
		bool hasCode = message->GetProperty(AuthMessage::PROP_HAS_CODE).toBool();
		if ( hasCode )
		{
			// Check whether client's code is valid.
			mc::Guid code = message->GetProperty(AuthMessage::PROP_CODE).toGuid();
			if ( listOfCookies_.end() != listOfCookies_.find(code) ) 
			{
				// Remove cookie. 
				listOfCookies_.erase(code);
				
				// Authorize client.
				HandleClient( protocol, true );	

				// As new client has appeared we should check whether another on
				// which may be potentially free is waiting for approval. 
				events_->CheckAuth();

				// Finish authentication scenario after this point.
				return mc::_S_OK;
			}
		}

		// Check current security policy.
		if ( std::string("allow_all") == Config::Instance().GetPolicy() )
		{
			// This policy allows for all external devices to be automatically 
			// connected without confirmation dialog.

			// Authorize client.
			HandleClient( protocol, true );			

			// Finish authentication scenario after this point.
			return mc::_S_OK;
		}

		// Check whether client is registered in local users database.
		try
		{
			// Lock users manager for reading.
			mc::IReadWriteLock::Reader_ locker(UsersManager::Instance().GetLock());
			
			// Find corresponding user in the database.
			std::string deviceId = message->GetProperty(AuthMessage::PROP_ID).toString();
			User& user = UsersManager::Instance().GetUser(deviceId);

			// Update user settings.
			std::string deviceName = message->GetProperty(AuthMessage::PROP_NAME).toString();
			user.SetDeviceName(deviceName);

			// Acquire ip address of current connection.
			std::string currentIp = protocol->GetConnection()->GetIpAddress();

			// Check whether ip security option enabled.
			if ( user.IsIpRequired() )
			{
				// Check whether user is connected with another ip address.
				if ( user.GetIp() != currentIp )
				{
					// Notify device on wrong ip.
					mc::IMessagePtr wrongIp = mc::Class< mc::MessageImpl >::Create( Messages::wrongDeviceIp_ );
					protocol->Send( wrongIp );	

					// Break connection with the device.
					UnregisterClient(protocol);
					return mc::_S_FALSE;
				}
			}
			else
			{
				// This is right place to update last user ip address.
				// This makes sense only when user is not associated woth any
				// particular ip.
				user.SetIp( currentIp );
			}

			// Request password if user is found.
			RequestPassword(protocol, user.IsAuthRequested());

			return mc::_S_OK;
		}
		catch ( ... )
		{
			// User has not been found in the database. 
			// Proceed to regular authentication.
		}

		// Initiate authentication (display authentication dialog).
		mc::IRunnablePtr authContext( 
			mc::Class< AuthContext >::Create( protocol->GetCookie(), events_ ) );
		authJobs_->Push(authContext);

		return mc::_S_OK;
	}

    mc::result ServerContext::HandleMoveByMessage(mc::IProtocolRef protocol, mc::IMessageRef message)
	{
		// Acquire message params.
        float dx = message->GetProperty(MoveMessage::PROP_DX).toFloat();
        float dy = message->GetProperty(MoveMessage::PROP_DY).toFloat();
        // Adjust cursor position.
        HardwareProvider::Instance().GetMouseControl()->MoveCursorBy(dx, dy);
        // Notify client on cursor position change.
        if ( clients_[protocol].AreServicesSupported() )
        {
            ScreenPoint position = HardwareProvider::Instance().GetMouseControl()->GetPosition();
            mc::IMessagePtr positionMessage( mc::Class< MoveToMessage >::Create(position.x_, position.y_) );
            protocol->Send(positionMessage);
        }
		return mc::_S_OK;
	}

    mc::result ServerContext::HandleMoveToMessage(mc::IProtocolRef protocol, mc::IMessageRef message)
    {
        // Acquire message params.
        float x = message->GetProperty(MoveMessage::PROP_DX).toFloat();
        float y = message->GetProperty(MoveMessage::PROP_DY).toFloat();
        // Adjust cursor position.
        HardwareProvider::Instance().GetMouseControl()->MoveCursorTo(x, y);
        // Notify client on cursor position change.
        if ( clients_[protocol].AreServicesSupported() )
        {
            ScreenPoint position = HardwareProvider::Instance().GetMouseControl()->GetPosition();
            mc::IMessagePtr positionMessage( mc::Class< MoveToMessage >::Create(position.x_, position.y_) );
            protocol->Send(positionMessage);
        }
        return mc::_S_OK;
    }

    mc::result ServerContext::HandleMouseClickMessage(mc::IMessageRef message)
    {
        MC_LOG_ROUTINE;
        // Acquire message params.
        float x = message->GetProperty(MouseClickMessage::PROP_X).toFloat();
        float y = message->GetProperty(MouseClickMessage::PROP_Y).toFloat();

        // Perform corresponding action.
        HardwareProvider::Instance().GetMouseControl()->MouseClick(x, y);

        return mc::_S_OK;
    }

	mc::result ServerContext::HandleScrollMessage(mc::IMessageRef message)
	{
		MC_LOG_ROUTINE;
		// Acquire message params.
		bool vertical = message->GetProperty(ScrollMessage::PROP_ORIENTATION).toBool();
		int value = message->GetProperty(ScrollMessage::PROP_VALUE).toInt();

		// Perform corresponding action.
		HardwareProvider::Instance().GetMouseControl()->Scroll(vertical, value);

		return mc::_S_OK;
	}

    mc::result ServerContext::HandleScrollMessage2(mc::IMessageRef message)
    {
        MC_LOG_ROUTINE;
        // Acquire message params.
        bool vertical = message->GetProperty(ScrollMessage::PROP_ORIENTATION).toBool();
        int value = message->GetProperty(ScrollMessage::PROP_VALUE).toInt();

        // Perform corresponding action.
        HardwareProvider::Instance().GetMouseControl()->Scroll2(vertical, value);

        return mc::_S_OK;
    }

	mc::result ServerContext::HandleKeyboardMessage(mc::IMessageRef message)
	{	
		MC_LOG_ROUTINE;
		int type = message->GetProperty(KeyboardMessage::PROP_TYPE).toInt();
        if ( KeyboardMessage::TYPE_UNICODE == type )
		{
			unsigned short code = message->GetProperty(KeyboardMessage::PROP_SCAN).toUShort();
			HardwareProvider::Instance().GetKeyboardControl()->KeyboardEventUnicode(code);
		}
		else if ( KeyboardMessage::TYPE_VIRTUAL == type )
		{
			unsigned char vk = message->GetProperty(KeyboardMessage::PROP_VK).toUChar();
			bool event = message->GetProperty(KeyboardMessage::PROP_EVENT).toBool();
			HardwareProvider::Instance().GetKeyboardControl()->KeyboardEventVirtual(vk, event);
		}

		return mc::_S_OK;
	}

	mc::result ServerContext::HandleVolumeMessage(mc::IMessageRef message)
	{
		MC_LOG_ROUTINE;
		unsigned long value = message->GetProperty(VolumeMessage::PROP_VALUE).toULong();
		HardwareProvider::Instance().GetVolumeControl()->SetNormalizedVolume(value);
		return mc::_S_OK;
	}

	mc::result ServerContext::HandleCookieMessage(mc::IMessageRef message)
	{
		MC_LOG_ROUTINE;
		mc::Guid code = message->GetProperty(CookieMessage::PROP_CODE).toGuid();
		listOfCookies_.insert(code);
		return mc::_S_OK;
	}

	mc::result ServerContext::HandleShellRequest(mc::IMessageRef message)
	{		
		IShellControlPtr shellControl = ShellHost::Instance().GetShellControl();
		if ( shellControl )
		{
			std::string command = message->GetProperty(ShellRequest::PROP_COMMAND).toString();
			shellControl->Write(command, true);
		}
		else
		{
			// TODO: Client should be notified when shell service is not started
			// on host side.
		}
		return mc::_S_OK;
	}

	mc::result ServerContext::HandlePassword(mc::IProtocolRef protocol, mc::IMessageRef message)
	{
		MC_LOG_ROUTINE;
		try
		{
			if ( clients_.end() == clients_.find(protocol) )
				throw std::exception();

			// Acquire device id from client record.
			std::string deviceId = clients_[protocol].GetId();
			// Acquire password from message.
			std::string passsword = message->GetProperty(PasswordMessage::PROP_PASSWORD).toString();

			// Lock users manager for reading.
			mc::IReadWriteLock::Reader_ locker(UsersManager::Instance().GetLock());

			// Extract user from the database.
			User& user = UsersManager::Instance().GetUser(deviceId);

			// Check whether password is correct.
			if ( user.TestPassword(passsword) )
			{
				// Check whether client satisfies all criteria and current 
				// situation.
				if ( !TestClient(clients_[protocol]) )
					return mc::_S_FALSE;

				// Forced authentication on the device is not required any more.
				user.RequestAuth(false);

				// Authorize client.
				HandleClient(protocol, true);
			}
			else
			{	
				// Check whether user still exists in the database.
				if ( !UsersManager::Instance().IsValidUser(deviceId) )  
					// This will cause user disconnection.
					throw std::exception();
					// TODO: Authentication should be requested here. This will 
					// initiate authentication procedure.

				// Notify client on password error.
				// Typically this should lead to the same password prompt (may be 
				// with additional prompt on error during previous attempt).
				// Server side should take care about limited number of attempts.
				WrongPassword(protocol);
			}
		}	
		catch ( ... )
		{
			// It may occur in the following cases:
			// - authentication message has not been received from client yet;
			// - user has not been found in the database.
			// Connection should be broken in this case.
			UnregisterClient(protocol);
			return mc::_E_FAIL;
		}
	
		return mc::_S_OK;
	}

	mc::result ServerContext::HandleZoomLevel(mc::IProtocolRef protocol, mc::IMessageRef message)
	{
		MC_LOG_ROUTINE;

		// Client record should exists to handle zoom message correctly.
		if ( clients_.end() == clients_.find(protocol) )
		{
			UnregisterClient(protocol);
			return mc::_E_FAIL;
		}

		// Remember new zoom level. It will be used to capture next screenshot.
		float level = message->GetProperty(ZoomLevelMessage::PROP_ZOOM_LEVEL).toFloat();
		clients_[protocol].SetZoomLevel(level);

		return mc::_S_OK;
	}

	mc::IMessagePtr ServerContext::PrepareAuthMessage(int clientMajor, int clientMinor)
	{
		// Note: Auth response message determines host platform automatically. 
		if ( Features::iRemoteExtendedAuthentication(clientMajor, clientMinor) )
			return mc::IMessagePtr( mc::Class< AuthResponse2 >::Create( Config::Instance().GetHostId() ) );
		else
			return mc::IMessagePtr( mc::Class< AuthResponse >::Create() );
	}

	mc::result ServerContext::RequestPassword(mc::IProtocolRef protocol, bool requestAuth)
	{
		mc::IMessagePtr passwordRequest = mc::Class< PasswordRequest >::Create( 
			Config::Instance().GetHostId(), requestAuth );
		protocol->Send( passwordRequest );		
		return mc::_S_OK;
	}

	mc::result ServerContext::WrongPassword(mc::IProtocolRef protocol)
	{
		protocol->Send( mc::Class< mc::MessageImpl >::Create( Messages::wrongPassword_ ) );		
		return mc::_S_OK;
	}
}
