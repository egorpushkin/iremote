/**
 * DatapointHelper.java
 * Copyright (C) 2009 Char Software Inc., DBA Localytics
 * 
 *  This code is provided under the Localytics Modified BSD License.
 *  A copy of this license has been distributed in a file called LICENSE
 *  with this source code.  
 *  
 *  Please visit www.localytics.com for more information.
 */

package com.Localytics.android;

import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;

import android.content.Context;
import android.content.pm.PackageManager;
import android.net.wifi.WifiManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.provider.Settings.System;
/**
 * Provides a number of static functions to aid in the collection and formatting
 * of datapoints.
 * @author Localytics
 */
public final class DatapointHelper 
{
	// This class should never be instantiated
	private DatapointHelper() { }	
	
    private static final String LOG_PREFIX = "(DatapointHelper) ";
    
	// Each YAML entry either goes to the session controller, the event controller 
	// or the optin controller  
	public static final String CONTROLLER_SESSION = "- c: se\n";
	public static final String CONTROLLER_EVENT   = "- c: ev\n";
	public static final String CONTROLLER_OPT     = "- c: optin\n";
	
	// Each entry is either a create action, or an update action  
	public static final String ACTION_CREATE      = "  a: c\n";
    public static final String ACTION_UPDATE      = "  a: u\n";	
    public static final String ACTION_OPTIN       = "  a: optin\n";
    
	// The target object for the data being set up.
	public static final String OBJECT_SESSION_DP  = "  se:\n";
    public static final String OBJECT_EVENT_DP    = "  ev:\n";
    public static final String OBJECT_OPT         = "  optin:\n";

    // Events can have attributes
    public static final String EVENT_ATTRIBUTE    = "   attrs:\n";

    /*******************************
     * WEBSERVICE PARAMENTER NAMES *
     * *****************************
     */
    
    // Every object has a UUID
    public static final String PARAM_UUID			= "u";
    
    // The identifier for this application, generated by the user on the webservice
    public static final String PARAM_APP_UUID      	 = "au";
    
    // The version of this application, taken from the application's manifest.
    public static final String PARAM_APP_VERSION     = "av";
    
    // A session's UUID as previously created.
    public static final String PARAM_SESSION_UUID    = "su";
    
    // A hashed identifier unique to this device
    public static final String PARAM_DEVICE_UUID     = "du";
    
    // android, iphone, blackberry, windowsmobile
    public static final String PARAM_DEVICE_PLATFORM = "dp";
    
    // maker of this device (currently not supported by Android)
    public static final String PARAM_DEVICE_MAKE     = "dma";
    
    // model of the device
    public static final String PARAM_DEVICE_MODEL    = "dmo";
    
    // version of the OS on this device
    public static final String PARAM_OS_VERSION      = "dov";
    
    // country device is from (obtained by querying the SIM card)
    public static final String PARAM_DEVICE_COUNTRY  = "dc";
    
    // country the current locale is set to
    public static final String PARAM_LOCALE_COUNTRY  = "dlc";
    
    // country the language is set to
    public static final String PARAM_LOCALE_LANGUAGE = "dll";
    
    // Locale as a language_country string.  (Not collected because this info 
    //  is already provided by LOCALE_LANGUAGE and LOCALE_COUNTRY.
    public static final String PARAM_LOCALE          = "dl";
    
    // Country the user is currently in (comes from Sim card)
    public static final String PARAM_NETWORK_COUNTRY = "nc";
    
    // Current carrier (comes from sim card)
    public static final String PARAM_NETWORK_CARRIER = "nca";
    
    // Current mobile network code (comes from sim card)
    public static final String PARAM_NETWORK_MNC     = "mnc";
    
    // current mobile country code (comes from sim card)
    public static final String PARAM_NETWORK_MCC     = "mcc";
    
    // type of data connection (wifi, umts, gprs, evdo, ...)
    public static final String PARAM_DATA_CONNECTION = "dac";
    
    // the version of this Localytics client library
    public static final String PARAM_LIBRARY_VERSION = "lv";
    
    // The source where the location came from
    public static final String PARAM_LOCATION_SOURCE = "ls";
    
    // the latitude returned by the location provider
    public static final String PARAM_LOCATION_LAT    = "lat";
    
    // the longitude from the location provider
    public static final String PARAM_LOCATION_LNG    = "lng";
    
    // the current time on the user's device
    public static final String PARAM_CLIENT_TIME     = "ct";
    
    // sent at closing time, the current time on the users's device
    public static final String PARAM_CLIENT_CLOSED_TIME = "ctc";
    
    // The name an event that occured
    public static final String PARAM_EVENT_NAME      = "n";   
    
    // the optin value sent in if a user opts in or out. 
    public static final String PARAM_OPT_VALUE       = "optin";
    
