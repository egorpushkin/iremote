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

namespace RemotePC
{

	class ServerContext
		: public mc::CommonImpl< mc::IRunnable >
		, public mc::CommonImpl< IServerControl >
		, public mc::CommonImpl< mc::DProtocol >
	{
	public:

		typedef mc::CommonImpl< mc::IRunnable > ClassRoot_;

		ServerContext( IUIEventsRef events, const std::string& ip, const std::string& service, mc::IMessageRef authMessage );
        
        /**
         * This prevents application (2.2 build) crash on 3.0 devices when disconnecting from host 
         * (in Holder::Stop "serverControl_ = NULL;" ).
         */
        virtual ~ServerContext();
        
		// IRunnable section
		virtual mc::result Run();

		// IServerControl section
        virtual mc::result Send(mc::IMessageRef message);
        
		virtual mc::result Stop();
        
        virtual void SetUIReady();
        
        virtual mc::IMessagePtr GetAuthResponse() const;
		
		virtual void BlockMessage(const mc::Guid& messageId);
		
		virtual void UnblockMessage(const mc::Guid& messageId);	
		
		// DProtocol section
		virtual mc::result MessageArrived(mc::IProtocolRef protocol, mc::IMessageRef message);

		virtual mc::result DataErrorOccured(mc::IProtocolRef protocol);

		virtual mc::result Disconnected(mc::IProtocolRef protocol);

	private:
		
		void HandleAuthCookie(mc::IMessageRef message);
		
		void HandlePasswordRequest(mc::IMessageRef message);
		
		void HandleWrongPassword();
		
	private:
		
		void SetStopFlag();

		bool TestStopFlag();
		
		bool TestBlock(const mc::Guid& messageId);

	private:

		/** Network service utilized by acceptor. */
		mc::IServicePtr service_;

        /** Ip address to connect to. */
        std::string ip_;
        
        /** Host service name to use. */
        std::string port_;        
        
        /** Message which is sent to host when connected. */
        mc::IMessagePtr authMessage_;
        
        /** Connection to server. */ 
		mc::IConnectionPtr connection_;
                
		/** Interaction protocol wrapper. */
		mc::IProtocolPtr protocol_;
        
        /** Message which is received from host when connection is approved. */
        mc::IMessagePtr authResponse_;
        
		/** Prevents message block list corruption. */
		mc::IMutexPtr blockListGuard_;
		
		/** Set of UUIDs. */
		typedef std::set< mc::Guid > GuidSet_;
		
		/** Set of temporarily blocked messages. */
		GuidSet_ tempBlockedMessages_;			

		/** 
		 * Control interface to the tool which is responsible for the delivery of events
		 * to application UI.
		 */
		IUIEventsPtr events_;
        
        /** Indicates whether UI is initialized and ready to handle server events. */
        bool isUIReady_;
		
		/** Secures references to stop flag from different threads. */
		mc::IMutexPtr stopFlagGuard_;
		
		/** This flag is set to true, if service is stopped now (its somewere
		 * inside Stop method). This flag should be checked before the body of
		 * any other ServerContext blocking method to prevent dead locks. */		 
		bool stopFlag_;
		
		/** Screenshots rendering engine. */
		RenderingQueuePtr renderingQueue_;		
		
	};

}

#endif // !SERVERCONTEXT_H__REMOTEPC__INCLUDED_
