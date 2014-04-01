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

import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdView;
import com.scientificsoft.iremote.server.DSession;
import com.scientificsoft.iremote.server.Features;
import com.scientificsoft.iremote.server.Holder;
import com.scientificsoft.iremote.server.Messenger;
import com.scientificsoft.iremote.server.hosts.HostsManager;
import com.scientificsoft.iremote.server.protocol.Guid;
import com.scientificsoft.iremote.android.iremote.R;
import com.scientificsoft.iremote.android.iremote.AppVersions;
import com.scientificsoft.iremote.android.iremote.tools.ContextHolder;
import com.scientificsoft.iremote.android.iremote.tools.activitymanager.IAppController;
import com.scientificsoft.iremote.android.iremote.tools.activitymanager.ManagedActivity;
import com.scientificsoft.iremote.android.iremote.ui.control.ControlActivity;

import android.app.Dialog;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

public class HomeActivity extends ManagedActivity implements IAppController, DSession {
	
	public static final int GROUP_HOME = 0x1;
	public static final int GROUP_CONTROL = 0x2;
    
	private static final int DIALOG_LOGIN = 0x1;	
	private static final int DIALOG_PASSWORD = 0x2;
	private static final int DIALOG_PASSWORD_WRONG = 0x4;
	private static final int DIALOG_HOSTS = 0x5;
	
	private static final int REQUEST_CODE_LICENSE = 0x1;
	
	private AdView ad_;
	
	private View copyright_;
	
	/** Indicates whether splash screen has already been presented during
	 * the life time of the process. */
	private static boolean splashShown_ = false;
	
	/** Business logic */	
	
	protected void askToAcceptLicense() {
		// Check whether EULA should be accepted.
		if ( !Features.eulaAccepted() ) {			
			startActivityForResult(new Intent(this, LicenseActivity.class), REQUEST_CODE_LICENSE);
		} else {
			showSplashScreen();
		}
	}
	
	protected void showSplashScreen() {
		if ( !splashShown_ ) {
			splashShown_ = true;
			startActivity(new Intent(this, SplashActivity.class));
		}
	}
	
	/** Application life cycle */	
	
	public int getActivityGroupId() {
		return GROUP_HOME;
	}
	
	public void sendToBack() {
		if ( Holder.STATUS_CONNECTED == Holder.instance().status() ) {
			Holder.instance().session().lockCookie(true);
			Holder.instance().disconnect();
		}
	}
	
	public void bringToFront() {
		Guid cookie = Holder.instance().session().cookie();
		if ( null != cookie ) {
			// Attempt to reconnect to last used host.
			Holder.instance().connect(
				HostsManager.instance().lastAccessed().ip_,
				HostsManager.instance().lastAccessed().port_);
		}
	}
	
