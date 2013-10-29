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

package com.scientificsoft.iremote.android.iremote.ui.control;

import java.util.Vector;

import com.scientificsoft.iremote.android.iremote.R;
import com.scientificsoft.iremote.server.DUIHost;
import com.scientificsoft.iremote.server.Messenger;
import com.scientificsoft.iremote.server.config.Config;
import com.scientificsoft.iremote.server.messages.ScreenshotMessage;
import com.scientificsoft.iremote.server.messages.ShellResponse;
import com.scientificsoft.iremote.server.messages.VolumeMessage;

import android.content.Context;
import android.text.TextUtils;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;

public class ShellTab extends LinearLayout implements DUIHost, ISettingsListener {
	
	/** Shell prompt. */
	private EditText shellPrompt_ = null;	
	
	/** Shell send. */
	private Button shellSend_ = null;	
	
	/** Shell output. */
	private ShellVector shellContent_ = null;	
	private ArrayAdapter<String> adapterWrap_ = null;	
	private ArrayAdapter<String> adapterNoWrap_ = null;	
	private ListView shellOutput_ = null;
	
	/** Internal constants. */
	private static final int shellOverflowThreshold_ = 100;
	
	/** DHost section. */

	public void screenshot(ScreenshotMessage message) {
    }
    
    public void shellRespose(ShellResponse message) {    	
    	try {
    		String response = message.getProperty(ShellResponse.PROP_RESPONSE).toString();
    		add(response);
	  	} catch ( Exception e ) {    		
	    }    	
    }	
    
    public void volumeLevel(VolumeMessage message) {    	
    }
    
	/** ISettingsListener section. */ 
	
	public void settingsChanged() {
		// Update scrolling mode.
        shellOutput_.setTranscriptMode(
        	Config.shellScroll_ 
        		? ListView.TRANSCRIPT_MODE_ALWAYS_SCROLL 
        		: ListView.TRANSCRIPT_MODE_NORMAL);
        // Update wrap mode.
        shellOutput_.setAdapter(getAdapter());
	}
	
	/** Event handlers. */
    
    @Override
    public boolean onKeyUp(int keyCode, KeyEvent event) {
    	if ( KeyEvent.KEYCODE_ENTER == keyCode ) {
    		// Send command.
    		sendListener_.onClick(shellSend_);
    	}    	
        return super.onKeyUp(keyCode, event);
    }        
	
	private View.OnClickListener sendListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	try { 
		    	String shellRequest = shellPrompt_.getText().toString();
		    	if ( shellRequest.length() > 0 ) {
			    	Messenger.sendShellRequest(shellRequest);
			    	shellPrompt_.setText("");
		    	}
	    	} catch ( Exception e ) {    
	    	}
	    }
    };
    
	private View.OnClickListener clearListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	try { 
	    		clear();
	    	} catch ( Exception e ) {    
	    	}
	    }
    };
    
	private View.OnClickListener ctrlcListener_ = new View.OnClickListener() {
	    public void onClick(View v) {
	    	try {
	    		Messenger.sendShellRequest("ctrl-c");
	    	} catch ( Exception e ) {    
	    	}
	    }
    };
    
    /** Shell output content management. */    
    
	public void add(String message) {
		String[] messages = TextUtils.split(message, "\n");
		ArrayAdapter<String> adapter = getAdapter();
		for ( int i = 0; i < messages.length ; ++i ) {
			adapter.add(messages[i]);
		}
		if ( shellContent_.size() > ( Config.shellMaxItems_ + shellOverflowThreshold_ ) ) {
			shellContent_.trim();
			adapter.notifyDataSetChanged();
		}			
	}
	
	public void clear() {
		getAdapter().clear();
	}    
    
    /** Life cycle and UI initialization. */

	public ShellTab(Context context) {
		super(context);
		configureUI(context);
	}
	
	public ShellTab(Context context, AttributeSet attrs) {
		super(context, attrs);
		configureUI(context);
	}
	
	private void configureUI(Context context) {
		// Load shell tab view.
		LayoutInflater factory = LayoutInflater.from(context);
        final View tabView = factory.inflate(R.layout.tab_shell, null);
        this.addView(tabView);
        // Remember controls.
        shellPrompt_ = (EditText)findViewById(R.id.shell_prompt);
        // Configure buttons.
        Button clearShell = (Button)findViewById(R.id.clear_shell);
        Button ctrlcShell = (Button)findViewById(R.id.ctrlc_shell);        
        clearShell.getBackground().setAlpha(100);
        ctrlcShell.getBackground().setAlpha(100);
        // Setup handlers.
        shellSend_ = (Button)findViewById(R.id.send_shell_request);
        shellSend_.setOnClickListener(sendListener_);
        clearShell.setOnClickListener(clearListener_);
        ctrlcShell.setOnClickListener(ctrlcListener_);
        // Configure shell output.
        shellContent_ = new ShellVector();
        adapterWrap_ = new ArrayAdapter<String>(context, R.layout.item_shell_wrap, shellContent_);
        adapterNoWrap_ = new ArrayAdapter<String>(context, R.layout.item_shell_nowrap, shellContent_);
        shellOutput_ = (ListView)findViewById(R.id.shell_output);                
        shellOutput_.setStackFromBottom(true);       
        shellOutput_.setDividerHeight(0);
        settingsChanged();
        postLogo();        
	}
	
	protected ArrayAdapter<String> getAdapter() {
		return Config.shellWrap_ ? adapterWrap_ : adapterNoWrap_;
	}
	
	protected void postLogo() {
		ArrayAdapter<String> adapter = getAdapter();
		adapter.add(" ");
		adapter.add("           G              G");               
		adapter.add("            G  GGGGGGGG  G");
		adapter.add("            GGGGGGGGGGGGGG");
		adapter.add("          GGG  GGGGGGGG  GGG");
		adapter.add("         GGGGGGGGGGGGGGGGGGGG");
		adapter.add(" ");
		adapter.add("        GGGGGGGGGGGGGGGGGGGGGG");           
		adapter.add("   G    GGGGGGGGGGGGGGGGGGGGGG    G");
		adapter.add("  GGG   GGGGGGGGGGGGGGGGGGGGGG   GGG");
		adapter.add("  GGGG  GGGGGGGGGGGGGGGGGGGGGG  GGGG");
		adapter.add("  GGGG  GGGGGGGGGGGGGGGGGGGGGG  GGGG");
		adapter.add("  GGGG  GGGGGGGGGGGGGGGGGGGGGG  GGGG");
		adapter.add("  GGGG  GGGGGGGGGGGGGGGGGGGGGG  GGGG");
		adapter.add("   G    GGGGGGGGGGGGGGGGGGGGGG    G");
		adapter.add("        GGGGGGGGGGGGGGGGGGGGGG");
		adapter.add("         GGGGGGGGGGGGGGGGGGGG");
		adapter.add("           GGGGG      GGGGG");
		adapter.add("           GGGGG      GGGGG");
		adapter.add("           GGGGG      GGGGG");
		adapter.add("            GGG        GGG");
		adapter.add(" ");
		adapter.add(" ");
		adapter.add(" iRemote Suite Shell");	    
		adapter.add(" Type your commands here...");
		adapter.add(" ");		
	}
	
	/**
	 * Provides additional tool to adjust vector size. 
	 */
	private class ShellVector extends Vector<String> {
		private static final long serialVersionUID = -5386947095420294255L;
		public void trim() { 
			this.removeRange(0, shellOverflowThreshold_);
		}
	}
}
