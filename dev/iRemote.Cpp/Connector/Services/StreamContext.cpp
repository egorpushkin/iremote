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

#include "Services.h"

#include "Messages/StreamRequest.h"
#include "Messages/Messages.h"

namespace RemotePC
{
	const size_t StreamContext::maxAttempts_ = 5;	
	
	StreamContext::StreamContext(const IServicesManagerPtr& servicesManager, const IServiceHandlerPtr& handler, mc::IServiceRef service, const std::string& ip, mc::IMessageRef message, mc::IThreadRef thread)
		: mc::CommonImpl< mc::IRunnable >()
		, servicesManager_( servicesManager )
		, handler_( handler )  
		, service_( service )	
		, ip_( ip )
		, message_( message )
		, thread_( thread )
	{
	}
	
	/**
	 * IThread section
	 */
	
	mc::result StreamContext::Run()
	{
		// Get arguments.
		std::string port = message_->GetProperty(StreamRequest::PROP_PORT).toString();

        // Establish connection.
		mc::IConnectionPtr connection;
		mc::result code = mc::_E_FAIL;
		for ( size_t attempt = 0 ; attempt < maxAttempts_ ; ++attempt )
		{
			connection = mc::Library::TCPConnection( service_ );
			code = connection->Establish( ip_, port );
			if ( mc::Error::IsSucceeded( code ) )
			{
				break;
			}
		}
		if ( mc::Error::IsFailed( code ) )
		{
			servicesManager_->RequestFailed(handler_);
			return mc::_S_OK;
		}		
		
        // Wrap stream.
		mc::IProtocolPtr stream( mc::Library::Protocol(connection, mc::IProtocol::ASYNC) );						

		// Register stream status messages. 
		stream->GetMessagesMap()->Register(Messages::streamSuccess_, mc::Class< mc::MessageImpl >::Create );
		stream->GetMessagesMap()->Register(Messages::streamFailure_, mc::Class< mc::MessageImpl >::Create );
		
		// Notify services manager on success
		servicesManager_->RegisterStream(handler_, stream);			
		
		// Send auth message.
		stream->Send(message_);		
		
		thread_ = NULL;
		
		return mc::_S_OK;
	}
}