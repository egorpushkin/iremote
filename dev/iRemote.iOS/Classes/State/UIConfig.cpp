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

#include "UIConfig.h"
#include "Connector/Device.h"

#include <pugixml.hpp>
#include <fstream>
#include <sstream>

namespace RemotePC
{
    
    const float UIConfig::connectionTimeout_ = 20.0f;

    const float UIConfig::reconnectAfterWakeup_ = 20.0f;
    
    const float UIConfig::adRefreshInterval_ = 20.0f;
	
	const float UIConfig::hostCellHeight_ = 65.0f;
	
	const float UIConfig::noHostsCellHeight_ = 191.0f;
	
	const float UIConfig::hostsHeaderHeight_ = 50.0f;
	
	const float UIConfig::screenshotWidth_ = 256.0f;
	
	const float UIConfig::screenshotHeight_ = 256.0f;			
	
	float UIConfig::maxZoom_ = 1.0f;
	
	float UIConfig::minZoom_ = 1.0f;
	
	float UIConfig::currentZoom_ = 1.0f;

    // Public accessors
    ////////////////////////////////////////////////////////////////////////////////
    
    void UIConfig::SetPanningMode(bool inverse)
    {
        inversePanning_ = inverse;
        Save();
    }
    
    bool UIConfig::PanningMode()
    {
        return inversePanning_;
    }
    
    void UIConfig::EnableAutorotation(bool enable)
    {
        autorotation_ = enable;
        Save();
    }
    
    bool UIConfig::Autorotation()        
    {
        return autorotation_;
    }
	
    // Configuration file parser
    ////////////////////////////////////////////////////////////////////////////////
    
	class UIConfigParser
	{
	public:
        
		static void Load()
		{
            // Parse xml document into pugi DOM.
            pugi::xml_document doc;
            pugi::xml_parse_result result = doc.load_file(UIConfig::Instance().GetConfigLocation().c_str());
            if ( pugi::status_ok  != result.status )
                return;
            
            // Extract root element - config.
            pugi::xml_node config = doc.child("config");
            // Extract arguments child.
            pugi::xml_node arguments = config.child("arguments");
            
            // Walk through all arguments.
            for ( pugi::xml_node argument = arguments.first_child() ; argument ; argument = argument.next_sibling() )
            {
                std::string name = argument.attribute("name").value();            
                std::string value = argument.attribute("value").value();            
                if ( "inversePanning" == name )
                {
                    UIConfig::Instance().SetPanningMode( std::string(value) == "1" );
                } 
                else if ( "autorotation" == name )
                {
                    UIConfig::Instance().EnableAutorotation( std::string(value) == "1" );                
                }
            }            
		}
        
        static std::string DumpArguments()
        {
			std::string inversePanning = ( UIConfig::Instance().PanningMode() ) ? ( std::string("1") ) : ( std::string("0") );
            std::string autorotation = ( UIConfig::Instance().Autorotation() ) ? ( std::string("1") ) : ( std::string("0") );
            
            std::string arguments 
                = std::string("    <arguments> \n")
                + std::string("        <argument name=\"inversePanning\" value=\"") + inversePanning + std::string("\"/> \n")
                + std::string("        <argument name=\"autorotation\" value=\"") + autorotation + std::string("\"/> \n")
                + std::string("    </arguments> \n");
            return arguments;
        }
        
		static void Save()
		{
			std::string config 
                = std::string("<?xml version=\"1.0\" encoding=\"UTF-8\" ?> \n") 
                + std::string("<config version=\"1.0\"> \n")
                + DumpArguments()
                + std::string("</config> \n"); 
            
			std::ofstream stream(UIConfig::Instance().GetConfigLocation().c_str());
			stream << config;            
        }
		
	};    
    
    // Config life cycle
    ////////////////////////////////////////////////////////////////////////////////
    
	UIConfig& UIConfig::Instance()
	{
		return Loki::SingletonHolder< UIConfig >::Instance();
	}
	
	UIConfig::UIConfig()
        // Default values are set here.
        : inversePanning_(true)
        , autorotation_(true)
	{
	}
    
    // Serialization and instantiation
    ////////////////////////////////////////////////////////////////////////////////
    
	void UIConfig::Load()
	{
		UIConfigParser::Load();
	}
    
	void UIConfig::Save() const
	{
		UIConfigParser::Save();
	}
	
	std::string UIConfig::GetConfigLocation()
	{
		return MinCOM::iPhone::Device::GetDocuments() + std::string("/uiconfig.xml");
	}

}
