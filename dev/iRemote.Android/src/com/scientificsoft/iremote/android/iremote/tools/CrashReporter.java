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

package com.scientificsoft.iremote.android.iremote.tools;

import java.io.OutputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.io.Writer;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.util.GregorianCalendar;
import java.util.Locale;
import java.util.Random;

import android.content.Context;
import android.provider.Settings.Secure;
import android.telephony.TelephonyManager;

import com.scientificsoft.iremote.android.iremote.AppVersions;
import com.scientificsoft.iremote.server.Versions;

/**
 * An idea of the tool was found here:
 * http://androidblogger.blogspot.com/2010/03/crash-reporter-for-android-slight.html
 */
public class CrashReporter implements Thread.UncaughtExceptionHandler {

	private static final String url_ = "http://www.scientific-soft.com/mobile/apps/iremote/services/crash.php";
	
	private static boolean set_ = false;

    private Thread.UncaughtExceptionHandler uncaughtExceptionHandlerPrev_ = Thread.getDefaultUncaughtExceptionHandler();
    
    private static String deviceId_            = null;
    private static String phoneType_           = null;
    private static String simCountryIso_       = null;
    private static String simOperator_         = null;
    private static String simOperatorName_     = null;
    private static String networkCountryIso_   = null;
    private static String networkOperator_     = null;
    private static String networkOperatorName_ = null;

    private boolean inHandler = false;
    
    static public void setup(Context context)
    {
        if ( !set_ ) {
            set_ = true;
            // Create our exception handler.
            Thread.setDefaultUncaughtExceptionHandler(new CrashReporter());
        }
    }
    
    static public void initialize(Context context) {
        // Grab the device ID
        try { 
        	deviceId_ = Secure.getString(context.getContentResolver(), Secure.ANDROID_ID); 
        } catch (Exception e) {}
        
        // Grab the carrier info.
        int networkType = -1;
        try {
            TelephonyManager telephonyManager = (TelephonyManager)context.getSystemService(Context.TELEPHONY_SERVICE);
            try { networkType          = telephonyManager.getPhoneType();           } catch (Exception e) {}
            try { simCountryIso_       = telephonyManager.getSimCountryIso();       } catch (Exception e) {}
            try { simOperator_         = telephonyManager.getSimOperator();         } catch (Exception e) {}
            try { simOperatorName_     = telephonyManager.getSimOperatorName();     } catch (Exception e) {}
            try { networkCountryIso_   = telephonyManager.getNetworkCountryIso();   } catch (Exception e) {}
            try { networkOperator_     = telephonyManager.getNetworkOperator();     } catch (Exception e) {}
            try { networkOperatorName_ = telephonyManager.getNetworkOperatorName(); } catch (Exception e) {}
        } catch (Exception e) {}
        
        // TelephonyManager.PHONE_TYPE_CDMA == 2, but CDMA wasn't added until API level 4
        phoneType_ = (TelephonyManager.PHONE_TYPE_NONE == networkType) ? "None"  : 
                     (TelephonyManager.PHONE_TYPE_GSM  == networkType) ? "GSM"   :
                     (2                                == networkType) ? "CDMA"  : null;
    }
    
    private CrashReporter() {
    }
    
    @Override
    public void uncaughtException(Thread thread, Throwable throwable) {
        try {
            if ( inHandler ) {
                return;
            }
            inHandler = true;

            // Send the report.
            sendReport(throwable);
            
            // Continue the chain of exception handlers.
            if ( null != uncaughtExceptionHandlerPrev_ ) {
                uncaughtExceptionHandlerPrev_.uncaughtException(thread, throwable);
            }
        } catch (Exception e) {}
        
        try {
            inHandler = false;
        } catch (Exception e) {}
    }
    
