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

#include "Server.h"

#include "Messages/MoveMessage.h"
#include "Messages/MoveToMessage.h"
#include "Messages/MouseClickMessage.h"
#include "Messages/ScrollMessage.h"
#include "Messages/ScrollMessage2.h"
#include "Messages/AuthMessage.h"
#include "Messages/KeyboardMessage.h"
#include "Messages/Messages.h"
#include "Messages/VolumeMessage.h"
#include "Messages/CookieMessage.h"
#include "Messages/PasswordMessage.h"
#include "Messages/ShellRequest.h"
#include "Messages/ZoomLevelMessage.h"
#include "Messages/AuthResponse.h"
#include "Messages/AuthResponse2.h"

// Server support
#include "IMessageService.h"
#include "IServerControl.h"
#include "IUIEvents.h"
#include "Services/Services.h"
#include "RenderingQueue.h"
#include "ServerContext.h"
#include "Device.h"

// Session support
#include "Hosts/Hosts.h"
#include "Session.h"

// Md5 calculator
#include <hashlibpp.h>

namespace RemotePC
{

    Holder& Holder::Instance()
    {
        return Loki::SingletonHolder< Holder >::Instance();        
    }    
    
    Holder::Holder()
	    : serverThread_()
		, service_( mc::Library::Service() )	
		, servicesManager_( mc::Class< RemotePC::ServicesManager >::Create(service_) )	
	    , serverControl_()
	    , events_()
        , hasCookie_( false )
        , cookie_( mc::TypeInfo< mc::ICommon >::GetGuid() ) 
        , cookieLock_( false )
    {
    }
	
	void Holder::SetEvents(IUIEventsRef events)
	{
		events_ = events;
	}
	
	mc::IServicePtr Holder::GetService() const
	{
		return service_;
	}
	
	IServicesManagerPtr Holder::ServicesManager()
	{
		return servicesManager_;
	}
    
    void Holder::Connect(const std::string& ip, const std::string& port)
    {
		// Check whether holder is configured correctly.
		if ( !events_ )
			return;
		
        // Construct authentication message.
		mc::IMessagePtr authMessage;
        if ( hasCookie_ )
            authMessage = mc::Class< AuthMessage >::Create( 
                AuthMessage::DEVICE_IPHONE, MinCOM::iPhone::Device::GetId(), MinCOM::iPhone::Device::GetName(), cookie_ );
        else
            authMessage = mc::Class< AuthMessage >::Create( 
                AuthMessage::DEVICE_IPHONE, MinCOM::iPhone::Device::GetId(), MinCOM::iPhone::Device::GetName() );            
        
        // Start listening service.
        serverThread_ = mc::Library::Thread();
        serverControl_ = mc::Class< RemotePC::ServerContext >::Create( events_, service_, ip, port, authMessage, servicesManager_ );
		serverThread_->SetContext(serverControl_);
        serverThread_->Start();
		
		// Start new session.
		Session::Instance().Flush();
		// Add interface with current connection settings to the host, which is
		// stored in current session. This host will be saved upon successful
		// connection attempt.
		Session::Instance().GetCurrent().AddInterface( NetInterface(ip, port) );
    }
    
    void Holder::Stop()    
    {
        // Reset cookie. Procedure takes care about cookie locking internally.
        ReleaseCookie();
        // TOINV: Determine when serverControl_ or serverThread_ becomes NULL.  
        // Stop listening service.        
		if ( serverControl_ )
			serverControl_->Stop();
		if ( serverThread_ )
			serverThread_->Join();
        serverThread_ = NULL;
        serverControl_ = NULL;
    }
    
    bool Holder::IsConnected()
    {
        return ( NULL != serverControl_ );        
    }
    
    void Holder::ExecuteShellCommand(const std::string& command)
    {
		try {
			mc::IMessagePtr shellRequest( mc::Class< ShellRequest >::Create( command ) );
			serverControl_->Send( shellRequest );   
		} catch ( ... ) {}
    }
    
    void Holder::SubmitPassword(const std::string& password)
    {
		try {		
			// Calculate md5 hash of a password.
			std::string passwordHash = CalculateMD5(password);		
			// Sumbit password.
			mc::IMessagePtr passwordMessage( mc::Class< PasswordMessage >::Create( passwordHash ) );
			serverControl_->Send( passwordMessage );   
			// Save password in session.
			Session::Instance().GetCurrent().SetMd5Password(passwordHash);	
		} catch ( ... ) {}			
    }
    
    void Holder::OnMovedBy(float dx, float dy)
    {
		try {		
			mc::IMessagePtr moveMessage( mc::Class< MoveMessage >::Create( dx, dy ) );
			serverControl_->Send( moveMessage );       
		} catch ( ... ) {}					
    }
	
	void Holder::OnMovedTo(float x, float y)
	{
		try {		
			mc::IMessagePtr moveMessage( mc::Class< MoveToMessage >::Create( x, y ) );
			serverControl_->Send( moveMessage );       
		} catch ( ... ) {}							
	}
	
	void Holder::OnClicked(float x, float y)	
	{
		try {		
			mc::IMessagePtr clickMessage( mc::Class< MouseClickMessage >::Create( x, y ) );
			serverControl_->Send( clickMessage );       
		} catch ( ... ) {}					
	}
    
    void Holder::OnScrolled(bool vertical, int direction)
    {
		try {		
			mc::IMessagePtr scrollMessage( mc::Class< ScrollMessage >::Create( vertical, direction ) );
			serverControl_->Send( scrollMessage );       
		} catch ( ... ) {}								
    }
	
