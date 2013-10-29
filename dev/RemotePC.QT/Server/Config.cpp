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
#include "expat.h"

// Add users manager support.
#include "UsersManager/User.h"
#include "UsersManager/DUsersManager.h"
#include "UsersManager/UsersManager.h"

#include <fstream>
#include <sstream>

namespace RemotePC
{

	Config::Config()
		: path_()
		, policy_( std::string("always_ask") )
		, service_( std::string("8444") )
		, screenshotSharing_( true )
		, fps_( 5.0f )
		, volumeUpdateInterval_( 10 * 1000 )
		, hasHostId_( false )
		, hostId_( mc::TypeInfo< mc::ICommon >::GetGuid() )
        , sfbEnabled_( true )
        , sfbFps_( 2.0f )
        , sfbPixelFormat_( SFB_TRUECOLOR )
        , sfbCompression_( 60 )
        , sfbCellSize_( 128 )
        , sfbChanged_( false )
	{
	}

	class ConfigParser
	{
	public:

		static void Load()
		{
			XML_Parser parser = XML_ParserCreate(NULL);
			if ( !parser )
				return;
			XML_SetElementHandler(parser, ConfigParser::StartElementHandler, ConfigParser::EndElementHandler);

			FILE * configFile = fopen(GetConfigLocation().c_str(), "r");
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
			if ( std::string("service") == std::string(name, strlen(name)) )
			{
				for ( size_t i = 0 ; attr[i] ; i += 2 ) 
				{
					std::string param( attr[i] );
					std::string value( attr[i + 1] );
					if ( std::string("name") == param )
					{
						Config::Instance().SetService( value );
					}
				}
			}
			else if ( std::string("screenshots") == std::string(name, strlen(name)) )
			{
				for ( size_t i = 0 ; attr[i] ; i += 2 )
				{
					std::string param( attr[i] );
					std::string value( attr[i + 1] );
					if ( std::string("mode") == param )
					{
						Config::Instance().SetScreenshotsSharing( value == std::string("share") );
					}
					else if ( std::string("fps") == param )
					{
						float fps;
						std::stringstream str;
						str << value;
						str >> fps;
						Config::Instance().SetFPS(fps);
					}
				}
			}
			else if ( std::string("security") == std::string(name, strlen(name)) )
			{
				for ( size_t i = 0 ; attr[i] ; i += 2 )
				{
					std::string param( attr[i] );
					std::string value( attr[i + 1] );
					if ( std::string("policy") == param )
					{
						Config::Instance().SetPolicy(value);
					}
				}
			}
			else if ( std::string("host") == std::string(name, strlen(name)) )
			{
				for ( size_t i = 0 ; attr[i] ; i += 2 )
				{
					std::string param( attr[i] );
					std::string value( attr[i + 1] );
					if ( std::string("id") == param )
					{
						Config::Instance().SetHostId(mc::Guid::FromString(value));
					}
				}
			}
			else if ( std::string("user") == std::string(name, strlen(name)) )
			{
				std::string deviceId;
				std::string deviceName;
				std::string ip;
				bool requireIp = true;
				std::string password;
				bool requestAuth = false;
				for ( size_t i = 0 ; attr[i] ; i += 2 )
				{
					std::string param( attr[i] );
					std::string value( attr[i + 1] );
					if ( std::string("deviceid") == param )
					{
						deviceId = value;
					}
					else if ( std::string("devicename") == param )
					{
						deviceName = value;
					}
					else if ( std::string("ip") == param )
					{
						ip = value;
					}
					else if ( std::string("requireip") == param )
					{
						requireIp = ( std::string(value) == "1" );
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
				// Check fields.
				if ( 0 == deviceId.size() || 0 == deviceName.size() || 0 == password.size() )
					return;
				// Add new user record.
				User user(deviceId, deviceName, ip, requestAuth);
				user.SetMd5Password(password);
				user.SetIpRequired(requireIp);
				UsersManager::Instance().AddUser(user);
			}
            else if ( std::string("sfb") == std::string(name, strlen(name)) )
            {
                for ( size_t i = 0 ; attr[i] ; i += 2 )
                {
                    std::string param( attr[i] );
                    std::string value( attr[i + 1] );
                    if ( std::string("enabled") == param )
                    {
                        bool enabled = ( std::string(value) == "1" );
                        Config::Instance().SetSFBEnabled(enabled);
                    }
                    else if ( std::string("fps") == param )
                    {
                        float fps;
                        std::stringstream str;
                        str << value;
                        str >> fps;
                        Config::Instance().SetSFBFPS(fps);
                    }
                    else if ( std::string("cellsize") == param )
                    {
                        size_t size;
                        std::stringstream str;
                        str << value;
                        str >> size;
                        Config::Instance().SetSFBCellSize(size);
                    }
                    else if ( std::string("pixel") == param )
                    {
                        int pixel;
                        std::stringstream str;
                        str << value;
                        str >> pixel;
                        Config::Instance().SetSFBPixelFormat((Config::SFBFormat_)pixel);
                    }
                    else if ( std::string("compr") == param )
                    {
                        int compr;
                        std::stringstream str;
                        str << value;
                        str >> compr;
                        Config::Instance().SetSFBCompression(compr);
                    }
                }
            }
		}

		static void EndElementHandler(void* /* userData */, const XML_Char* /* name */)
		{
		}

		static std::string DumpUser(const User& user)
		{
			std::string requireIp = ( user.IsIpRequired() ) ? ( std::string("1") ) : ( std::string("0") );
			std::string requestAuth = ( user.IsAuthRequested() ) ? ( std::string("1") ) : ( std::string("0") );
			std::string userRecord 
				= std::string("		<user deviceid=\"")
				+ user.GetDeviceId()
				+ std::string("\" devicename=\"")
				+ user.GetDeviceName()
				+ std::string("\" ip=\"")
				+ user.GetIp()
				+ std::string("\" requireip=\"")
				+ requireIp
				+ std::string("\" passphrase=\"")
				+ user.GetPassword()
				+ std::string("\" auth=\"")
				+ requestAuth
				+ std::string("\"/>\n");
			return userRecord;
		}

		static std::string DumpUsers()
		{
			std::string usersTag
				= std::string("	<users>\n");
			
			// Lock users manager for reading externally.
			mc::IReadWriteLock::Reader_ reader(UsersManager::Instance().GetLock());
			size_t usersCount = UsersManager::Instance().GetUsersCount();
			for ( size_t i = 0 ; i < usersCount ; ++i )
			{
				usersTag += DumpUser( UsersManager::Instance().GetUser(i) );
			}

			usersTag
				+= std::string("	</users>\n");
			return usersTag; 
		}

        static std::string DumpSFB()
        {
            std::string enabledStr = ( Config::Instance().IsSFBEnabled() ) ? ( std::string("1") ) : ( std::string("0") );
            std::string fpsStr;
            std::stringstream str;
            str << Config::Instance().GetSFBFPS();
            str >> fpsStr;
            std::string cellSizeStr;
            str.clear();
            str << Config::Instance().GetSFBCellSize();
            str >> cellSizeStr;
            std::string pixelStr;
            str.clear();
            str << Config::Instance().GetSFBPixelFormat();
            str >> pixelStr;
            std::string comprStr;
            str.clear();
            str << Config::Instance().GetSFBCompression();
            str >> comprStr;

            std::string sfbTag
                = std::string("	    <sfb enabled=\"")
                + enabledStr
                + std::string("\" fps=\"")
                + fpsStr
                + std::string("\" cellsize=\"")
                + cellSizeStr
                + std::string("\" pixel=\"")
                + pixelStr
                + std::string("\" compr=\"")
                + comprStr
                + std::string("\"/> \n");
            return sfbTag;
        }

		static void Save()
		{
			std::string fpsString;
			std::stringstream str;
			str << Config::Instance().GetFPS();
			str >> fpsString;

			std::string config 
				= std::string("<?xml version=\"1.0\" encoding=\"UTF-8\" ?> \n") 
				+ std::string("<config version=\"1.0\"> \n")
				+ std::string("	<service name=\"") 
					+ Config::Instance().GetService()
					+ std::string("\"/> \n") 
				+ std::string("	<screenshots mode=\"") +
					+ ( ( Config::Instance().ShareScreenshots() ) ? ( "share" ) : ( "lock" ) )
					+ std::string("\" fps=\"")
					+ fpsString
					+ std::string("\"/> \n")
				+ std::string("	<host id=\"")
					+ Config::Instance().GetHostId().ToString()
					+ std::string("\"/> \n")
                + DumpUsers()
                + DumpSFB()
                + std::string("</config> \n");

			std::ofstream stream(GetConfigLocation().c_str());
			stream << config;
		}

		static std::string GetConfigLocation()
		{
			return
				Config::Instance().GetPath() +
				std::string("/config.xml");
		}
	};

	Config& Config::Instance()
	{
		return Loki::SingletonHolder< Config >::Instance();
	}

	void Config::Load(const std::string& path)
	{
		// Remember configuration path.
		path_ = path;
		// Load configuration.
		ConfigParser::Load();
		// Generate host id on first launch.
		if ( !hasHostId_ )
		{
			// Generate guid.
			hostId_ = mc::Guid::Generate();
			hasHostId_ = true;
		}
	}

	void Config::Save() const
	{
		ConfigParser::Save();
	}

	std::string Config::GetPath() const
	{
		return path_;
	}

	std::string Config::GetPolicy() const
	{
		return policy_;
	}

	void Config::SetPolicy(const std::string& policy)
	{
		policy_ = policy;
	}

	std::string Config::GetService() const
	{
		return service_;
	}

	void Config::SetService(const std::string& service)
	{
		// Remember new value for this session.
		service_ = service;
	}

	bool Config::ShareScreenshots() const
	{
		return screenshotSharing_;
	}

	void Config::SetScreenshotsSharing(bool share)
	{
		screenshotSharing_ = share;
	}

	float Config::GetFPS() const
	{
		return fps_;
	}

	void Config::SetFPS(float fps)
	{
		fps_ = fps;
	}

	int Config::GetVolumeUpdateInterval() const
	{
		return volumeUpdateInterval_;
	}

    mc::Guid Config::GetHostId() const
	{
		return hostId_;
	}

	void Config::SetHostId(mc::RefClsid hostId)
	{
		hasHostId_ = true;
		hostId_ = hostId;
	}

    bool Config::IsSFBEnabled() const
    {
        return sfbEnabled_;
    }

    void Config::SetSFBEnabled(bool enabled)
    {
        sfbEnabled_ = enabled;
    }

    float Config::GetSFBFPS() const
    {
        return sfbFps_;
    }

    void Config::SetSFBFPS(float fps)
    {
        sfbFps_ = fps;
    }

    Config::SFBFormat_ Config::GetSFBPixelFormat() const
    {
        return sfbPixelFormat_;
    }

    void Config::SetSFBPixelFormat(SFBFormat_ format)
    {
        sfbPixelFormat_ = format;
    }

    int Config::GetSFBCompression() const
    {
        return sfbCompression_;
    }

    void Config::SetSFBCompression(int compression)
    {
        sfbCompression_ = compression;
    }

    size_t Config::GetSFBCellSize() const
    {
        return sfbCellSize_;
    }

    void Config::SetSFBCellSize(size_t size)
    {
        sfbCellSize_ = size;
    }

    void Config::SetSFBChanged(bool changed)
    {
        sfbChanged_ = changed;
    }

    bool Config::IsSFBChanged() const
    {
        return sfbChanged_;
    }

    void Config::FlushFlags()
    {
        sfbChanged_ = false;
    }
}
