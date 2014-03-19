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

#ifndef SERVERCONTEXT_H__REMOTEPC__INCLUDED_
#define SERVERCONTEXT_H__REMOTEPC__INCLUDED_

#include "Client.h"
#include "IClientHandler.h"
#include "Services/Services.h"
#include "IServerControl.h"
#include "IUIEvents.h"
#include "Shell/IShellControl.h"

namespace RemotePC
{

	class ServerContext
		: public mc::CommonImpl< mc::IRunnable >
		, public mc::CommonImpl< IServerControl >
		, public mc::CommonImpl< mc::DServer >
		, public mc::CommonImpl< mc::DProtocol >
	{
	public:

		typedef mc::CommonImpl< mc::IRunnable > ClassRoot_;

        ServerContext( IUIEventsRef events );
		virtual ~ServerContext();

        // ICommon section
        virtual mc::result PostInit();

		// IRunnable section
		virtual mc::result Run();

		// IServerControl section
		virtual mc::result Stop();

		virtual mc::result HandleClient(size_t cookie, bool auth);

		virtual mc::result HandleClient(mc::IProtocolRef client, bool auth);

		virtual bool HasClients();

		virtual bool HasClient(size_t cookie);

		virtual Client GetClient(size_t cookie);

		virtual bool TestClient(const Client& client);

		virtual mc::result SpreadMessage(mc::IMessageRef message);

		virtual mc::result RequestPassword(size_t cookie, bool requestAuth);

		virtual mc::result DisconnectClient(size_t cookie);

		virtual void HandleAuthorized(IClientHandlerRef handler);

        virtual IServicesManagerPtr ServicesManager();

		// DServer section
		virtual mc::result ConnectionAccepted(mc::IConnectionRef connection);

		// DProtocol section
		virtual mc::result MessageArrived(mc::IProtocolRef protocol, mc::IMessageRef message);

		virtual mc::result DataErrorOccured(mc::IProtocolRef protocol);

		virtual mc::result Disconnected(mc::IProtocolRef protocol);

	protected:

		void SendVolumeLevel(mc::IProtocolRef client);

		mc::IProtocolPtr ClientByCookie(size_t cookie);
		void UnregisterClient(mc::IProtocolRef protocol);

        mc::result HandleStream(mc::IProtocolRef protocol, mc::IMessageRef message);
		mc::result HandleAuthMessage(mc::IProtocolRef protocol, mc::IMessageRef message);
        mc::result HandleMouseClickMessage(mc::IMessageRef message);
        mc::result HandleMoveToMessage(mc::IProtocolRef protocol, mc::IMessageRef message);
        mc::result HandleMoveByMessage(mc::IProtocolRef protocol, mc::IMessageRef message);
		mc::result HandleScrollMessage(mc::IMessageRef message);
        mc::result HandleScrollMessage2(mc::IMessageRef message);
		mc::result HandleKeyboardMessage(mc::IMessageRef message);
		mc::result HandleVolumeMessage(mc::IMessageRef message);
		mc::result HandleCookieMessage(mc::IMessageRef message);
		mc::result HandleShellRequest(mc::IMessageRef message);
		mc::result HandlePassword(mc::IProtocolRef protocol, mc::IMessageRef message);
		mc::result HandleZoomLevel(mc::IProtocolRef protocol, mc::IMessageRef message);
		mc::IMessagePtr PrepareAuthMessage(int clientMajor, int clientMinor);
		mc::result RequestPassword(mc::IProtocolRef protocol, bool requestAuth);
		mc::result WrongPassword(mc::IProtocolRef protocol);

	private:

		/** Network service utilized by acceptor. */
		mc::IServicePtr service_;

		/**
		 * Each value of this associative container represents client
		 * record with complete set of required fields.
		 * See Client for more details.
		 */ 
		typedef std::map< mc::IProtocolPtr, Client > Clients_;

		/** 
		 * Contains the list of current connections. 
		 */ 
		Clients_ clients_;

		/**
		 * Rationale of using JobsQueue instead of simple pool of threads:
		 * This tool combines two conditions which are important for auth tasks:
		 * 1. Jobs are performed in separate thread.
		 * 2. Jobs cannot be performed simultaneously. So user will never see 
		 * two or more authentication dialogs at the same time.
		 */ 
		mc::IJobsQueuePtr authJobs_;

		/**
		 * List of currently available cookies.
		 */ 
		std::set< mc::Guid > listOfCookies_;

		/** 
		 * Control interface to tool responsible for the delivery of events
		 * to application UI.
		 */
		IUIEventsRef events_;

        /**
         * Streaming services manager.
         */
        IServicesManagerPtr servicesManager_;
	};
}

#endif // !SERVERCONTEXT_H__REMOTEPC__INCLUDED_
