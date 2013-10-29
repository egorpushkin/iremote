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

#ifndef PASSWORDREQUEST_H__MINCOM__INCLUDED_
#define PASSWORDREQUEST_H__MINCOM__INCLUDED_

namespace RemotePC 
{

	/**
	 * Password request message is sent from host and optionally informs the 
	 * device that password should be entered again even if it is saved. 
	 */ 
    class PasswordRequest
        : public mc::MessageImpl
    {
    public:

		typedef enum tagProperties_
		{
			PROP_HOST_ID = 1,
			PROP_REQUEST_AUTH
		} 
		Properties_;
        
		static const mc::Guid messageId_;
        
        PasswordRequest();
        
		PasswordRequest( mc::RefGuid hostId, bool requestAuth );

        // IMessage section
        virtual mc::IMessage::Size_ GetDataSize();
        
        // MessageImpl section
        virtual mc::result WriteBody(std::ostream& stream);
        
		virtual mc::result ReadBody(std::istream& stream);        
        
    };

}

#endif // PASSWORDREQUEST_H__MINCOM__INCLUDED_
