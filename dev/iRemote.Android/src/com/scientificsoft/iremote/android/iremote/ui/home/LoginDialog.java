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

package com.scientificsoft.iremote.android.iremote.ui.home;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;

import com.scientificsoft.iremote.android.iremote.R;
import com.scientificsoft.iremote.server.hosts.HostsManager;

public class LoginDialog {
	
	private static ILoginListener loginListener_ = null;
	
	private static EditText ipField_ = null;
	
	private static EditText portField_ = null;

	public static Dialog create(Context context, ILoginListener loginListener) {		
		loginListener_ = loginListener;		
		AlertDialog.Builder builder = new AlertDialog.Builder(context);
		
        // Load dialog content from resources.
		LayoutInflater factory = LayoutInflater.from(context);
        final View loginView = factory.inflate(R.layout.login_dialog, null);
        builder.setIcon(R.drawable.home_connect);
        builder.setView(loginView);
        builder.setTitle(R.string.login_dialog_title);
        
        ipField_ = (EditText)loginView.findViewById(R.id.host_edit);
        portField_ = (EditText)loginView.findViewById(R.id.port_edit);
        
        ipField_.setText(HostsManager.instance().lastAccessed().ip_);
        portField_.setText(HostsManager.instance().lastAccessed().port_);
        
        builder.setPositiveButton("Connect", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
            	// Read entered values.
            	String ip = ipField_.getText().toString();
            	String port = portField_.getText().toString();
            	// Initiate connection.
    	    	loginListener_.connect(ip, port);
            }
        });
        builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
            }
        });        
        
        return builder.create();
	}	
	
}
