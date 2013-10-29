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

import com.scientificsoft.iremote.android.iremote.R;
import com.scientificsoft.iremote.server.hosts.Host;
import com.scientificsoft.iremote.server.hosts.HostsManager;
import com.scientificsoft.iremote.server.hosts.NetInterface;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.ExpandableListAdapter;
import android.widget.ExpandableListView;

public class HostsDialog extends AlertDialog implements IHostsListener {

	private ILoginListener loginListener_ = null;	
	
    private View content_ = null;
	
	private View hostsContent_ = null;
    
    private View noHostsContent_ = null; 
    
    private ExpandableListView hostsList_ = null;
    
	public HostsDialog(Context context, ILoginListener loginListener) {
		super(context);
		loginListener_ = loginListener;
        configureDialog();
        findKeyViews();		
	}

	protected void onStart () {
        if ( switchToProperContent() ) {
        	configureHostsList();
        	expandHosts();
        }				
	}
    
    private void configureDialog() {
		// Configure dialog.
		LayoutInflater factory = LayoutInflater.from(getContext());
		content_ = factory.inflate(R.layout.hosts_dialog, null);        
        setTitle("Known Hosts");
        setIcon(R.drawable.home_hosts);        
        setView(content_);        
        setCancelable(true);        
        
        // Add Cancel button.
        setButton(DialogInterface.BUTTON_POSITIVE, "Cancel", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
            }
        });
    }
    
    private void findKeyViews() {
        // Choose content to present.
        hostsContent_ = content_.findViewById(R.id.hosts_list_content);
        noHostsContent_ = content_.findViewById(R.id.no_hosts_content);
        hostsList_ = (ExpandableListView)hostsContent_.findViewById(R.id.hosts_list);
    }
    
    private boolean switchToProperContent() {
        if ( 0 == HostsManager.instance().hostsCount() ) {
        	hostsContent_.setVisibility(View.GONE);
        	noHostsContent_.setVisibility(View.VISIBLE);
        	hostsList_.setAdapter((ExpandableListAdapter)null);
        	return false;
        } else {
        	hostsContent_.setVisibility(View.VISIBLE);
        	noHostsContent_.setVisibility(View.GONE);
        	return true;
        }    	
    }
    
    private void configureHostsList() {
        // Configure the list.
    	hostsList_.setAdapter(new HostsAdapter(getContext(), this));
        
        // Add selection listener.
    	hostsList_.setOnChildClickListener(new ExpandableListView.OnChildClickListener() {
    		public boolean onChildClick (ExpandableListView parent, View v, int groupPosition, int childPosition, long id) {
    			// Hide host picker. 
    			HostsDialog.this.dismiss();
    			// Find picked interface.
    			Host host = HostsManager.instance().host(groupPosition);
    			NetInterface netInterface = host.interfaceAtIndex(childPosition);
    			// Initiate connection.
    			loginListener_.connect(netInterface.ip_, netInterface.port_);
    			return true;
    		}
        });        	
    }
    
    private void expandHosts() {
        // Expand all groups.
        ExpandableListView hostsList = (ExpandableListView)this.findViewById(R.id.hosts_list);
        ExpandableListAdapter listAdapter = hostsList.getExpandableListAdapter();
        if ( null != listAdapter ) {
	        int groupCount = listAdapter.getGroupCount();
	        for ( int i = 0 ; i < groupCount ; ++i )
	        	hostsList.expandGroup(i);
        }    	
    }
    
	public void showNoHosts() {
		switchToProperContent();
	}
}
