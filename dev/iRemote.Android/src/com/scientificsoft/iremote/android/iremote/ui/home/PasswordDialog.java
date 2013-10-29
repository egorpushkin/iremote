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

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.EditText;
import android.widget.TextView;

public class PasswordDialog {
	
	private static IPasswordListener passwordListener_ = null;
	
	private static EditText passwordField_ = null;

	public static Dialog create(Context context, boolean wrong, IPasswordListener passwordListener) {
		passwordListener_ = passwordListener;
		AlertDialog.Builder builder = new AlertDialog.Builder(context);
		
        // Load dialog content from resources.
		LayoutInflater factory = LayoutInflater.from(context);
        final View loginView = factory.inflate(R.layout.password_prompt, null);
        builder.setIcon(R.drawable.home_password);
        builder.setView(loginView);
        // Set dialog title.
        if ( wrong )
        	builder.setTitle(R.string.password_dialog_title_wrong);
        else
        	builder.setTitle(R.string.password_dialog_title);
        // Set dialog prompt.
        TextView prompt = (TextView)loginView.findViewById(R.id.prompt);
        if ( wrong )
        	prompt.setText(R.string.password_dialog_prompt_wrong);
        else
        	prompt.setText(R.string.password_dialog_prompt);        
        // Remember password field.
        passwordField_ = (EditText)loginView.findViewById(R.id.password);
        
        // Register OK/Cancel handlers.
        builder.setPositiveButton("Ok", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
            	passwordListener_.handle(PasswordDialog.passwordField_.getText().toString());
            	passwordField_.setText("");
            }
        });
        builder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
            	passwordListener_.cancel();
            	passwordField_.setText("");
            }
        });        
        
        // Register cancellation handler.
        builder.setOnCancelListener(new DialogInterface.OnCancelListener() {
            public void onCancel(DialogInterface dialog) {
            	passwordListener_.cancel();
            	passwordField_.setText("");
            }
        });        
        
        return builder.create();
	}
	
}