    /**
     * Returns the given key/value pair as a YAML string.  This string is intended to be
     * used to define values for the first level of data in the YAML file.  This is
     * different from the datapoints which belong another level in. 
     * @param paramName The name of the parameter 
     * @param paramValue The value of the parameter
     * @param paramIndent The indent level of the parameter
     * @return a YAML string which can be dumped to the YAML file
     */
    public static String formatYAMLLine(String paramName,String paramValue, int paramIndent)
    {
        if (paramName.length() > LocalyticsSession.MAX_NAME_LENGTH)
        {
            Log.v(DatapointHelper.LOG_PREFIX, "Parameter name exceeds " 
                    + LocalyticsSession.MAX_NAME_LENGTH + " character limit.  Truncating.");
            paramName = paramName.substring(0, LocalyticsSession.MAX_NAME_LENGTH);
        }
        if (paramValue.length() > LocalyticsSession.MAX_NAME_LENGTH)
        {
            Log.v(DatapointHelper.LOG_PREFIX, "Parameter value exceeds " 
                    + LocalyticsSession.MAX_NAME_LENGTH + " character limit.  Truncating.");
            paramValue = paramValue.substring(0, LocalyticsSession.MAX_NAME_LENGTH);
        }
    	// The params are stored in the second tier of the YAML data.
    	// so with spacing, the expected result is: "    paramname:paramvalue\n"
        StringBuffer formattedString = new StringBuffer();
        for (int currentIndent = 0; currentIndent < paramIndent; currentIndent++)
        {
            formattedString.append(" ");
        }

        formattedString.append(escapeString(paramName));
        formattedString.append(": ");

        // Escape the string.
        formattedString.append(escapeString(paramValue));

        formattedString.append("\n");
        
        return formattedString.toString();
    }

    /**
     * Gets a 1-way hashed value of the device's unique ID.  This value is encoded using a SHA-256
     * one way hash and cannot be used to determine what device this data came from.
     * @param appContext The context used to access the settings resolver
     * @return An 1-way hashed identifier unique to this device or null if an ID, or the hashing
     * algorithm is not available. 
     */
    public static String getGlobalDeviceId(final Context appContext)
    {
    	String systemId = System.getString(appContext.getContentResolver(), System.ANDROID_ID);
    	if(systemId == null)
    	{    		
    		return null;
    	}
    	
    	try
    	{
    		MessageDigest md = MessageDigest.getInstance("SHA-256");
    		byte[] digest = md.digest(systemId.getBytes());
    		BigInteger hashedNumber = new BigInteger(1, digest);
    		return new String(hashedNumber.toString(16));
    		
    	}
    	catch(NoSuchAlgorithmException e)
    	{
    		return null;
    	}    
    }
    
    /**
     * Determines the type of network this device is connected to.
     * @param appContext the context used to access the device's WIFI
     * @param telephonyManager The manager used to access telephony info
     * @return The type of network, or unknown if the information is unavailable
     */
    public static String getNetworkType(
    									final Context appContext,
    									TelephonyManager telephonyManager) 
    {   
    	WifiManager wifiManager = (WifiManager)appContext.getSystemService(Context.WIFI_SERVICE);    	
    	
    	// this will only work for apps which already have wifi permissions.
    	try
    	{
    		if(wifiManager.isWifiEnabled())
    		{
    			return "wifi";
    		}
    	}    	    
    	catch (Exception e) { }      	
    	
    	switch (telephonyManager.getNetworkType())
        {    	
            case TelephonyManager.NETWORK_TYPE_EDGE : return "edge";
            case TelephonyManager.NETWORK_TYPE_GPRS : return "GPRS";
            case TelephonyManager.NETWORK_TYPE_UMTS : return "UMTS";
            case TelephonyManager.NETWORK_TYPE_UNKNOWN : return "unknown";            
        }
        
        return "none"; 
    }
    
    /**
     * Gets the pretty string for this application's version.
     * @param appContext The context used to examine packages
     * @return The application's version as a pretty string
     */
    public static String getAppVersion(final Context appContext)    		
    {
    	PackageManager pm = appContext.getPackageManager();
    	
    	try
    	{
    		return pm.getPackageInfo(appContext.getPackageName(), 0).versionName;
    	}
    	catch (PackageManager.NameNotFoundException	e)
    	{
    		return "unknown";
    	}
    }
    
    /**
     * Gets the current time, along with local timezone, formatted as a DateTime for the webservice. 
     * @return a DateTime of the current local time and timezone.
     */
    public static String getTimeAsDatetime()
    {
    	SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss-00:00");
    	sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
    	return sdf.format(new Date());
    }

    /***************************
     * Private Helper Functions *
     ***************************/

    /**
     * Escapes strings for YAML parser
     * @param rawString The string we want to escape.
     * @return An escaped string ready for YAML
     */
    private static String escapeString(String rawString)
    {
        if (rawString == null)
        {
            return "";
        }    	
    	
        StringBuffer parseString = new StringBuffer("\"");

        int startRead = 0;       // Index to start reading at
        int stopRead = 0;        // Index characters get read from and where the substring ends
        int bufferLength = rawString.length();

        // Every time we come across a " or \, append what we have so far, append a \,
        // then manage our indexes to continue where we left off.
        while (stopRead < bufferLength)
        {
            if (rawString.charAt(stopRead) == '\"' || rawString.charAt(stopRead) == '\\')
            {
                parseString.append(rawString.substring(startRead, stopRead));
                parseString.append('\\');
                startRead = stopRead;
            }
            // Skip null characters.
            else if (rawString.charAt(stopRead) == '\0')
            {
                parseString.append(rawString.substring(startRead, stopRead));
                startRead = stopRead + 1;
            }
            stopRead++;
        }
        // Append whatever is left after parsing
        parseString.append(rawString.substring(startRead, stopRead));
        // and finish with a closing "
        parseString.append('\"');
        return parseString.toString();
    }
}