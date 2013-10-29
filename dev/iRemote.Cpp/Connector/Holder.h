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

#ifndef HOLDER_H__REMOTEPC__INCLUDED_
#define HOLDER_H__REMOTEPC__INCLUDED_

namespace RemotePC
{

	/**
	 * Note: C++ exception safety.
	 * Holder is mostly used from ourside of server component to notify 
	 * Remote PC on any user events. It (Holder) lies on the border between 
	 * C++-based server component and ObjectiveC-based iRemote UI. Thus, 
	 * it should not allow exceptions to leave its extent. This generally
	 * means that each method should be wrapped into try-catch block.
	 * Alternative solutions, which is much less effective is to perform 
	 * additional checks inside each method (serverControl_ for NULL, etc.).
	 *
	 * Holder cannot act accordingly, if exception is trown somewhere inside
	 * server component. So, it is not handled in most cases.
	 */
    class Holder
    {
	protected:
		
		/** Allows to protect class constructor. */
		friend class Loki::CreateUsingNew< Holder >;		
        
        Holder();

    public:        
		
        static Holder& Instance();
		
		void SetEvents(IUIEventsRef events);
		
		mc::IServicePtr GetService() const;
		
		IServicesManagerPtr ServicesManager();
        
        void Connect(const std::string& ip, const std::string& port);
        
        void Stop();
        
        bool IsConnected();
        
        /**
         * Perform shell command on host side.
         */
        void ExecuteShellCommand(const std::string& command);
        
        /**
         * Accepts raw password string.
         */
        void SubmitPassword(const std::string& password);        
        
        void OnMovedBy(float dx, float dy);
		
        void OnMovedTo(float x, float y);		
		
		void OnClicked(float x, float y);
        
        void OnScrolled(bool vertical, int direction);        
		
        void OnScrolled2(bool vertical, int direction);        		
        
        void OnLeftDown();
        
        void OnLeftUp();
        
        void OnMiddleDown();
        
        void OnMiddleUp();        
        
        void OnRightDown();
        
        void OnRightUp();        
        
        void OnVirtualKey(unsigned char key, bool keydown);
        
        void OnUnicodeKey(unsigned short key);
        
        void OnVolume(unsigned long value);
        
        void OnZoom(float level);
        
        IServerControlPtr GetServer();
        
        void SetCookie(const mc::Guid& cookie);
        
        void ReleaseCookie();  
        
        bool HasCookie();
        
        void LockCookie(bool lock);
		
        mc::result GetRemotePCVersion(int & major, int & minor);
        
	private:
		
		std::string CalculateMD5(const std::string& source);
        
    private:
		
		mc::IServicePtr service_;		
		
		IServicesManagerPtr servicesManager_;				
        
        mc::IThreadPtr serverThread_;
        
        RemotePC::IServerControlPtr serverControl_;

        RemotePC::IUIEventsPtr events_;
        
        bool hasCookie_;
        
        mc::Guid cookie_;
        
        bool cookieLock_;
		        
    };
    
}

#endif // !HOLDER_H__REMOTEPC__INCLUDED_