	/** Activity life cycle */	
	
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState); 
		try {
	        // Display initial application UI.
	        setContentView(R.layout.home_activity);	        
	        updateVersion();
	        updateOrientation();
	        initUI();	        
	        
	        // Check whether EULA should be accepted and present corresponding UI.
	        askToAcceptLicense();        	
	        // Become application status listener.
	        ContextHolder.instance().activityManager().setAppController(this);	        
	        // Initialize localytics.
	        ContextHolder.instance().localyticsStart();
        } catch ( Exception e ) {
        }
    }
    
    protected void updateVersion() {
    	copyright_ = findViewById(R.id.home_copyright);
    	TextView version = (TextView)findViewById(R.id.home_version);
    	version.setText(version.getText() + AppVersions.formatVersion());
    }
    
    /** Called when the activity is going into the background. */    
    @Override    
    public void onPause() {
        super.onPause();
        // Pause localytics section.
        ContextHolder.instance().localyticsPause();
        // Pause ads.
    	if ( null != ad_ ) {
    		ad_.pause();
    	}
    }    
    
    /** Called when the activity is restored on the screen. */
    @Override    
    public void onResume() {
        super.onResume();
        // Subscribe on session events.
        Holder.instance().configureSession(this);
        // Check status. 
        if ( Holder.STATUS_DISCONNECTED == Holder.instance().status() ) {
        	int reason = Holder.instance().reason();
        	disconnected(reason);
        }
        // This should hide ads in landscape.
        updateOrientation();
        // Resume localytics section.
        ContextHolder.instance().localyticsResume();
        // Resume ads.
    	if ( null != ad_ ) {
    		ad_.resume();
    	}        
    }
    
    /** Called when the activity is being destroyed. */
    @Override
    public void onDestroy() {
    	if ( null != ad_ ) {
    		ad_.destroy();
    	}
    	super.onDestroy();
    }
    
    /** Called when device orientation changes. */    
    @Override    
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		updateOrientation();
	}    
        
    /** Called once(!), the first time the options menu is displayed. */
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
    	MenuInflater inflater = getMenuInflater();
    	inflater.inflate(R.menu.main, menu);
    	return true;
    }
    
    /** Handle "catch" action. */
	private View.OnClickListener catchListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	// Show login dialog.
	    	showDialog(DIALOG_LOGIN);
	    }
    };
    
    /** Handle "hosts" action. */
	private View.OnClickListener hostsListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	// Show the list of hosts.
			showDialog(DIALOG_HOSTS);
	    }
    };
    
    /** Handle "help" action. */
	private View.OnClickListener helpListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	// Show help.
	    	startActivity(new Intent(HomeActivity.this, HelpActivity.class));
	    }
    };
    
    /** Handle "about" action. */
	private View.OnClickListener aboutListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	// Show about.
	    	startActivity(new Intent(HomeActivity.this, AboutActivity.class));
	    }
    };
    
    /** Called when child activity exits. */
    @Override
    protected void onActivityResult (int requestCode, int resultCode, Intent data) {
    	super.onActivityResult(requestCode, resultCode, data);
    	if ( REQUEST_CODE_LICENSE == requestCode ) {
    		if ( RESULT_OK == resultCode ) {
    			Features.acceptEula();
    			showSplashScreen();
    		} else {
    			this.finish();
    		}
    	}
    }
    
    /** Called when options item is selected. */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
    	int itemId = item.getItemId();
    	switch ( itemId ) {
    	case R.id.menu_quit:
    		// Request splash screen on next launch.
    		splashShown_ = false;
    		this.finish();
    		break;
    	}    	
    	return true;
    }
    
    /** Called when any dialog is requested. */ 
    @Override
    protected Dialog onCreateDialog(int id) {
        switch (id) {
        case DIALOG_LOGIN:        	
        	return LoginDialog.create(this, new LoginListener());
        case DIALOG_PASSWORD:        	
        	return PasswordDialog.create(this, false, new PasswordListener());
        case DIALOG_PASSWORD_WRONG:
        	return PasswordDialog.create(this, true, new PasswordListener());
        case DIALOG_HOSTS:
        	return new HostsDialog(this, new LoginListener());
        }
        return null;
    }
    
    private class LoginListener implements ILoginListener {
    	/** Called when user attempts to connect to remote host. */
	    public void connect(String host, String port) {
	    	// Establish connection.
	      	Holder.instance().connect(host, port);
	    	// Save last accessed host, no matter what the result was.
	    	HostsManager.instance().setLastAccessed(host, port);	      	
	    }
    }
    
    private class PasswordListener implements IPasswordListener {
    	public void handle(String password) {
    		showStatusProgress("Approving connection...", "Verifying your password.");
    		Messenger.sendPassword(password);
    	}
     
    	public void cancel() {
    		// Cancel connection.
    		Holder.instance().disconnect();
    	}    	
    }
    
    /** DSession section. */
    
    public void connecting() {
    	deactivateControls();
    	showStatusProgress("Connecting to host...", "Establishing TCP connection.");
    }
    
	public void connected() {		
		showStatusProgress("Approving connection...", "Press 'Allow' button on PC to proceed.");
		ContextHolder.instance().localyticsTagConnected();
	}
	
	public void passwordRequested(boolean wrong) {	
		showDialog(wrong ? DIALOG_PASSWORD_WRONG : DIALOG_PASSWORD);		
	}
	
	public void approved() {
		showStatusMessage("Connection established.", "Ready to control your desktop.");
		startActivity(new Intent(this, ControlActivity.class));
	}
	
	public void disconnected(int reason) {
		updateDisconnectionReason(reason);
		activateControls();
	}
	
	/** UI tools */
	
	private void initUI() {
		configureButton(findViewById(R.id.button_connect), catchListener_);
		configureButton(findViewById(R.id.button_hosts), hostsListener_);
		configureButton(findViewById(R.id.button_help), helpListener_);
		configureButton(findViewById(R.id.button_about), aboutListener_);
		
        ad_ = (AdView)findViewById(R.id.ad);
        AdRequest adRequest = new AdRequest.Builder().build();
        ad_.loadAd(adRequest);        
	}
	
	private void configureButton(View view, View.OnClickListener listener) {
		view.getBackground().setAlpha(100);
		view.setOnClickListener(listener);
	}
	
    private void updateOrientation() {
    	boolean landscape = ( Configuration.ORIENTATION_LANDSCAPE == getResources().getConfiguration().orientation );
		if ( null != ad_ )
			ad_.setVisibility(landscape ? View.GONE : View.VISIBLE);
		copyright_.setVisibility(landscape ? View.GONE : View.VISIBLE);		
    }
	
    private void updateDisconnectionReason(int reason) {
    	if ( DSession.ERROR_OK == reason ) {  
    		showStatusMessage("Disconnected successfully.", "Ready to initiate next session.");        		
    	} else if ( DSession.ERROR_PROTOCOL == reason ) {
    		showStatusError("Protocol error.");        		
    	} else if ( DSession.ERROR_TERMINATED_BY_HOST == reason ) {
    		showStatusMessage("Terminated by host.", "Ready to initiate next session.");
    	} else if ( DSession.ERROR_TERMINATED_BY_USER == reason ) {
    		showStatusMessage("Terminated by user.", "Ready to initiate next session.");
    	} else if ( DSession.ERROR_WRONG_DEVICE_IP == reason ) {
    		showStatusError("Wrong device IP address.");
    	} else if ( DSession.ERROR_LIMIT_REACHED == reason ) {
    		showStatusError("Limit of connections is reached.");
    	} else if ( DSession.ERROR_OLD_REMOTE_PC == reason ) {
    		showStatusError("Please, download new version of Remote PC Suite. See Help section for URL.");    		
    	} else if ( DSession.ERROR_TIMEOUT == reason ) {
    		showStatusError("Connection timeout.");    		
    	} else if ( DSession.ERROR_FAILED_TO_CONNECT == reason ) {
    		showStatusError("Failed to connect to host.");   
    	} else if ( DSession.ERROR_UNKNOWN == reason ) {
    		showStatusError("Unknown error.");
    	}    	
    }		
	
	private void hideAllStatuses() {
		ViewGroup areaGroup = (ViewGroup)this.findViewById(R.id.home_status_area);
		int statusesCount = areaGroup.getChildCount();
		for ( int i = 0 ; i < statusesCount ; ++i ) {
			View statusView = areaGroup.getChildAt(i);
			statusView.setVisibility(View.GONE);
		}
	}
	
	private void showStatusInternal(int id, String primary, String secondary) {
		hideAllStatuses();
		View statusView = this.findViewById(id);
		statusView.setVisibility(View.VISIBLE);
		if ( null != primary ) {
			TextView primaryText = (TextView)statusView.findViewById(R.id.home_primary_status);
			primaryText.setText(primary);
		}
		if ( null != secondary ) {
			TextView secondaryText = (TextView)statusView.findViewById(R.id.home_secondary_status);
			secondaryText.setText(secondary);
		}
	}
	
	private void showStatusProgress(String primary, String secondary) {
		showStatusInternal(R.id.home_status_progress, primary, secondary);
	}
	
	private void showStatusMessage(String primary, String secondary) {
		showStatusInternal(R.id.home_status_message, primary, secondary);
	}	

	private void showStatusError(String error) {
		showStatusInternal(R.id.home_status_error, error, null);
	}	
	
	private void activateControls() {
		enableView(findViewById(R.id.home_content), true);
	}

	private void deactivateControls() {
		enableView(findViewById(R.id.home_content), false);
	}
	
	private void enableView(View view, boolean enabled) {
	    view.setClickable(enabled);
	    view.setEnabled(enabled);
	    if ( view instanceof ViewGroup ) {
	        ViewGroup group = (ViewGroup)view;
	        for ( int idx = 0 ; idx < group.getChildCount() ; idx++ ) {
	        	enableView(group.getChildAt(idx), enabled);
	        }
	    }
	}
}
