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

package com.scientificsoft.iremote.server.config;

import java.io.InputStream;
import java.io.OutputStream;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

import com.scientificsoft.iremote.platform.tools.File;
import com.scientificsoft.iremote.server.hosts.Host;
import com.scientificsoft.iremote.server.hosts.HostsManager;
import com.scientificsoft.iremote.server.hosts.NetInterface;

public class Config {
	
	public static final long connectionTimeout_ = 10000; 
	
	public static final long authResponseTimeout_ = 5000; 	
	
	public static final int adRefreshIntervalSec_ = 20;
	
	public static int acceptedEulaVersion_ = -1;
	
	public static boolean inversePanning_ = false;
	
	public static boolean shellWrap_ = true;
	
	public static boolean shellScroll_ = true;
	
	public static final int splashDelay_ = 2000;
	
	public static final int shellMaxItems_ = 10000;
	
	private static String configFilename_ = "config.xml";
	
	private static boolean loading_ = false;
		
	public static void load() {
		loading_ = true;
        try {
			// Load file.
			InputStream configDataStream = File.openFile(configFilename_);
			if ( null != configDataStream ) {
				// Parse file.
	            SAXParserFactory factory = SAXParserFactory.newInstance();
	            SAXParser parser = factory.newSAXParser();
	            ConfigParser handler = new ConfigParser();
	            parser.parse(configDataStream, handler);            
			}
        } catch (Exception e) {
        	// Failed to parse configuration file.
        } 
        loading_ = false;
	}	
	
	public static void save() {
		// Ensure that loading is not performed.
		if ( loading_ )
			return;
		try {		
			// Load file.
			OutputStream configDataStream = File.createFile(configFilename_);
			if ( null != configDataStream ) {
				// Dump to a file.
				configDataStream.write(ConfigWriter.dump().getBytes());
				configDataStream.close();
			}
		} catch (Exception e) {
			// Failed to write configuration file.		
		}
	}
	
	private static class ConfigParser extends DefaultHandler {
		
		/** Temporary variable to save host, which is currently being parsed. */
		private Host host_ = null;
		
	    public void startElement(String uri, String localName, String name, Attributes attributes) throws SAXException {
	        super.startElement(uri, localName, name, attributes);
			if ( localName.equalsIgnoreCase("host") ) {
				// Create new host record.
				host_ = new Host();
				String hasHostId = attributes.getValue("hashostid");
				if ( ( null != hasHostId ) && ( hasHostId.equalsIgnoreCase("1") ) ) {
					String hostId = attributes.getValue("hostid");
					host_.setHostId(hostId);
				}
				String hasPassword = attributes.getValue("haspassphrase");
				if ( ( null != hasPassword ) && ( hasPassword.equalsIgnoreCase("1") ) ) {
					String password = attributes.getValue("passphrase");
					host_.setMd5Password(password);
				}
				String lastAccess = attributes.getValue("lastAccess");
				host_.setLastAccessTime(lastAccess);
			} else if ( localName.equalsIgnoreCase("interface") ) {
				String ip = attributes.getValue("ip");			
				String port = attributes.getValue("port");						
				// Check fields.
				if ( null == ip || null == port || 0 == ip.length() || 0 == port.length() )
					return;
				// Add new interface record.
				NetInterface netInterface = new NetInterface(ip, port);
				host_.addInterface(netInterface);
			} else if ( localName.equalsIgnoreCase("param") ) {
				String pName = attributes.getValue("name");
				String pValue = attributes.getValue("value");
				if ( pName.equalsIgnoreCase("acceptedEula") ) {
					acceptedEulaVersion_ = Integer.parseInt(pValue);
				} else if ( pName.equalsIgnoreCase("inversePanning") ) {
					inversePanning_ = Boolean.parseBoolean(pValue);
				} else if ( pName.equalsIgnoreCase("shellWrap") ) {
					shellWrap_ = Boolean.parseBoolean(pValue);
				} else if ( pName.equalsIgnoreCase("shellScroll") ) {
					shellScroll_ = Boolean.parseBoolean(pValue);
				}
			} else if ( localName.equalsIgnoreCase("lastAccessed") ) {
				String ip = attributes.getValue("ip");			
				String port = attributes.getValue("port");						
				// Check fields.
				if ( null == ip || null == port || 0 == ip.length() || 0 == port.length() )
					return;
				// Set values.
				HostsManager.instance().setLastAccessed(ip, port);
			}			
	    }	
		
