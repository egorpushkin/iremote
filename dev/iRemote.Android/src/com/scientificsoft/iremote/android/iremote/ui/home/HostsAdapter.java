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

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.Button;
import android.widget.TextView;

import com.scientificsoft.iremote.android.iremote.R;
import com.scientificsoft.iremote.server.hosts.Host;
import com.scientificsoft.iremote.server.hosts.HostsManager;
import com.scientificsoft.iremote.server.hosts.NetInterface;

public class HostsAdapter extends BaseExpandableListAdapter {
	
	private IHostsListener hostsListener_ = null;
	
	private LayoutInflater inflater_ = null;
	
	private RemoveListener removeListener_ = new RemoveListener();
	
	public HostsAdapter(Context context, IHostsListener hostsListener) {
		hostsListener_ = hostsListener;
		inflater_ = LayoutInflater.from(context);
	}
    
	/**
	 * Prepares view to show network interface item.
	 */
    public View getChildView(int groupPosition, int childPosition, boolean isLastChild, View convertView, ViewGroup parent) {
        // A ViewHolder keeps references to children views to avoid unneccessary calls
        // to findViewById() on each row.
        ViewHolder holder = null;

        // When convertView is not null, we can reuse it directly, there is no need
        // to reinflate it. We only inflate a new View when the convertView supplied
        // by ListView is null.
        if (convertView == null) {
            convertView = inflater_.inflate(R.layout.net_interface_item, null);

            // Creates a ViewHolder and store references to the two children views
            // we want to bind data to.
            holder = new ViewHolder();
            holder.primary = (TextView)convertView.findViewById(R.id.host_item_primary);
            holder.secondary = (TextView)convertView.findViewById(R.id.host_item_secondary);
            holder.remove = (Button)convertView.findViewById(R.id.host_item_remove);

            // Set remove handler.
            holder.remove.setOnClickListener(removeListener_);

            // Remember holder for later access.
            convertView.setTag(holder);
            holder.remove.setTag(holder);
        } else {
            // Get the ViewHolder back to get fast access to the TextView
            // and the ImageView.
            holder = (ViewHolder)convertView.getTag();
        }
        
        // Remember indexes.
        holder.group = groupPosition;
        holder.child = childPosition;
        
        // Access data.
    	Host host = HostsManager.instance().host(groupPosition);
        NetInterface netInterface = host.interfaceAtIndex(childPosition);
        String accessTime = host.lastAccessTime();        

        // Bind the data efficiently with the holder.
        holder.primary.setText(netInterface.ip_ + ":" + netInterface.port_);
        holder.secondary.setText("Last accessed: " + ( ( null != accessTime ) ? accessTime : "never" ) );

        return convertView;
    }    
    
    /**
     * Handles 'remove' button clicks. 
     */
	private class RemoveListener implements View.OnClickListener {
	    public void onClick(View v) {
	    	try {
	    		ViewHolder holder = (ViewHolder)v.getTag();
	    		// Remove interface from data model.
	    		HostsManager.instance().removeInterface(holder.group, holder.child);
	    		// There is no sense in updating data, if there no more hosts
	    		// on the list.  
	    		if ( 0 != HostsManager.instance().hostsCount() ) {
	    			// Update view.
	    			HostsAdapter.this.notifyDataSetChanged();
	    		} else {
	    			// Switch to 'no hosts' content.
	    			hostsListener_.showNoHosts();
	    		}
	    	} catch ( Exception e ) {
	    	}
	    }
    }
	
    private static class ViewHolder {
    	public TextView primary = null;
    	public TextView secondary = null;
    	public Button remove = null;
    	public int group = -1;
    	public int child = -1;
    }
	
	/** Rest adapter methods. */
	
    public View getGroupView(int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
        if (convertView == null) {
            convertView = inflater_.inflate(R.layout.empty_separator, null);
        }
        return convertView;
    }
    
    public Object getChild(int groupPosition, int childPosition) {
    	Host host = HostsManager.instance().host(groupPosition);
        return host.interfaceAtIndex(childPosition);
    }

    public long getChildId(int groupPosition, int childPosition) {
        return childPosition;
    }

    public int getChildrenCount(int groupPosition) {
    	Host host = HostsManager.instance().host(groupPosition);
        int i = host.interfacesCount();
        return i;
    }

    public Object getGroup(int groupPosition) {
        return HostsManager.instance().host(groupPosition);
    }

    public int getGroupCount() {
        int i = HostsManager.instance().hostsCount();
        return i;
    }

    public long getGroupId(int groupPosition) {
        return groupPosition;
    }    
    
    public boolean isChildSelectable(int groupPosition, int childPosition) {
        return true;
    }

    public boolean hasStableIds() {
        return true;
    }    	
}
