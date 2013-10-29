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

#include "Config.h"
#include "Connector/Device.h"

// Add hosts manager support.
#include "../Hosts/Hosts.h"

#include <expat.h>
#include <fstream>
#include <sstream>

namespace RemotePC
{

	class ConfigParser
	{
	private:
		
		static ConfigParser& Instance()
		{
			return Loki::SingletonHolder< ConfigParser >::Instance();
		}
		
	public:

		static void Load()
		{
			XML_Parser parser = XML_ParserCreate(NULL);
			if ( !parser )
				return;
			XML_SetElementHandler(parser, ConfigParser::StartElementHandler, ConfigParser::EndElementHandler);

			FILE * configFile = fopen(Config::Instance().GetConfigLocation().c_str(), "r");
			if ( !configFile )
				return;

			bool done = false;
			const size_t bufferSize = 8192;
			char buffer[bufferSize];
			do 
			{
				size_t readBytes = fread(buffer, 1, bufferSize, configFile);
				if ( ferror(configFile) ) 
				{
					break;
				}
				done = ( 0 != feof(configFile) );

				if ( ! XML_Parse(parser, buffer, (int)readBytes, done) ) 
				{
					break;
				}
			} 
			while ( !done );

			XML_ParserFree(parser);
			fclose(configFile);
		}

		static void StartElementHandler(void* /* userData */, const XML_Char* name, const XML_Char** attr)
		{
			if ( std::string("host") == std::string(name, strlen(name)) )
			{
				bool hasHostId;				
				std::string hostId;				
				bool hasPassword;
				std::string password;				
				bool requestAuth;
				for ( size_t i = 0 ; attr[i] ; i += 2 )
				{
					std::string param( attr[i] );
					std::string value( attr[i + 1] );
					if ( std::string("hashostid") == param )
					{
						hasHostId = ( std::string(value) == "1" );
					}
					else if ( std::string("hostid") == param )
					{
						hostId = value;
					}
					else if ( std::string("haspassphrase") == param )
					{
						hasPassword = ( std::string(value) == "1" );
					}
					else if ( std::string("passphrase") == param )
					{
						password = value;
					}			
					else if ( std::string("auth") == param )
					{
						requestAuth = ( std::string(value) == "1" );
					}					
				}
				// Add new host record.
				ConfigParser::Instance().InitHost();
				if ( hasHostId )
					ConfigParser::Instance().GetHost().SetHostId(hostId);
				if ( hasPassword )
					ConfigParser::Instance().GetHost().SetMd5Password(password);
			}
			else if ( std::string("interface") == std::string(name, strlen(name)) )
			{
				std::string ip;				
				std::string port;				
				for ( size_t i = 0 ; attr[i] ; i += 2 )
				{
					std::string param( attr[i] );
					std::string value( attr[i + 1] );
					if ( std::string("ip") == param )
					{
						ip = value;
					}
					else if ( std::string("port") == param )
					{
						port = value;
					}
				}
				// Check fields.
				if ( 0 == ip.size() || 0 == port.size() )
					return;
				// Add new interface record.
				NetInterface netInterface;
				netInterface.SetIp(ip);
				netInterface.SetPort(port);
				ConfigParser::Instance().GetHost().AddInterface(netInterface);
			}
		}

		static void EndElementHandler(void* /* userData */, const XML_Char* name)
		{
			if ( std::string("host") == std::string(name, strlen(name)) )
			{
                Host& host = ConfigParser::Instance().GetHost();
                if ( 0 == host.GetInterfacesCount() )
                    return;
				HostsManager::Instance().AddHost( host );
			}
		}
		
		static std::string DumpInterface(const NetInterface& netInterface)
		{
			std::string interfaceTag 
				= std::string("			<interface ip=\"")
				+ netInterface.GetIp()
				+ std::string("\" port=\"")
				+ netInterface.GetPort()
				+ std::string("\"/>\n");
			return interfaceTag;
		}		

		/** 
		 * Passing non constant reference is caused by necessarity to invoke 
		 * Host::GetInterface method, which is not constant.
		 */ 
		static std::string DumpHost(const Host& host)
		{
			std::string hasHostId = ( host.HasHostId() ) ? ( std::string("1") ) : ( std::string("0") );
			std::string hasPassword = ( host.HasPassword() ) ? ( std::string("1") ) : ( std::string("0") );

			std::string hostTag 
				= std::string("		<host hashostid=\"")
				+ hasHostId
				+ std::string("\" hostid=\"")
				+ host.GetHostId().ToString()			
				+ std::string("\" haspassphrase=\"")
				+ hasPassword
				+ std::string("\" passphrase=\"")
				+ host.GetPassword()			
				+ std::string("\">\n");

			size_t interfacesCount = host.GetInterfacesCount();
			for ( size_t i = 0 ; i < interfacesCount ; ++i )
			{
				hostTag += DumpInterface( host.GetInterface(i) );
			}			
			
			hostTag
				+= std::string("		</host>\n");			
			return hostTag;
		}

		static std::string DumpHosts()
		{
			std::string hostsTag
				= std::string("	<hosts>\n");
			
			// Lock hosts manager for reading externally.
			mc::IReadWriteLock::Reader_ reader(HostsManager::Instance().GetLock());
			size_t hostsCount = HostsManager::Instance().GetHostsCount();
			for ( size_t i = 0 ; i < hostsCount ; ++i )
			{
				hostsTag += DumpHost( HostsManager::Instance().GetHost(i) );
			}
			
			hostsTag
				+= std::string("	</hosts>\n");
			return hostsTag; 
		}

		static void Save()
		{
			std::string config 
				= std::string("<?xml version=\"1.0\" encoding=\"UTF-8\" ?> \n") 
				+ std::string("<config version=\"1.0\"> \n")
				+ DumpHosts()
				+ std::string("</config> \n"); 

			std::ofstream stream(Config::Instance().GetConfigLocation().c_str());
			stream << config;
		}
		
	private:
		
		void InitHost()
		{
			currentHost_ = Host();
		}
		
		Host& GetHost()
		{
			return currentHost_;
		}
		
	private:
		
		/** Temporary variable, which is utilized during parsing config file. */ 
		Host currentHost_;
		
	};

	Config& Config::Instance()
	{
		return Loki::SingletonHolder< Config >::Instance();
	}
	
	Config::Config()
	{
	}

	void Config::Load()
	{
		ConfigParser::Load();
	}

	void Config::Save() const
	{
		ConfigParser::Save();
	}
	
	std::string Config::GetConfigLocation()
	{
		return MinCOM::iPhone::Device::GetDocuments() + std::string("/config.xml");
	}

}