    static public void sendReport(Throwable throwable) {
        try {
            // Get the locale.
            Locale locale = Locale.getDefault();
            String language   = (null == locale) ? null : locale.getLanguage();
            String country    = (null == locale) ? null : locale.getCountry();
            String localeCode = (isEmpty(language) ? "??" : language) + "-" + (isEmpty(country) ? "??" : country);
            
            // Build the stack trace.
            final Writer stringWriter = new StringWriter();
            final PrintWriter printWriter = new PrintWriter(stringWriter);
            if (null != throwable)
            {
                throwable.printStackTrace(printWriter);
            }
            
            // Get the current timestamp.
            GregorianCalendar calendar = new GregorianCalendar();
            String name =
                String.format("%tF_%tH-%tM-%tS.%tL_", calendar, calendar, calendar, calendar, calendar) +
                AppVersions.iREMOTE_VER_MAJOR + "_" + 
                AppVersions.iREMOTE_VER_MINOR + "_" + 
                AppVersions.iREMOTE_VER_FIX + "_" + 
                AppVersions.iREMOTE_VER_SUFFIX + "_" +
                (Versions.DEBUG ? "_D_" : "_R_") +
                "Android" + "_" + 
                localeCode + "_" +
                safeStr(android.os.Build.BRAND) + "_" +
                safeStr(android.os.Build.MODEL) + "_" +
                safeStr(android.os.Build.VERSION.RELEASE) + "_" +
                safeStr(android.os.Build.VERSION.SDK) + "_" +
                safeStr(android.os.Build.VERSION.INCREMENTAL) + "_" +
                String.format("%8X", (new Random()).nextInt());
            
            // Build one giant string with all our debug info.
            String report =
                  "Report Name:       " + name +
                "\n" +
                "\nBuild Type:        " + "Android" +
                "\niRemote App:       " + AppVersions.formatVersion() +
                "\niRemote Platform:  " + Versions.iREMOTE_VER_MAJOR + "." + Versions.iREMOTE_VER_MINOR + "." + Versions.iREMOTE_VER_REV +                
                "\niRemote Debug:     " + Versions.DEBUG +
                "\n" +
                "\nTimestamp:         " + String.format("%tF %tH:%tM:%tS.%tL", calendar, calendar, calendar, calendar, calendar) +
                "\nDevice ID:         " + safeStr(deviceId_) +
                "\nLocale Name:       " + safeStr((null == locale) ? null : locale.getDisplayName()) +
                "\nLocale Code:       " + localeCode +
                "\nRelease Ver:       " + safeStr(android.os.Build.VERSION.RELEASE) +
                "\nSDK Ver:           " + safeStr(android.os.Build.VERSION.SDK) +
                "\nIncremental Ver:   " + safeStr(android.os.Build.VERSION.INCREMENTAL) +
                "\nBuild Brand:       " + safeStr(android.os.Build.BRAND) +
                "\nBuild Model:       " + safeStr(android.os.Build.MODEL) +
                "\nBuild Board:       " + safeStr(android.os.Build.BOARD) +
                "\nBuild Device:      " + safeStr(android.os.Build.DEVICE) +
                "\nBuild Display:     " + safeStr(android.os.Build.DISPLAY) +
                "\nBuild Fingerprint: " + safeStr(android.os.Build.FINGERPRINT) +
                "\nBuild Host:        " + safeStr(android.os.Build.HOST) +
                "\nBuild ID:          " + safeStr(android.os.Build.ID) +
                "\nBuild Product:     " + safeStr(android.os.Build.PRODUCT) +
                "\nBuild Tags:        " + safeStr(android.os.Build.TAGS) +
                "\nBuild Time:        " + android.os.Build.TIME +
                "\nBuild Type:        " + safeStr(android.os.Build.TYPE) +
                "\nBuild User:        " + safeStr(android.os.Build.USER) +
                "\n" +
                "\nPhone Type:        " + safeStr(phoneType_) +
                "\nSim Country:       " + safeStr(simCountryIso_) +
                "\nSim Operator:      " + safeStr(simOperator_) +
                "\nSim Operator Name: " + safeStr(simOperatorName_) +
                "\nNet Country:       " + safeStr(networkCountryIso_) +
                "\nNet Operator:      " + safeStr(networkOperator_) +
                "\nNet Operator Name: " + safeStr(networkOperatorName_) +
                "\n\n" +
                stringWriter.toString();
            
            // Close our stream writer.
            printWriter.close();
            stringWriter.close();
            
            // Create an HttpURLConnection.
            URL url = new URL(url_ + "?type=android&name=" + URLEncoder.encode(name, "UTF-8") + ".txt");
            HttpURLConnection httpURLConnection = (HttpURLConnection)url.openConnection();
            
            // Configure the connection.
            httpURLConnection.setConnectTimeout(30 * 1000);
            httpURLConnection.setAllowUserInteraction(false);
            httpURLConnection.setDoOutput(true);
            httpURLConnection.setRequestProperty("Content-Type", "text/plain; charset=utf-8");
            
            // Write the data.
            byte[] post = report.getBytes();
            OutputStream outputStream = httpURLConnection.getOutputStream();
            outputStream.write(post, 0, post.length);
            outputStream.flush();
            outputStream.close();
            
            // Issue the request and get the response code.
            /* int nCode = */httpURLConnection.getResponseCode();
            
            // Log details locally.
            // Log("SendReport - bytes: " + abPost.length + ", result: " + nCode);  
            
            // Close the connection.
            httpURLConnection.disconnect();
        } catch (Exception e) {
        	// Log exception, which is caused by crash reporter.
            // Log(e);
        }
    }
    
    public static String safeStr(String str) {
        return ((null == str) ? "" : str);
    }
    
    public static boolean isEmpty(String str) {
        return ((null == str) || (str.length() <= 0));
    }    
}
 