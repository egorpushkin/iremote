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

#ifndef ISERVERCONTROL_H__REMOTEPC__INCLUDED_
#define ISERVERCONTROL_H__REMOTEPC__INCLUDED_

namespace RemotePC
{
    struct IServerControl : public mc::ICommon
	{
		virtual mc::result Stop() = 0;

		virtual mc::result HandleClient(size_t cookie, bool auth) = 0;

		virtual mc::result HandleClient(mc::IProtocolRef client, bool auth) = 0;

		virtual bool HasClients() = 0;

		virtual bool HasClient(size_t cookie) = 0;

		virtual Client GetClient(size_t cookie) = 0;

		virtual bool TestClient(const Client& client) = 0;

		virtual mc::result SpreadMessage(mc::IMessageRef message) = 0;

		virtual mc::result RequestPassword(size_t cookie, bool requestAuth) = 0;

		virtual mc::result DisconnectClient(size_t cookie) = 0;

		virtual void HandleAuthorized(IClientHandlerRef handler) = 0;

        virtual IServicesManagerPtr ServicesManager() = 0;
	};

	typedef mc::Strong< IServerControl > IServerControlPtr;
	typedef mc::Weak< IServerControl > IServerControlWeak;
	typedef const IServerControlPtr& IServerControlRef;
}

#endif // !ISERVERCONTROL_H__REMOTEPC__INCLUDED_