    void Holder::OnScrolled2(bool vertical, int direction)
    {
		try {		
			mc::IMessagePtr scrollMessage( mc::Class< ScrollMessage2 >::Create( vertical, direction ) );
			serverControl_->Send( scrollMessage );       
		} catch ( ... ) {}								
    }	
    
    void Holder::OnLeftDown()
    {
		try {		
			mc::IMessagePtr message( mc::Class< mc::MessageImpl >::Create( Messages::leftButtonDown_ ) );
			serverControl_->Send( message );      
		} catch ( ... ) {}					
    }
    
    void Holder::OnLeftUp()
    {
		try {		
			mc::IMessagePtr message( mc::Class< mc::MessageImpl >::Create( Messages::leftButtonUp_ ) );
			serverControl_->Send( message );    
		} catch ( ... ) {}								
    }    
    
    void Holder::OnMiddleDown()
    {
		try {		
			mc::IMessagePtr message( mc::Class< mc::MessageImpl >::Create( Messages::middleButtonDown_ ) );
			serverControl_->Send( message );    
		} catch ( ... ) {}								
    }
    
    void Holder::OnMiddleUp()            
    {
		try {		
			mc::IMessagePtr message( mc::Class< mc::MessageImpl >::Create( Messages::middleButtonUp_ ) );
			serverControl_->Send( message );    
		} catch ( ... ) {}								
    }
    
    void Holder::OnRightDown()
    {
		try {		
			mc::IMessagePtr message( mc::Class< mc::MessageImpl >::Create( Messages::rightButtonDown_ ) );
			serverControl_->Send( message );    
		} catch ( ... ) {}								
    }
    
    void Holder::OnRightUp()
    {
		try {		
			mc::IMessagePtr message( mc::Class< mc::MessageImpl >::Create( Messages::rightButtonUp_ ) );
			serverControl_->Send( message );    
		} catch ( ... ) {}								
    }     
    
    void Holder::OnVirtualKey(unsigned char key, bool keydown)
    {
		try {		
			mc::IMessagePtr message( mc::Class< KeyboardMessage >::Create( KeyboardMessage::TYPE_VIRTUAL, key, keydown ) );
			serverControl_->Send( message ); 
		} catch ( ... ) {}								
    }
    
    void Holder::OnUnicodeKey(unsigned short key)
    {
		try {		
			mc::IMessagePtr message( mc::Class< KeyboardMessage >::Create( KeyboardMessage::TYPE_UNICODE, key ) );
			serverControl_->Send( message );    
		} catch ( ... ) {}								
    }
    
    void Holder::OnVolume(unsigned long value)
    {
		try {		
			mc::IMessagePtr message( mc::Class< VolumeMessage >::Create( VolumeMessage::DEVICE_MASTER, value ) );
			serverControl_->Send( message );    
		} catch ( ... ) {}								
    }
    
    void Holder::OnZoom(float level)
    {
		try {		
			mc::IMessagePtr message( mc::Class< ZoomLevelMessage >::Create( level ) );
			serverControl_->Send( message );          
		} catch ( ... ) {}								
    }
    
    IServerControlPtr Holder::GetServer()
    {
        return serverControl_;
    }
    
    void Holder::SetCookie(const mc::Guid& cookie)
    {
        hasCookie_ = true;
        cookie_ = cookie;
        // Cookie should not be locked initially.
        LockCookie(false);
    }
    
    void Holder::ReleaseCookie()
    {
        // Locking prevents from releasing cookie while going to spleep.
        if ( !cookieLock_ )
            hasCookie_ = false;        
    }    
    
    bool Holder::HasCookie()
    {
        return hasCookie_;
    }
    
    void Holder::LockCookie(bool lock)
    {
        if ( !hasCookie_ )
            return;
        
        // Prevents from removing cookie.
        cookieLock_ = lock;
        // If connection with host is alive and cookie is being locked.
        if ( serverControl_ && cookieLock_ ) 
        {
            // Notify host on cookie so it may be used for futher reconnection.
            mc::IMessagePtr cookieMessage( mc::Class< CookieMessage >::Create( cookie_ ) );
            serverControl_->Send(cookieMessage);
        }
    }
	
    mc::result Holder::GetRemotePCVersion(int & major, int & minor)
	{
		try
		{
			// Acquire server response message.
			if ( !serverControl_ ) 
				return mc::_E_FAIL;
			mc::IMessagePtr authResponse = serverControl_->GetAuthResponse();
			if ( !authResponse )
				return mc::_E_FAIL;
			// Acquire server version.
			major = authResponse->GetProperty(AuthResponse2::PROP_VER_MAJOR).toInt();
			minor = authResponse->GetProperty(AuthResponse2::PROP_VER_MINOR).toInt();
			return mc::_S_OK;		
		}
		catch ( ... )
		{
			// We can get here, if auth response message does not contain 
			// PROP_VER_MAJOR or PROP_VER_MAJOR. This is true for RemotePC 1.1
			// and older. 
			// Of course, zooming is not supported int this case.
			return mc::_E_FAIL;
		}
	}
	
	// Private tools.
	std::string Holder::CalculateMD5(const std::string& source)
	{		
		try
		{
			Loki::SmartPtr< hashwrapper > wrapper ( new md5wrapper() );
			return wrapper->getHashFromString(source);
		}
		catch( ... )
		{
			return std::string("");
		}
		// Exception handling should be probably removed here to allow 
		// caller to deal with an error at its discretion. 
	}
    
}
