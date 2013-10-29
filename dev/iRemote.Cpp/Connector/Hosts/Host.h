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

#ifndef HOST_H__REMOTEPC__INCLUDED_
#define HOST_H__REMOTEPC__INCLUDED_

namespace RemotePC 
{

	/**
	 * Contains information about single host. 
	 */
	class Host
	{
	public:

		Host();
		
		void SetHostId(const std::string& hostId);
		
		void SetHostId(mc::RefGuid hostId);
		
		bool HasHostId() const;
		
		mc::Guid GetHostId() const;
		
		/** 
		 * Accepts md5 password string.
		 */
		void SetMd5Password(const std::string& password);
		
		/** 
		 * Returns md5 form of a password.
		 */
		std::string GetPassword() const;
		
		bool HasPassword() const;		
        
        void ResetPassword();
		
		void AddInterface(const NetInterface& netInterface);
		
		size_t GetInterfacesCount() const;
		
		const NetInterface& GetInterface(size_t index) const;
		
		void RemoveInterface(size_t index);

	private:
		
		bool hasHostId_;
		
		mc::Guid hostId_;

		bool hasPassword_;

		/** User password is stored in md5. */		 
		std::string password_;
		
		typedef std::vector< NetInterface > NetInterfaces_;
		
		/** List of all host network interfaces. */
		NetInterfaces_ netInterfaces_;

	};
	
}

#endif // !HOST_H__REMOTEPC__INCLUDED_
