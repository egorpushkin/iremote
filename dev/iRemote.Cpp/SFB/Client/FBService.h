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

#ifndef FBSERVICE_H__SFB__REMOTEPC__INCLUDED_
#define FBSERVICE_H__SFB__REMOTEPC__INCLUDED_

namespace RemotePC
{
namespace SFB
{
	class FBService
		: public mc::CommonImpl< IServiceHandler >
		, public mc::CommonImpl< IFBService >	
	{
	public:
		
		typedef mc::CommonImpl< IServiceHandler > ClassRoot_;
		
		static const mc::Guid serviceId_;
		
		typedef enum tagProperties_
		{
			// PROP_SFB_SUPPORT = 0,
		}
		Properties_;
		
	public:
		
		FBService();

		/**
		 * IFBService section
		 */		
		
		virtual void setEvents(const IFBEventsPtr& events);
		
		virtual SurfacePtr surface();				
		
		/**
		 * IServiceHandler section
		 */
		
        virtual mc::Guid ServiceId();
		
        virtual void SetManager(IServicesManagerRef manager);				
		
		virtual void DeviceConnected();
		
        virtual void DeviceDisconnected();		
		
        virtual void StreamConnected(mc::IProtocolRef stream);
		
        virtual void StreamDisconnected(mc::IProtocolRef stream);
		
        virtual void ControlMessage(mc::IMessageRef message);
		
        virtual void StreamMessage(mc::IProtocolRef stream, mc::IMessageRef message);		
		
	private:
		
		/** Reference to parent services manager. */
		IServicesManagerWeak servicesManager_;
		
		/** Desktop screen surface model. */
		SurfacePtr surface_;
		
		/** Tiles rendering tool. */
		RenderingQueuePtr renderingQueue_;		
		
		/** Service events listener. */
		IFBEventsPtr events_;
		
		/** Contains size of each texture block. */
		size_t cellSize_;
		
	};
}
}

#endif // !FBSERVICE_H__SFB__REMOTEPC__INCLUDED_
