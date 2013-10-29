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

#include "rememberuserdialog.h"
#include "ui_rememberuserdialog.h"

#include "UIConfig.h"

// UI tools
#include "Controls.h"

RememberUserDialog::RememberUserDialog(QWidget *parent)
	: QDialog(parent, RemotePC::UIConfig::alwaysOnTopdialogFlags_)
    , m_ui(new Ui::RememberUserDialog)
{
    m_ui->setupUi(this);
	m_ui->password_->setFocus();
}

RememberUserDialog::~RememberUserDialog()
{
    delete m_ui;
}

void RememberUserDialog::Close()
{
	done( Rejected );
}

void RememberUserDialog::remembered()
{
	if ( 0 == m_ui->password_->text().size() )
	{
		RemotePC::Controls::Warning(
			this,
			"Error in password field",
			"Password cannot be empty.");
		m_ui->password_->setFocus();
		return;
	}

	done( Accepted );
}

void RememberUserDialog::canceled()
{
	done( Rejected );
}

void RememberUserDialog::reject()
{
	// This dialog cannot be rejected.
}

QString RememberUserDialog::GetPassword()
{
	return m_ui->password_->text();
}