	    public void endElement(String uri, String localName, String name) throws SAXException {
	        super.endElement(uri, localName, name);

			if ( localName.equalsIgnoreCase("host") ) {
                if ( ( null == host_ ) || ( 0 == host_.interfacesCount() ) )
                    return;
				HostsManager.instance().addHost( host_ );
			}	        
	    }		
	}
	
	private static class ConfigWriter {
				
		public static String dumpInterface(NetInterface netInterface) {
			String interfaceTag 
				= "            <interface ip=\""
				+ netInterface.ip_
				+ "\" port=\""
				+ netInterface.port_
				+ "\"/>\n";
			return interfaceTag;
		}
		
		public static String dumpLastAccessed(NetInterface netInterface) {
			String interfaceTag 
				= "    <lastAccessed ip=\""
				+ netInterface.ip_
				+ "\" port=\""
				+ netInterface.port_
				+ "\"/>\n";
			return interfaceTag;
		}			

		public static String dumpHost(Host host) {
			String hasHostId = ( host.hasHostId() ) ? ( "1" ) : ( "0" );
			String password = host.password();
			String lastAccess = host.lastAccessTime();
			String hasPassword = ( null != password ) ? ( "1" ) : ( "0" );			
			password = ( null == password ) ? "" : password;			
			lastAccess = ( null == lastAccess ) ? "never" : lastAccess;

			String hostTag 
				= "        <host hashostid=\""
				+ hasHostId
				+ "\" hostid=\""
				+ host.hostId().toString()			
				+ "\" haspassphrase=\""
				+ hasPassword
				+ "\" passphrase=\""
				+ password			
				+ "\" lastAccess=\""
				+ lastAccess
				+ "\">\n";

			int interfacesCount = host.interfacesCount();
			for ( int i = 0 ; i < interfacesCount ; ++i ) {
				hostTag += dumpInterface( host.interfaceAtIndex(i) );
			}			
			
			hostTag
				+= "        </host>\n";			
			return hostTag;
		}

		public static String dumpHosts() {
			String hostsTag
				= "    <hosts>\n";			
			try {
				int hostsCount = HostsManager.instance().hostsCount();
				for ( int i = 0 ; i < hostsCount ; ++i ) {
					hostsTag += dumpHost( HostsManager.instance().host(i) );
				}
			} catch ( Exception e ) {				
			}			
			hostsTag
				+= "    </hosts>\n";
			return hostsTag; 
		}
		
		public static String dumpParameters() {
			String paramsTag
				= "    <params>\n"
				+ "        <param name=\"acceptedEula\" value=\"" + acceptedEulaVersion_ + "\"/>\n"		
				+ "        <param name=\"inversePanning\" value=\"" + ( inversePanning_ ? "true" : "false" ) + "\"/>\n"
				+ "        <param name=\"shellWrap\" value=\"" + ( shellWrap_ ? "true" : "false" ) + "\"/>\n"
				+ "        <param name=\"shellScroll\" value=\"" + ( shellScroll_ ? "true" : "false" ) + "\"/>\n"
				+ "    </params>\n";
			return paramsTag;			
		}

		public static String dump() {
			String config 
				= "<?xml version=\"1.0\" encoding=\"UTF-8\" ?> \n" 
				+ "<config version=\"1.0\"> \n"
				+ dumpLastAccessed(HostsManager.instance().lastAccessed())
				+ dumpHosts()
				+ dumpParameters()
				+ "</config> \n";
			return config;
		}
	}
}
